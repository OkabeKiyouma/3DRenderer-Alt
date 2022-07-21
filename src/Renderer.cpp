#include "Renderer.h"

#include <algorithm>
#include <list>

void Renderer::Init(const char* windowName, int width, int height,
                    SDL_WindowFlags window_flags,
                    SDL_RendererFlags renderer_flags) {
#if !SDL_VERSION_ATLEAST(2, 0, 17)
#error This backend requires SDL 2.0.17+ because of SDL_RenderGeometry() function
#endif

  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) !=
      0) {
    printf("Error: %s\n", SDL_GetError());
    exit(1);
  }

  winName = windowName;

  window =
      SDL_CreateWindow(windowName, SDL_WINDOWPOS_CENTERED,
                       SDL_WINDOWPOS_CENTERED, width, height, window_flags);

  renderer = SDL_CreateRenderer(window, -1, renderer_flags);
  if (renderer == NULL) {
    SDL_Log("Error creating SDL_Renderer!");
    exit(1);
  }
  // SDL_RendererInfo info;
  // SDL_GetRendererInfo(renderer, &info);
  // SDL_Log("Current SDL_Renderer: %s", info.name);

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  io = ImGui::GetIO();
  (void)io;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  // Enable Keyboard Controls
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
  // Enable Gamepad Controls
  io.Fonts->AddFontFromFileTTF("res/fonts/DroidSans.ttf", 16.f);

  ImGui::StyleColorsDark();
  // ImGui::StyleColorsClassic();

  // Setup Platform/Renderer backends
  ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
  ImGui_ImplSDLRenderer_Init(renderer);

  this->ScreenWidth = width;
  this->ScreenHeight = height;
}

void Renderer::Clear(Vec4 color) {
  SDL_SetRenderDrawColor(renderer, (Uint8)(color.x * 255),
                         (Uint8)(color.y * 255), (Uint8)(color.z * 255),
                         (Uint8)(color.w * 255));
  SDL_RenderClear(renderer);
}

void Renderer::PollEvents() {
  while (SDL_PollEvent(&event)) {
    ImGui_ImplSDL2_ProcessEvent(&event);
    if (event.type == SDL_QUIT) isApplicationRunning = false;
    if (event.type == SDL_WINDOWEVENT &&
        event.window.event == SDL_WINDOWEVENT_CLOSE &&
        event.window.windowID == SDL_GetWindowID(window))
      isApplicationRunning = false;
    if (event.type == SDL_WINDOWEVENT) {
      float factorx = pipeline.viewportWidth / (float)ScreenWidth;
      float factory = pipeline.viewportHeight / (float)ScreenHeight;
      SDL_GetWindowSize(window, &ScreenWidth, &ScreenHeight);
      pipeline.viewportWidth = factorx * ScreenWidth;
      pipeline.viewportHeight = factory * ScreenHeight;
      pipeline.fAspectRatio =
          (float)pipeline.viewportWidth / (float)pipeline.viewportHeight;
      if (pipeline.pDepthBuffer) delete[] pipeline.pDepthBuffer;
      pipeline.pDepthBuffer =
          new float[pipeline.viewportWidth * pipeline.viewportHeight];
      SDL_SetWindowSize(window, ScreenWidth, ScreenHeight);
    }
    // if (event.type == SDL_KEYDOWN || event.type == SDL_MOUSEMOTION)
    //   HandleInputs(event, 25.f);
    if (event.type == SDL_KEYDOWN) HandleInputs(event, 25.f);
  }
}

