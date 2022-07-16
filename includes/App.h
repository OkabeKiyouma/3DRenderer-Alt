#ifndef _APP
#define _APP

#include "Renderer.h"

struct Pipeline {
  Mesh model;
  Mat4 matWorld;
  Mat4 matView;
  Mat4 matProj;
  Camera camera;
  float fNear = 0.1f;
  float fFar = 1000.0f;
  float fFov = 90.f;
  float* pDepthBuffer = nullptr;
  float fAspectRatio;
  int viewportWidth;
  int viewportHeight;
};

class App {
 public:
  Renderer renderer;
  Pipeline pipeline;
  SDL_Event event;
  bool isApplicationRunning = true;
  int ScreenWidth;
  int ScreenHeight;
  Vec4 clearColor = Vec4(0.45f, 0.55f, 0.60f, 1.00f);

  // Fill Pipeline Object
  void Init(int screenWidth, int screenHeight);
  void InitPipeline(int viewportWidth, int viewportHeight, Camera camera,
                    std::string modelPath, bool bHasTexture, float fNear = 0.1f,
                    float fFar = 1000.0f, float fFov = 90.f);
  void UpdatePipeline(Mat4 matWorld, Mat4 matView, Mat4 matProj);

  void PollEvents();
  void NewFrame();
  void Update();
  void TexturedTriangle(FinalTriangle t);

 private:
  void HandleInputs(SDL_Event& event, Uint64 deltaTime);
};

Vec4 VectorIntersectPlane(Vec4& planeP, Vec4& planeN, Vec4& lineStart,
                          Vec4& lineEnd, float& t);

#endif
