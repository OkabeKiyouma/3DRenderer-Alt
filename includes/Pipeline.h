#ifndef _PIPELINE
#define _PIPELINE
#include "Camera.h"

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

  void InitPipeline(int viewportWidth, int viewportHeight, Camera camera,
                    float fNear = 0.1f, float fFar = 1000.0f,
                    float fFov = 90.f);
  void SetPipelineMatrix(Mat4 matWorld, Mat4 matView, Mat4 matProj);
};

#endif