void Renderer::HandleInputs(SDL_Event& event, Uint64 deltaTime) {
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

void Renderer::NewFrame() {
  // Start the Dear ImGui frame
  ImGui_ImplSDLRenderer_NewFrame();
  ImGui_ImplSDL2_NewFrame();
  ImGui::NewFrame();
  static float f = 0.0f;

  ImGui::SetNextWindowPos(ImVec2((0.8f * ScreenWidth), 0));
  ImGui::SetNextWindowSize(ImVec2((0.2f * ScreenWidth), ScreenHeight));
  ImGui::Begin(
      "World Variables", 0,
      ImGuiWindowFlags_NoResize | ImGuiWindowFlags_HorizontalScrollbar);
  ImGui::Checkbox("Wireframe", &pipeline.wireFrame);
  ImGui::SliderFloat3("Light\nPosition", pipeline.light.Pos.m, -10.f, 10.f);
  ImGui::SliderFloat("Ambient\nStength", &pipeline.light.Ka, 0.f, 1.f);
  ImGui::SliderFloat("Diffuse\nStength", &pipeline.light.Kd, 0.f, 1.f);
  ImGui::SliderFloat("Specular\nStength", &pipeline.light.Ks, 0.f, 1.f);
  ImGui::SliderInt("Alpha", &pipeline.light.alpha, 1, 32);
  ImGui::Text("Application average \n%.3f ms/frame (%.1f FPS)",
              1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
  ImGui::End();
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
}

void Renderer::Present() {
  // might need to put back in main loop
  SDL_SetWindowTitle(
      window,
      (winName + "- " + std::to_string((int)ImGui::GetIO().Framerate) + " FPS")
          .c_str());
  ImGui::Render();

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
                             triProjected.tex[1], triProjected.tex[2],
                             triProjected.texUnit});
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

      // Delete in final version
      FinalTriangle tri;
      {
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
        tri.texUnit = t.texUnit;
        // TexturedTriangle(tri);
      }
      if (t.texUnit != -1)
        SDL_RenderGeometry(renderer, pipeline.textureUnit[t.texUnit], vert, 3,
                           nullptr, 0);
      else
        SDL_RenderGeometry(renderer, nullptr, vert, 3, nullptr, 0);
      if (pipeline.wireFrame) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderDrawLineF(renderer, t.p[0].m[0], t.p[0].m[1], t.p[1].m[0],
                            t.p[1].m[1]);
        SDL_RenderDrawLineF(renderer, t.p[1].m[0], t.p[1].m[1], t.p[2].m[0],
                            t.p[2].m[1]);
        SDL_RenderDrawLineF(renderer, t.p[2].m[0], t.p[2].m[1], t.p[0].m[0],
                            t.p[0].m[1]);
      }
    }
    vecTrianglesToRaster.clear();
  }

  ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
  SDL_RenderPresent(renderer);
}

void Renderer::Destroy() {
  // Cleanup
  ImGui_ImplSDLRenderer_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}

void ConvertNDCToViewportPixels(Triangle& triProjected, int ViewportWidth,
                                int ViewportHeight) {
  // Scale into view
  triProjected.p[0] += Vec4{1.0f, 1.0f, 0.0f, 0.0f};
  triProjected.p[1] += Vec4{1.0f, 1.0f, 0.0f, 0.0f};
  triProjected.p[2] += Vec4{1.0f, 1.0f, 0.0f, 0.0f};

  triProjected.p[0].x *= 0.5f * ViewportWidth;
  triProjected.p[0].y *= 0.5f * ViewportHeight;
  triProjected.p[1].x *= 0.5f * ViewportWidth;
  triProjected.p[1].y *= 0.5f * ViewportHeight;
  triProjected.p[2].x *= 0.5f * ViewportWidth;
  triProjected.p[2].y *= 0.5f * ViewportHeight;
}

Vec4 VectorIntersectPlane(Vec4& planeP, Vec4& planeN, Vec4& lineStart,
                          Vec4& lineEnd, float& t) {
  planeN = NormalizeChecked(Vec3(planeN));
  float planeD = -DotProduct(Vec3(planeN), Vec3(planeP));
  float ad = DotProduct(Vec3(lineStart), Vec3(planeN));
  float bd = DotProduct(Vec3(lineEnd), Vec3(planeN));
  t = (-planeD - ad) / (bd - ad);
  Vec4 lineStartToEnd = Vec3(lineEnd) - Vec3(lineStart);
  Vec4 lineToIntersect = Vec3(lineStartToEnd) * t;
  return Vec3(lineStart) + Vec3(lineToIntersect);
}

