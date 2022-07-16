#ifndef _RENDERER
#define _RENDERER

#include <imgui.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_sdlrenderer.h>

#include "Mesh.h"

class Renderer {
 public:
  SDL_Window* window;
  std::string winName;
  SDL_Renderer* renderer;
  ImGuiIO io;

  void Init(const char* windowName, int width, int height,
            SDL_WindowFlags window_flags =
                (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE |
                                  SDL_WINDOW_ALLOW_HIGHDPI),
            SDL_RendererFlags renderer_flags =
                (SDL_RendererFlags)(SDL_RENDERER_PRESENTVSYNC |
                                    SDL_RENDERER_ACCELERATED));

  void Clear(Vec4 color);
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
