
#include <stdio.h>

#include "App.h"

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

// Main code
int main(int, char**) {
  App app;
  app.Init(SCREEN_WIDTH, SCREEN_HEIGHT);
  {
    app.InitPipeline(0.8f * SCREEN_WIDTH, SCREEN_HEIGHT, Camera(),
                     "res/models/unt/untitled.obj", true);
    Mat4 matRotZ = RotationZ(DegreesToRadians(30.f));
    Mat4 matRotX = RotationX(DegreesToRadians(30.f));
    Mat4 matTrans = TranslationMat4(0.f, 0.f, 10.f);

    Mat4 matWorld = matTrans * matRotX * matRotZ;
    Mat4 matView = app.pipeline.camera.GetViewMatrix();
    Mat4 matProj = PerspectiveProjectionRH(
        DegreesToRadians(app.pipeline.fFov), app.pipeline.fAspectRatio,
        app.pipeline.fNear, app.pipeline.fFar);
    app.UpdatePipeline(matWorld, matView, matProj);
  }

  float fTheta = 0.01f;
  while (app.isApplicationRunning) {
    app.PollEvents();
    {
      fTheta += 0.0005f;
      Mat4 matRotZ = RotationZ(DegreesToRadians(fTheta * 1000.f));
      Mat4 matRotX = RotationX(DegreesToRadians(fTheta * 1000.f));
      Mat4 matTrans = TranslationMat4(0.f, -00.f, 10.f);

      Mat4 matWorld = matTrans * matRotX * matRotZ;
      Mat4 matView = app.pipeline.camera.GetViewMatrix();
      Mat4 matProj = PerspectiveProjectionRH(
          DegreesToRadians(app.pipeline.fFov), app.pipeline.fAspectRatio,
          app.pipeline.fNear, app.pipeline.fFar);
      app.UpdatePipeline(matWorld, matView, matProj);
    }
    app.NewFrame();
    app.Update();
  }

  return 0;
}
