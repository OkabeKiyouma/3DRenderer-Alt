#include "App.h"

#include <algorithm>
#include <list>

void App::Init(int screenWidth, int screenHeight) {
  this->ScreenHeight = screenHeight;
  this->ScreenWidth = screenWidth;
  // renderer.Init("Mid-Term Project", ScreenWidth, ScreenHeight);

  // Disable V-sync
  renderer.Init(
      "Mid-Term Project", ScreenWidth, ScreenHeight,
      (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI),
      (SDL_RendererFlags)(SDL_RENDERER_ACCELERATED));
}

void App::InitPipeline(int viewportWidth, int viewportHeight, Camera camera,
                       std::string modelPath, bool bHasTexture, float fNear,
                       float fFar, float fFov) {
  pipeline.model.LoadFromObjectFile(modelPath, bHasTexture);
  pipeline.model.texture =
      IMG_LoadTexture(renderer.renderer, pipeline.model.texFile.c_str());
  pipeline.camera = camera;
  pipeline.fNear = fNear;
  pipeline.fFar = fFar;
  pipeline.fFov = fFov;
  pipeline.viewportWidth = viewportWidth;
  pipeline.viewportHeight = viewportHeight;
  pipeline.fAspectRatio =
      (float)pipeline.viewportWidth / (float)pipeline.viewportHeight;
  pipeline.pDepthBuffer =
      new float[pipeline.viewportWidth * pipeline.viewportHeight];
}

void App::UpdatePipeline(Mat4 matWorld, Mat4 matView, Mat4 matProj) {
  this->pipeline.matWorld = matWorld;
  this->pipeline.matView = matView;
  this->pipeline.matProj = matProj;
}

void App::PollEvents() {
  while (SDL_PollEvent(&event)) {
    ImGui_ImplSDL2_ProcessEvent(&event);
    if (event.type == SDL_QUIT) isApplicationRunning = false;
    if (event.type == SDL_WINDOWEVENT &&
        event.window.event == SDL_WINDOWEVENT_CLOSE &&
        event.window.windowID == SDL_GetWindowID(renderer.window))
      isApplicationRunning = false;
    if (event.type == SDL_WINDOWEVENT) {
      float factorx = pipeline.viewportWidth / (float)ScreenWidth;
      float factory = pipeline.viewportHeight / (float)ScreenHeight;
      SDL_GetWindowSize(renderer.window, &ScreenWidth, &ScreenHeight);
      pipeline.viewportWidth = factorx * ScreenWidth;
      pipeline.viewportHeight = factory * ScreenHeight;
      pipeline.fAspectRatio =
          (float)pipeline.viewportWidth / (float)pipeline.viewportHeight;
      if (pipeline.pDepthBuffer) delete[] pipeline.pDepthBuffer;
      pipeline.pDepthBuffer =
          new float[pipeline.viewportWidth * pipeline.viewportHeight];
      SDL_SetWindowSize(renderer.window, ScreenWidth, ScreenHeight);
    }
    if (event.type == SDL_KEYDOWN) HandleInputs(event, 25.f);
  }
}

void App::NewFrame() {
  // Start the Dear ImGui frame
  ImGui_ImplSDLRenderer_NewFrame();
  ImGui_ImplSDL2_NewFrame();
  ImGui::NewFrame();
}

