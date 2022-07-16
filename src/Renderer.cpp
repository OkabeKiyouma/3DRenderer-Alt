#include "Renderer.h"

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
}

void Renderer::Clear(Vec4 color) {
  SDL_SetRenderDrawColor(renderer, (Uint8)(color.x * 255),
                         (Uint8)(color.y * 255), (Uint8)(color.z * 255),
                         (Uint8)(color.w * 255));
  SDL_RenderClear(renderer);
}

void Renderer::Present() {
  // might need to put back in main loop
  SDL_SetWindowTitle(
      window,
      (winName + "- " + std::to_string((int)ImGui::GetIO().Framerate) + " FPS")
          .c_str());
  ImGui::Render();

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

    return 2;  // Return two newly formed triangles which form a quad
  }
}
