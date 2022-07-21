#ifndef _PIPELINE
#define _PIPELINE
#include <SDL2/SDL.h>

#include <string>
#include <vector>

#include "Camera.h"

struct Light {
  Vec3 Pos = {0.f, -2.f, -1.f};
  Vec3 Color = {255.f, 255.f, 255.f};
  float Ka = 0.1f;
  float Kd = 0.9f;
  float Ks = 0.00f;
  int alpha = 1;
};

class Pipeline {
 public:
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
  bool wireFrame = false;
  Light light;
  // std::vector<Light> light;
  std::vector<std::string> texName;
  std::vector<SDL_Texture*> textureUnit;
  int textureCount = 0;

  void InitPipeline(int viewportWidth, int viewportHeight, Camera camera,
                    float fNear = 0.1f, float fFar = 1000.0f,
                    float fFov = 90.f);
  // void SetLights();
  void SetPipelineMatrix(Mat4 matWorld, Mat4 matView, Mat4 matProj);
  int BindTexture(std::string texName, SDL_Texture* texture);
};

#endif
