#ifndef _RENDERER
#define _RENDERER

#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include <imgui.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_sdlrenderer.h>

#include <string>
#include <vector>

#include "Pipeline.h"
#include "Primitives.h"

extern Pipeline pipeline;

class Renderer {
 public:
  SDL_Window* window;
  std::string winName;
  SDL_Renderer* renderer;
  SDL_Event event;
  bool isApplicationRunning = true;
  int ScreenWidth;
  int ScreenHeight;
  std::vector<FinalTriangle> vecTrianglesToRaster;

  ImGuiIO io;

  void Init(const char* windowName, int width, int height,
            SDL_WindowFlags window_flags =
                (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE |
                                  SDL_WINDOW_ALLOW_HIGHDPI),
            SDL_RendererFlags renderer_flags =
                (SDL_RendererFlags)(SDL_RENDERER_PRESENTVSYNC |
                                    SDL_RENDERER_ACCELERATED));

  void Clear(Vec4 color);
  void PollEvents();
  void HandleInputs(SDL_Event& event, Uint64 deltaTime);
  void NewFrame();
  void Present();
  void Destroy();
};

void ConvertNDCToViewportPixels(Triangle& triProjected, int ViewportWidth,
                                int ViewportHeight);
Vec4 VectorIntersectPlane(Vec4& planeP, Vec4& planeN, Vec4& lineStart,
                          Vec4& lineEnd, float& t);
int TriangleClipAgainstPlane(Vec4 planeP, Vec4 planeN, Triangle& inTri,
                             Triangle& outTri1, Triangle& outTri2);

#endif
