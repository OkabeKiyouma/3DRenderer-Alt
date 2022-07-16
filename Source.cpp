
#include <stdio.h>

#include "Mesh.h"
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
Renderer renderer;
Pipeline pipeline;

// Main code
int main(int, char**) {
  renderer.Init("Mid-Term Project", SCREEN_WIDTH, SCREEN_HEIGHT);
  {
    pipeline.InitPipeline(0.8f * SCREEN_WIDTH, SCREEN_HEIGHT, Camera());
    Mat4 matRotZ = RotationZ(DegreesToRadians(30.f));
    Mat4 matRotX = RotationX(DegreesToRadians(30.f));
    Mat4 matTrans = TranslationMat4(0.f, 0.f, 10.f);

    Mat4 matWorld = matTrans * matRotX * matRotZ;
    Mat4 matView = pipeline.camera.GetViewMatrix();
    Mat4 matProj = PerspectiveProjectionRH(DegreesToRadians(pipeline.fFov),
                                           pipeline.fAspectRatio,
                                           pipeline.fNear, pipeline.fFar);
    pipeline.SetPipelineMatrix(matWorld, matView, matProj);
  }

  Mesh mesh;
  mesh.LoadFromObjectFile("res/models/cube/cube.obj", true);

  float fTheta = 0.01f;
  while (renderer.isApplicationRunning) {
    renderer.PollEvents();
    {
      fTheta += 0.0005f;

      Mat4 matRotZ = RotationZ(DegreesToRadians(fTheta * 1000.f));
      Mat4 matRotX = RotationX(DegreesToRadians(fTheta * 1000.f));
      Mat4 matTrans = TranslationMat4(0.f, -00.f, 10.f);

      Mat4 matWorld = matTrans * matRotX * matRotZ;
      Mat4 matView = pipeline.camera.GetViewMatrix();
      Mat4 matProj = PerspectiveProjectionRH(DegreesToRadians(pipeline.fFov),
                                             pipeline.fAspectRatio,
                                             pipeline.fNear, pipeline.fFar);
      pipeline.SetPipelineMatrix(matWorld, matView, matProj);
    }
    renderer.NewFrame();
    renderer.Clear(Vec4(0.46f, 0.54f, 0.6f, 1.0f));
    mesh.Draw();
    renderer.Present();
  }

  return 0;
}