int TriangleClipAgainstPlane(Vec4 planeP, Vec4 planeN, Triangle& inTri,
                             Triangle& outTri1, Triangle& outTri2) {
  planeN = NormalizeChecked(Vec3(planeN));
  auto dist = [&](Vec4& p) {
    Vec4 n = NormalizeChecked(Vec3(p));
    return (DotProduct(Vec3(planeN), Vec3(p)) -
            DotProduct(Vec3(planeN), Vec3(planeP)));
  };

  // Create two temporary storage arrays to classify points either side of plane
  // If distance sign is positive, point lies on "inside" of plane
  Vec4* insidePoints[3];
  int nInsidePointCount = 0;
  Vec4* outsidePoints[3];
  int nOutsidePointCount = 0;
  Vec3* insideTex[3];
  int nInsideTexCount = 0;
  Vec3* outsideTex[3];
  int nOutsideTexCount = 0;

  // Get signed distance of each point in triangle to plane
  float d0 = dist(inTri.p[0]);
  float d1 = dist(inTri.p[1]);
  float d2 = dist(inTri.p[2]);

  if (d0 >= 0) {
    insidePoints[nInsidePointCount++] = &inTri.p[0];
    insideTex[nInsideTexCount++] = &inTri.tex[0];
  } else {
    outsidePoints[nOutsidePointCount++] = &inTri.p[0];
    outsideTex[nOutsideTexCount++] = &inTri.tex[0];
  }
  if (d1 >= 0) {
    insidePoints[nInsidePointCount++] = &inTri.p[1];
    insideTex[nInsideTexCount++] = &inTri.tex[1];
  } else {
    outsidePoints[nOutsidePointCount++] = &inTri.p[1];
    outsideTex[nOutsideTexCount++] = &inTri.tex[1];
  }
  if (d2 >= 0) {
    insidePoints[nInsidePointCount++] = &inTri.p[2];
    insideTex[nInsideTexCount++] = &inTri.tex[2];
  } else {
    outsidePoints[nOutsidePointCount++] = &inTri.p[2];
    outsideTex[nOutsideTexCount++] = &inTri.tex[2];
  }

  // Now classify triangle points, and break the input triangle into
  // smaller output triangles if required. There are four possible
  // outcomes...

  if (nInsidePointCount == 0) {
    // All points lie on the outside of plane, so clip whole triangle
    // It ceases to exist

    return 0;  // No returned triangles are valid
  }

  if (nInsidePointCount == 3) {
    // All points lie on the inside of plane, so do nothing
    // and allow the triangle to simply pass through
    outTri1 = inTri;

    return 1;  // Just the one returned original triangle is valid
  }

  if (nInsidePointCount == 1 && nOutsidePointCount == 2) {
    // Triangle should be clipped. As two points lie outside
    // the plane, the triangle simply becomes a smaller triangle

    // The inside point is valid, so keep that...
    outTri1.p[0] = *insidePoints[0];
    outTri1.tex[0] = *insideTex[0];

    // but the two new points are at the locations where the
    // original sides of the triangle (lines) intersect with the plane

    float t;
    outTri1.p[1] = VectorIntersectPlane(planeP, planeN, *insidePoints[0],
                                        *outsidePoints[0], t);
    outTri1.tex[1] = t * (*outsideTex[0] - *insideTex[0]) + *insideTex[0];

    outTri1.p[2] = VectorIntersectPlane(planeP, planeN, *insidePoints[0],
                                        *outsidePoints[1], t);
    outTri1.tex[2] = t * (*outsideTex[1] - *insideTex[0]) + *insideTex[0];

    outTri1.texUnit = inTri.texUnit;
    return 1;  // Return the newly formed single triangle
  }

  if (nInsidePointCount == 2 && nOutsidePointCount == 1) {
    // Triangle should be clipped. As two points lie inside the plane,
    // the clipped triangle becomes a "quad". Fortunately, we can
    // represent a quad with two new triangles

    // The first triangle consists of the two inside points and a new
    // point determined by the location where one side of the triangle
    // intersects with the plane
    outTri1.p[0] = *insidePoints[0];
    outTri1.tex[0] = *insideTex[0];
    outTri1.p[1] = *insidePoints[1];
    outTri1.tex[1] = *insideTex[1];

    float t;
    outTri1.p[2] = VectorIntersectPlane(planeP, planeN, *insidePoints[0],
                                        *outsidePoints[0], t);
    outTri1.tex[2] = t * (*outsideTex[0] - *insideTex[0]) + *insideTex[0];

    // The second triangle is composed of one of he inside points, a
    // new point determined by the intersection of the other side of the
    // triangle and the plane, and the newly created point above
    outTri2.p[0] = *insidePoints[1];
    outTri2.tex[0] = *insideTex[1];
    outTri2.p[1] = outTri1.p[2];
    outTri2.tex[1] = outTri1.tex[2];

    outTri2.p[2] = VectorIntersectPlane(planeP, planeN, *insidePoints[1],
                                        *outsidePoints[0], t);
    outTri2.tex[2] = t * (*outsideTex[0] - *insideTex[1]) + *insideTex[1];

    outTri1.texUnit = inTri.texUnit;
    outTri2.texUnit = inTri.texUnit;
    return 2;  // Return two newly formed triangles which form a quad
  }
}
