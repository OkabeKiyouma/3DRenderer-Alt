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
  ImGui::Begin("World Variables");
  ImGui::Text("This is some useful text.");
  ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
  ImGui::Checkbox("Wireframe", &pipeline.wireFrame);
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