void App::Update() {
  {
    static float f = 0.0f;
    static int counter = 0;

    ImGui::SetNextWindowPos(ImVec2((0.8f * ScreenWidth), 0));
    ImGui::SetNextWindowSize(ImVec2((0.2f * ScreenWidth), ScreenHeight));
    ImGui::Begin("World Variables");
    ImGui::Text("This is some useful text.");
    ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
    ImGui::ColorEdit3("Background\nColor", (float*)&clearColor);

    if (ImGui::Button("Button")) counter++;
    ImGui::SameLine();
    ImGui::Text("counter = %d", counter);
    ImGui::Text("Application average \n%.3f ms/frame (%.1f FPS)",
                1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::End();
  }

  renderer.Clear(clearColor);
  SDL_SetRenderDrawColor(renderer.renderer, 255, 255, 255, 255);

  std::vector<FinalTriangle> vecTrianglesToRaster;
  for (auto& tri : pipeline.model.tris) {
    Triangle triTransformed, triViewed, triProjected;

    // World Co-ordinate

    triTransformed.p[0] = pipeline.matWorld * tri.p[0];
    triTransformed.p[1] = pipeline.matWorld * tri.p[1];
    triTransformed.p[2] = pipeline.matWorld * tri.p[2];
    triTransformed.tex[0] = tri.tex[0];
    triTransformed.tex[1] = tri.tex[1];
    triTransformed.tex[2] = tri.tex[2];

    // View Co-ordinate

    triViewed.p[0] = pipeline.matView * triTransformed.p[0];
    triViewed.p[1] = pipeline.matView * triTransformed.p[1];
    triViewed.p[2] = pipeline.matView * triTransformed.p[2];
    triViewed.tex[0] = triTransformed.tex[0];
    triViewed.tex[1] = triTransformed.tex[1];
    triViewed.tex[2] = triTransformed.tex[2];

    Vec3 normal;
    Vec4 line1, line2;
    line1 = triViewed.p[1] - triViewed.p[0];
    line2 = triViewed.p[2] - triViewed.p[0];

    normal = CrossProduct(Vec3(line1), Vec3(line2));
    normal = NormalizeChecked(normal);

    // float decision =  // DotProduct(normal, (Vec3(-camera.Front)));
    float decision =
        DotProduct(normal, Vec3(triViewed.p[0]) - pipeline.camera.Position);
    //  DotProduct(normal, {0.f,0.f,-1.f});
    if (decision < 0.f) {
      // Color Generation
      //{
      Vec3 lightDirection = {0.0f, 1.0f, -1.0f};
      lightDirection = NormalizeChecked(lightDirection);

      float dotProduct = Maximum(0.0f, DotProduct(lightDirection, normal));

      Vec4 color[3];
      color[0] =
          Vec4(Vec3{255 * dotProduct, 255 * dotProduct, 255 * dotProduct}, 255);
      color[1] =
          Vec4(Vec3{255 * dotProduct, 255 * dotProduct, 255 * dotProduct}, 255);
      color[2] =
          Vec4(Vec3{255 * dotProduct, 255 * dotProduct, 255 * dotProduct}, 255);

      int nClippedTriangles = 0;
      Triangle clipped[2];
      nClippedTriangles = TriangleClipAgainstPlane(
          Vec4(pipeline.camera.Position + Vec3{0.0f, 0.0f, 0.0f}),
          Vec4(NormalizeChecked(-pipeline.camera.Front)), triViewed, clipped[0],
          clipped[1]);
      for (int n = 0; n < nClippedTriangles; n++) {
        // Perspective Projection

        triProjected.p[0] = pipeline.matProj * clipped[n].p[0];
        triProjected.p[1] = pipeline.matProj * clipped[n].p[1];
        triProjected.p[2] = pipeline.matProj * clipped[n].p[2];
        triProjected.tex[0] = clipped[n].tex[0];
        triProjected.tex[1] = clipped[n].tex[1];
        triProjected.tex[2] = clipped[n].tex[2];

        // Perspective Division
        triProjected.tex[0] /= triProjected.p[0].w;
        triProjected.tex[1] /= triProjected.p[1].w;
        triProjected.tex[2] /= triProjected.p[2].w;
        triProjected.p[0] /= triProjected.p[0].w;
        triProjected.p[1] /= triProjected.p[1].w;
        triProjected.p[2] /= triProjected.p[2].w;

        triProjected.p[0].x = -triProjected.p[0].x;
        triProjected.p[1].x = -triProjected.p[1].x;
        triProjected.p[2].x = -triProjected.p[2].x;
        triProjected.p[0].y = -triProjected.p[0].y;
        triProjected.p[1].y = -triProjected.p[1].y;
        triProjected.p[2].y = -triProjected.p[2].y;

        triProjected.p[0].z = (pipeline.fFar + pipeline.fNear) /
                                  (pipeline.fFar - pipeline.fNear) +
                              1 / clipped[n].p[0].z *
                                  (-2 * pipeline.fFar * pipeline.fNear /
                                   (pipeline.fFar - pipeline.fNear));
        triProjected.p[1].z = (pipeline.fFar + pipeline.fNear) /
                                  (pipeline.fFar - pipeline.fNear) +
                              1 / clipped[n].p[1].z *
                                  (-2 * pipeline.fFar * pipeline.fNear /
                                   (pipeline.fFar - pipeline.fNear));
        triProjected.p[2].z = (pipeline.fFar + pipeline.fNear) /
                                  (pipeline.fFar - pipeline.fNear) +
                              1 / clipped[n].p[2].z *
                                  (-2 * pipeline.fFar * pipeline.fNear /
                                   (pipeline.fFar - pipeline.fNear));

        ConvertNDCToViewportPixels(triProjected, pipeline.viewportWidth,
                                   pipeline.viewportHeight);

        vecTrianglesToRaster.push_back(FinalTriangle{
            triProjected.p[0], triProjected.p[1], triProjected.p[2],
            triProjected.tex[0], triProjected.tex[1], triProjected.tex[2],
            color[0], color[1], color[2]});
      }
    }
  }
  sort(vecTrianglesToRaster.begin(), vecTrianglesToRaster.end(),
       [](FinalTriangle& t1, FinalTriangle& t2) {
         float z1 = (t1.p[0].z + t1.p[1].z + t1.p[2].z) / 3.0f;
         float z2 = (t2.p[0].z + t2.p[1].z + t2.p[2].z) / 3.0f;
         return z1 > z2;
       });

  // Clear Depth Buffer
  for (int i = 0; i < pipeline.viewportWidth * pipeline.viewportHeight; ++i)
    pipeline.pDepthBuffer[i] = -10000000.f;

  for (auto& triProjected : vecTrianglesToRaster) {
    // Clip triangles against all four screen edges, this could yield
    // a bunch of triangles, so create a queue that we traverse to
    //  ensure we only test new triangles generated against planes
    Triangle clipped[2];
    std::list<Triangle> listTriangles;

    // Add initial triangle
    listTriangles.push_back({triProjected.p[0], triProjected.p[1],
                             triProjected.p[2], triProjected.tex[0],
                             triProjected.tex[1], triProjected.tex[2]});
    int nNewTriangles = 1;

    for (int p = 0; p < 4; p++) {
      int nTrisToAdd = 0;
      while (nNewTriangles > 0) {
        // Take triangle from front of queue
        Triangle test = listTriangles.front();
        listTriangles.pop_front();
        nNewTriangles--;

        // Clip it against a plane. We only need to test each
        // subsequent plane, against subsequent new triangles
        // as all triangles after a plane clip are guaranteed
        // to lie on the inside of the plane. I like how this
        // comment is almost completely and utterly justified
        switch (p) {
          case 0:
            nTrisToAdd = TriangleClipAgainstPlane(Vec4({0.0f, 0.0f, 0.0f}),
                                                  Vec4({0.0f, 1.0f, 0.0f}),
                                                  test, clipped[0], clipped[1]);
            break;
          case 1:
            nTrisToAdd = TriangleClipAgainstPlane(
                Vec4({0.0f, (float)pipeline.viewportHeight - 1, 0.0f}),
                Vec4({0.0f, -1.0f, 0.0f}), test, clipped[0], clipped[1]);
            break;
          case 2:
            nTrisToAdd = TriangleClipAgainstPlane(Vec4({0.0f, 0.0f, 0.0f}),
                                                  Vec4({1.0f, 0.0f, 0.0f}),
                                                  test, clipped[0], clipped[1]);
            break;
          case 3:
            nTrisToAdd = TriangleClipAgainstPlane(
                Vec4({(float)pipeline.viewportWidth - 1, 0.0f, 0.0f}),
                Vec4({-1.0f, 0.0f, 0.0f}), test, clipped[0], clipped[1]);
            break;
        }

        // Clipping may yield a variable number of triangles, so
        // add these new ones to the back of the queue for subsequent
        // clipping against next planes
        for (int w = 0; w < nTrisToAdd; w++)
          listTriangles.push_back(clipped[w]);
      }
      nNewTriangles = listTriangles.size();
    }
    for (auto& t : listTriangles) {
      SDL_Vertex vert[3];
      vert[0].position.x = t.p[0].x;
      vert[0].position.y = t.p[0].y;
      vert[0].tex_coord.x = t.tex[0].x / t.tex[0].z;
      vert[0].tex_coord.y = t.tex[0].y / t.tex[0].z;
      vert[0].color.r = triProjected.col[0].r;
      vert[0].color.g = triProjected.col[0].g;
      vert[0].color.b = triProjected.col[0].b;
      vert[0].color.a = triProjected.col[0].a;

      vert[1].position.x = t.p[1].x;
      vert[1].position.y = t.p[1].y;
      vert[1].tex_coord.x = t.tex[1].x / t.tex[1].z;
      vert[1].tex_coord.y = t.tex[1].y / t.tex[1].z;
      vert[1].color.r = triProjected.col[1].r;
      vert[1].color.g = triProjected.col[1].g;
      vert[1].color.b = triProjected.col[1].b;
      vert[1].color.a = triProjected.col[1].a;

      vert[2].position.x = t.p[2].x;
      vert[2].position.y = t.p[2].y;
      vert[2].tex_coord.x = t.tex[2].x / t.tex[2].z;
      vert[2].tex_coord.y = t.tex[2].y / t.tex[2].z;
      vert[2].color.r = triProjected.col[2].r;
      vert[2].color.g = triProjected.col[2].g;
      vert[2].color.b = triProjected.col[2].b;
      vert[2].color.a = triProjected.col[2].a;

      FinalTriangle tri;
      tri.p[0].x = t.p[0].x;
      tri.p[0].y = t.p[0].y;
      tri.p[0].z = t.p[0].z;
      tri.p[0].w = t.p[0].w;
      tri.tex[0].x = t.tex[0].x;
      tri.tex[0].y = t.tex[0].y;
      tri.tex[0].z = t.tex[0].z;
      tri.col[0].r = 255;  // triProjected.col[0].r;
      tri.col[0].g = 000;  // triProjected.col[0].g;
      tri.col[0].b = 000;  // triProjected.col[0].b;
      tri.col[0].a = 255;  // triProjected.col[0].a;

      tri.p[1].x = t.p[1].x;
      tri.p[1].y = t.p[1].y;
      tri.p[1].z = t.p[1].z;
      tri.p[1].w = t.p[1].w;
      tri.tex[1].x = t.tex[1].x;
      tri.tex[1].y = t.tex[1].y;
      tri.tex[1].z = t.tex[1].z;
      tri.col[1].r = 000;  // triProjected.col[1].r;
      tri.col[1].g = 255;  // triProjected.col[1].g;
      tri.col[1].b = 000;  // triProjected.col[1].b;
      tri.col[1].a = 255;  // triProjected.col[1].a;

      tri.p[2].x = t.p[2].x;
      tri.p[2].y = t.p[2].y;
      tri.p[2].z = t.p[2].z;
      tri.p[2].w = t.p[2].w;
      tri.tex[2].x = t.tex[2].x;
      tri.tex[2].y = t.tex[2].y;
      tri.tex[2].z = t.tex[2].z;
      tri.col[2].r = 000;  // triProjected.col[2].r;
      tri.col[2].g = 000;  // triProjected.col[2].g;
      tri.col[2].b = 255;  // triProjected.col[2].b;
      tri.col[2].a = 255;  // triProjected.col[2].a;
                           // TexturedTriangle(tri);
      // SDL_RenderGeometry(renderer.renderer, pipeline.model.texture, vert, 3,
      //                    nullptr, 0);
      SDL_RenderGeometry(renderer.renderer, nullptr, vert, 3, nullptr, 0);
      SDL_SetRenderDrawColor(renderer.renderer, 0, 0, 0, 0);
      SDL_RenderDrawLineF(renderer.renderer, t.p[0].m[0], t.p[0].m[1],
                          t.p[1].m[0], t.p[1].m[1]);
      SDL_RenderDrawLineF(renderer.renderer, t.p[1].m[0], t.p[1].m[1],
                          t.p[2].m[0], t.p[2].m[1]);
      SDL_RenderDrawLineF(renderer.renderer, t.p[2].m[0], t.p[2].m[1],
                          t.p[0].m[0], t.p[0].m[1]);
    }
  }

  // SDL_RenderDrawLine(renderer.renderer, 100, 100, 200, 200);
  renderer.Present();
}

void App::HandleInputs(SDL_Event& event, Uint64 deltaTime) {
  switch (event.key.keysym.sym) {
    case SDLK_ESCAPE:
      isApplicationRunning = false;
      break;
    case SDLK_w:
      pipeline.camera.ProcessKeyboard(FORWARD, deltaTime);
      // pipeline.camera.Position += pipeline.camera.Front * deltaTime / 5.f;
      // pipeline.camera.updateCameraVectors();
      break;
    case SDLK_s:
      pipeline.camera.ProcessKeyboard(BACKWARD, deltaTime);
      // pipeline.camera.Position -= pipeline.camera.Front * deltaTime / 5.f;
      // pipeline.camera.updateCameraVectors();
      break;
    case SDLK_a:
      pipeline.camera.ProcessKeyboard(LEFT, deltaTime);
      // pipeline.camera.Yaw -= deltaTime / 5.f;
      // pipeline.camera.updateCameraVectors();
      break;
    case SDLK_d:
      pipeline.camera.ProcessKeyboard(RIGHT, deltaTime);
      // pipeline.camera.Yaw += deltaTime / 5.f;
      // pipeline.camera.updateCameraVectors();
      break;
    case SDLK_LEFT:
      // pipeline.camera.ProcessMouseMovement(-100 *
      // pipeline.camera.MovementSpeed * deltaTime, 0.f,
      //                             true);
      pipeline.camera.Position.x -= deltaTime / 5.f;
      break;
    case SDLK_RIGHT:
      // pipeline.camera.ProcessMouseMovement(100 *
      // pipeline.camera.MovementSpeed * deltaTime, 0.f,
      //                             true);
      pipeline.camera.Position.x += deltaTime / 5.f;
      break;
    case SDLK_UP:
      pipeline.camera.Position.y += deltaTime / 5.f;
      // pipeline.camera.ProcessMouseMovement(0.f, 100 *
      // pipeline.camera.MovementSpeed * deltaTime,
      //                             true);
      break;
    case SDLK_DOWN:
      pipeline.camera.Position.y -= deltaTime / 5.f;
      // pipeline.camera.ProcessMouseMovement(0.f, -100 *
      // pipeline.camera.MovementSpeed * deltaTime,
      //                             true);
      break;
      break;
  }
  if (event.type == SDL_MOUSEMOTION) {
    pipeline.camera.ProcessMouseMovement(event.motion.xrel, event.motion.yrel,
                                         true);
  }
}

void App::TexturedTriangle(FinalTriangle t) {
  auto swap = [](Vec4& p1, Vec4& p2) {
    Vec4 temp = p1;
    p1 = p2;
    p2 = temp;
  };

  auto swap3 = [](Vec3& p1, Vec3& p2) {
    Vec3 temp = p1;
    p1 = p2;
    p2 = temp;
  };

  auto swapf = [](float& a, float& b) {
    float temp = a;
    a = b;
    b = temp;
  };

  auto swapi = [](int& a, int& b) {
    int temp = a;
    a = b;
    b = temp;
  };

  if (t.p[1].y < t.p[0].y) {
    swap(t.p[0], t.p[1]);
    swap(t.col[0], t.col[1]);
    swap3(t.tex[0], t.tex[1]);
  }

  if (t.p[2].y < t.p[0].y) {
    swap(t.p[0], t.p[2]);
    swap(t.col[0], t.col[2]);
    swap3(t.tex[0], t.tex[2]);
  }

  if (t.p[2].y < t.p[1].y) {
    swap(t.p[1], t.p[2]);
    swap(t.col[1], t.col[2]);
    swap3(t.tex[1], t.tex[2]);
  }

  int dy1 = int(t.p[1].y) - int(t.p[0].y);
  int dx1 = int(t.p[1].x) - int(t.p[0].x);
  float dz1 = t.p[1].z - t.p[0].z;
  float dr1 = t.col[1].r - t.col[0].r;
  float dg1 = t.col[1].g - t.col[0].g;
  float db1 = t.col[1].b - t.col[0].b;
  float da1 = t.col[1].a - t.col[0].a;
  float du1 = t.tex[1].x - t.tex[0].x;
  float dv1 = t.tex[1].y - t.tex[0].y;
  float dw1 = t.tex[1].z - t.tex[0].z;

  int dy2 = int(t.p[2].y) - int(t.p[0].y);
  int dx2 = int(t.p[2].x) - int(t.p[0].x);
  float dz2 = t.p[2].z - t.p[0].z;
  float dr2 = t.col[2].r - t.col[0].r;
  float dg2 = t.col[2].g - t.col[0].g;
  float db2 = t.col[2].b - t.col[0].b;
  float da2 = t.col[2].a - t.col[0].a;
  float du2 = t.tex[2].x - t.tex[0].x;
  float dv2 = t.tex[2].y - t.tex[0].y;
  float dw2 = t.tex[2].z - t.tex[0].z;

  float tex_u, tex_v, tex_w, pos_z, col_r, col_g, col_b, col_a;

  float dx1_step = 0, dx2_step = 0, dz1_step = 0, dz2_step = 0, dr1_step = 0,
        dr2_step = 0, dg1_step = 0, dg2_step = 0, db1_step = 0, db2_step = 0,
        da1_step = 0, da2_step = 0, du1_step = 0, dv1_step = 0, du2_step = 0,
        dv2_step = 0, dw1_step = 0, dw2_step = 0;

  if (dy1) {
    dx1_step = dx1 / (float)abs(dy1);
    dz1_step = dz1 / (float)abs(dy1);
    dr1_step = dr1 / (float)abs(dy1);
    dg1_step = dg1 / (float)abs(dy1);
    db1_step = db1 / (float)abs(dy1);
    da1_step = da1 / (float)abs(dy1);
    du1_step = du1 / (float)abs(dy1);
    dv1_step = dv1 / (float)abs(dy1);
    dw1_step = dw1 / (float)abs(dy1);
  }

  if (dy2) {
    dx2_step = dx2 / (float)abs(dy2);
    dz2_step = dz2 / (float)abs(dy2);
    dr2_step = dr2 / (float)abs(dy2);
    dg2_step = dg2 / (float)abs(dy2);
    db2_step = db2 / (float)abs(dy2);
    da2_step = da2 / (float)abs(dy2);

    du2_step = du2 / (float)abs(dy2);
    dv2_step = dv2 / (float)abs(dy2);
    dw2_step = dw2 / (float)abs(dy2);
  }

  if (dy1) {
    for (int i = t.p[0].y; i <= int(t.p[1].y); i++) {
      int sx = int(t.p[0].x) + (float)(i - int(t.p[0].y)) * dx1_step;
      int ex = int(t.p[0].x) + (float)(i - int(t.p[0].y)) * dx2_step;

      float sz = t.p[0].z + (float)(i - int(t.p[0].y)) * dz1_step;
      float ez = t.p[0].z + (float)(i - int(t.p[0].y)) * dz2_step;

      float col_sr = t.col[0].r + (float)(i - t.p[0].y) * dr1_step;
      float col_sg = t.col[0].g + (float)(i - t.p[0].y) * dg1_step;
      float col_sb = t.col[0].b + (float)(i - t.p[0].y) * db1_step;
      float col_sa = t.col[0].a + (float)(i - t.p[0].y) * da1_step;

      float col_er = t.col[0].r + (float)(i - t.p[0].y) * dr2_step;
      float col_eb = t.col[0].g + (float)(i - t.p[0].y) * dg2_step;
      float col_eg = t.col[0].b + (float)(i - t.p[0].y) * db2_step;
      float col_ea = t.col[0].a + (float)(i - t.p[0].y) * da2_step;

      float tex_su = t.tex[0].x + (float)(i - t.p[0].y) * du1_step;
      float tex_sv = t.tex[0].y + (float)(i - t.p[0].y) * dv1_step;
      float tex_sw = t.tex[0].z + (float)(i - t.p[0].y) * dw1_step;

      float tex_eu = t.tex[0].x + (float)(i - t.p[0].y) * du2_step;
      float tex_ev = t.tex[0].y + (float)(i - t.p[0].y) * dv2_step;
      float tex_ew = t.tex[0].z + (float)(i - t.p[0].y) * dw2_step;

      if (sx > ex) {
        swapi(sx, ex);
        swapf(sz, ez);
        swapf(col_sa, col_er);
        swapf(col_sg, col_eg);
        swapf(col_sb, col_eb);
        swapf(col_sa, col_ea);
        swapf(tex_su, tex_eu);
        swapf(tex_sv, tex_ev);
        swapf(tex_sw, tex_ew);
      }

      tex_u = tex_su;
      tex_v = tex_sv;
      tex_w = tex_sw;
      col_r = col_sr;
      col_g = col_sg;
      col_b = col_sb;
      col_a = col_sa;
      pos_z = sz;

      float tstep = 1.0f / ((float)(ex - sx));
      float t = 0.0f;

      for (int j = sx; j < ex; j++) {
        tex_u = (1.0f - t) * tex_su + t * tex_eu;
        tex_v = (1.0f - t) * tex_sv + t * tex_ev;
        tex_w = (1.0f - t) * tex_sw + t * tex_ew;
        col_r = (1.0f - t) * col_sr + t * col_er;
        col_g = (1.0f - t) * col_sg + t * col_eg;
        col_b = (1.0f - t) * col_sb + t * col_eb;
        col_a = (1.0f - t) * col_sa + t * col_ea;
        pos_z = (1.0 - t) * sz + t * ez;

        // if (1) {
        if (pos_z > pipeline.pDepthBuffer[i * pipeline.viewportWidth + j]) {
          SDL_SetTextureColorMod(pipeline.model.texture, col_r, col_r, col_r);
          // SDL_SetTextureColorMod(texture, 255, 255, 255);
          SDL_Rect src = {tex_u / tex_w * 1920, tex_v / tex_w * 1080, 1, 1};
          SDL_Rect dest = {j, i, 1, 1};
          // SDL_RenderCopy(renderer.renderer, pipeline.model.texture, &src,
          //                &dest);
          SDL_SetRenderDrawColor(renderer.renderer, col_r, col_g, col_b, 255);
          SDL_RenderDrawPoint(renderer.renderer, j, i);
          pipeline.pDepthBuffer[i * pipeline.viewportWidth + j] = pos_z;
        }
        t += tstep;
      }
    }
  }

  dy1 = int(t.p[2].y) - int(t.p[1].y);
  dx1 = int(t.p[2].x) - int(t.p[1].x);
  dz1 = t.p[2].z - t.p[1].z;
  dr1 = t.col[2].r - t.col[1].r;
  dg1 = t.col[2].g - t.col[1].g;
  db1 = t.col[2].b - t.col[1].b;
  da1 = t.col[2].a - t.col[1].a;
  du1 = t.tex[2].x - t.tex[1].x;
  dv1 = t.tex[2].y - t.tex[1].y;
  dw1 = t.tex[2].z - t.tex[1].z;

  if (dy1) dx1_step = dx1 / (float)abs(dy1);
  if (dy2) dx2_step = dx2 / (float)abs(dy2);

  du1_step = 0, dv1_step = 0, dz1_step = 0, dr1_step = 0, db1_step = 0,
  dg1_step = 0, da1_step = 0;
  if (dy1) {
    dz1_step = dz1 / (float)abs(dy1);
    dr1_step = dr1 / (float)abs(dy1);
    dg1_step = dg1 / (float)abs(dy1);
    db1_step = db1 / (float)abs(dy1);
    da1_step = da1 / (float)abs(dy1);
    du1_step = du1 / (float)abs(dy1);
    dv1_step = dv1 / (float)abs(dy1);
    dw1_step = dw1 / (float)abs(dy1);

    for (int i = t.p[1].y; i <= int(t.p[2].y); i++) {
      int ax = int(t.p[1].x) + (float)(i - int(t.p[1].y)) * dx1_step;
      int bx = int(t.p[0].x) + (float)(i - int(t.p[0].y)) * dx2_step;

      float sz = t.p[1].z + (float)(i - int(t.p[1].y)) * dz1_step;
      float col_sr = t.col[1].r + (float)(i - t.p[1].y) * du1_step;
      float col_sg = t.col[1].g + (float)(i - t.p[1].y) * dv1_step;
      float col_sb = t.col[1].b + (float)(i - t.p[1].y) * dw1_step;
      float col_sa = t.col[1].a + (float)(i - t.p[1].y) * dw1_step;
      float tex_su = t.tex[1].x + (float)(i - t.p[1].y) * du1_step;
      float tex_sv = t.tex[1].y + (float)(i - t.p[1].y) * dv1_step;
      float tex_sw = t.tex[1].z + (float)(i - t.p[1].y) * dw1_step;

      float ez = t.p[0].z + (float)(i - int(t.p[0].y)) * dz2_step;
      float col_er = t.col[0].r + (float)(i - t.p[0].y) * du1_step;
      float col_eg = t.col[0].g + (float)(i - t.p[0].y) * dv1_step;
      float col_eb = t.col[0].b + (float)(i - t.p[0].y) * dw1_step;
      float col_ea = t.col[0].a + (float)(i - t.p[0].y) * dw1_step;
      float tex_eu = t.tex[0].x + (float)(i - t.p[0].y) * du2_step;
      float tex_ev = t.tex[0].y + (float)(i - t.p[0].y) * dv2_step;
      float tex_ew = t.tex[0].z + (float)(i - t.p[0].y) * dw2_step;

      if (ax > bx) {
        swapi(ax, bx);
        swapf(sz, ez);
        swapf(col_sr, col_er);
        swapf(col_sg, col_eg);
        swapf(col_sb, col_eb);
        swapf(col_sa, col_ea);
        swapf(tex_su, tex_eu);
        swapf(tex_sv, tex_ev);
        swapf(tex_sw, tex_ew);
      }

      col_r = col_sr;
      col_g = col_sg;
      col_b = col_sb;
      col_a = col_sa;
      tex_u = tex_su;
      tex_v = tex_sv;
      tex_w = tex_sw;
      pos_z = sz;

      float tstep = 1.0f / ((float)(bx - ax));
      float t = 0.0f;

      for (int j = ax; j < bx; j++) {
        col_r = (1.0f - t) * col_sr + t * col_er;
        col_g = (1.0f - t) * col_sg + t * col_eg;
        col_b = (1.0f - t) * col_sb + t * col_eb;
        col_a = (1.0f - t) * col_sa + t * col_ea;
        tex_u = (1.0f - t) * tex_su + t * tex_eu;
        tex_v = (1.0f - t) * tex_sv + t * tex_ev;
        tex_w = (1.0f - t) * tex_sw + t * tex_ew;
        pos_z = (1.0 - t) * sz + t * ez;

        // if (1) {
        if (pos_z > pipeline.pDepthBuffer[i * pipeline.viewportWidth + j]) {
          SDL_SetTextureColorMod(pipeline.model.texture, col_r, col_r, col_r);
          // SDL_SetTextureColorMod(texture, 255, 255, 255);
          SDL_Rect src = {tex_u / tex_w * 1920, tex_v / tex_w * 1080, 1, 1};
          SDL_Rect dest = {j, i, 1, 1};
          SDL_SetRenderDrawColor(renderer.renderer, col_r, col_g, col_b, 255);
          SDL_RenderDrawPoint(renderer.renderer, j, i);
          // SDL_RenderCopy(renderer.renderer, pipeline.model.texture, &src,
          //               &dest);
          pipeline.pDepthBuffer[i * pipeline.viewportWidth + j] = pos_z;
        }
        t += tstep;
      }
    }
  }
}
