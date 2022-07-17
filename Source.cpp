
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
  // SDL_SetRelativeMouseMode(SDL_TRUE);
  Mesh mesh;
  mesh.LoadFromObjectFile("res/models/cube/cube.obj", true);

  Mesh mesh2;
  mesh2.tris = {
      // SOUTH
      {{Vec4{0.0f, 0.0f, 0.0f, 1.0f}, Vec4{0.0f, 1.0f, 0.0f, 1.0f},
        Vec4{1.0f, 1.0f, 0.0f, 1.0f}}},
      {{Vec4{0.0f, 0.0f, 0.0f, 1.0f}, Vec4{1.0f, 1.0f, 0.0f, 1.0f},
        Vec4{1.0f, 0.0f, 0.0f, 1.0f}}},

      // EAST
      {{Vec4{1.0f, 0.0f, 0.0f, 1.0f}, Vec4{1.0f, 1.0f, 0.0f, 1.0f},
        Vec4{1.0f, 1.0f, 1.0f, 1.0f}}},
      {{Vec4{1.0f, 0.0f, 0.0f, 1.0f}, Vec4{1.0f, 1.0f, 1.0f, 1.0f},
        Vec4{1.0f, 0.0f, 1.0f, 1.0f}}},

      // NORTH
      {{Vec4{1.0f, 0.0f, 1.0f, 1.0f}, Vec4{1.0f, 1.0f, 1.0f, 1.0f},
        Vec4{0.0f, 1.0f, 1.0f, 1.0f}}},
      {{Vec4{1.0f, 0.0f, 1.0f, 1.0f}, Vec4{0.0f, 1.0f, 1.0f, 1.0f},
        Vec4{0.0f, 0.0f, 1.0f, 1.0f}}},

      // WEST
      {{Vec4{0.0f, 0.0f, 1.0f, 1.0f}, Vec4{0.0f, 1.0f, 1.0f, 1.0f},
        Vec4{0.0f, 1.0f, 0.0f, 1.0f}}},
      {{Vec4{0.0f, 0.0f, 1.0f, 1.0f}, Vec4{0.0f, 1.0f, 0.0f, 1.0f},
        Vec4{0.0f, 0.0f, 0.0f, 1.0f}}},

      // TOP
      {{Vec4{0.0f, 1.0f, 0.0f, 1.0f}, Vec4{0.0f, 1.0f, 1.0f, 1.0f},
        Vec4{1.0f, 1.0f, 1.0f, 1.0f}}},
      {{Vec4{0.0f, 1.0f, 0.0f, 1.0f}, Vec4{1.0f, 1.0f, 1.0f, 1.0f},
        Vec4{1.0f, 1.0f, 0.0f, 1.0f}}},

      // BOTTOM
      {{Vec4{1.0f, 0.0f, 1.0f, 1.0f}, Vec4{0.0f, 0.0f, 1.0f, 1.0f},
        Vec4{0.0f, 0.0f, 0.0f, 1.0f}}},
      {{Vec4{1.0f, 0.0f, 1.0f, 1.0f}, Vec4{0.0f, 0.0f, 0.0f, 1.0f},
        Vec4{1.0f, 0.0f, 0.0f, 1.0f}}},

  };

  Mesh mesh3;
  mesh3.tris = {
      // SOUTH
      {Vec4(0.0f, 0.0f, 0.0f, 1.0f), Vec4(0.0f, 1.0f, 0.0f, 1.0f),
       Vec4(1.0f, 1.0f, 0.0f, 1.0f), Vec3(0.0f, 1.0f, 1.0f),
       Vec3(0.0f, 0.0f, 1.0f), Vec3(1.0f, 0.0f, 1.0f)},
      {
          Vec4(0.0f, 0.0f, 0.0f, 1.0f),
          Vec4(1.0f, 1.0f, 0.0f, 1.0f),
          Vec4(1.0f, 0.0f, 0.0f, 1.0f),
          Vec3(0.0f, 1.0f, 1.0f),
          Vec3(1.0f, 0.0f, 1.0f),
          Vec3(1.0f, 1.0f, 1.0f),
      },

      // EAST
      {Vec4(1.0f, 0.0f, 0.0f, 1.0f), Vec4(1.0f, 1.0f, 0.0f, 1.0f),
       Vec4(1.0f, 1.0f, 1.0f, 1.0f), Vec3(0.0f, 1.0f, 1.0f),
       Vec3(0.0f, 0.0f, 1.0f), Vec3(1.0f, 0.0f, 1.0f)},
      {Vec4(1.0f, 0.0f, 0.0f, 1.0f), Vec4(1.0f, 1.0f, 1.0f, 1.0f),
       Vec4(1.0f, 0.0f, 1.0f, 1.0f), Vec3(0.0f, 1.0f, 1.0f),
       Vec3(1.0f, 0.0f, 1.0f), Vec3(1.0f, 1.0f, 1.0f)},

      // NORTH
      {
          Vec4(1.0f, 0.0f, 1.0f, 1.0f),
          Vec4(1.0f, 1.0f, 1.0f, 1.0f),
          Vec4(0.0f, 1.0f, 1.0f, 1.0f),
          Vec3(0.0f, 1.0f, 1.0f),
          Vec3(0.0f, 0.0f, 1.0f),
          Vec3(1.0f, 0.0f, 1.0f),
      },
      {Vec4(1.0f, 0.0f, 1.0f, 1.0f), Vec4(0.0f, 1.0f, 1.0f, 1.0f),
       Vec4(0.0f, 0.0f, 1.0f, 1.0f), Vec3(0.0f, 1.0f, 1.0f),
       Vec3(1.0f, 0.0f, 1.0f), Vec3(1.0f, 1.0f, 1.0f)},

      // WEST
      {Vec4(0.0f, 0.0f, 1.0f, 1.0f), Vec4(0.0f, 1.0f, 1.0f, 1.0f),
       Vec4(0.0f, 1.0f, 0.0f, 1.0f), Vec3(0.0f, 1.0f, 1.0f),
       Vec3(0.0f, 0.0f, 1.0f), Vec3(1.0f, 0.0f, 1.0f)},
      {Vec4(0.0f, 0.0f, 1.0f, 1.0f), Vec4(0.0f, 1.0f, 0.0f, 1.0f),
       Vec4(0.0f, 0.0f, 0.0f, 1.0f), Vec3(0.0f, 1.0f, 1.0f),
       Vec3(1.0f, 0.0f, 1.0f), Vec3(1.0f, 1.0f, 1.0f)},

      // TOP
      {Vec4(0.0f, 1.0f, 0.0f, 1.0f), Vec4(0.0f, 1.0f, 1.0f, 1.0f),
       Vec4(1.0f, 1.0f, 1.0f, 1.0f), Vec3(0.0f, 1.0f, 1.0f),
       Vec3(0.0f, 0.0f, 1.0f), Vec3(1.0f, 0.0f, 1.0f)},
      {Vec4(0.0f, 1.0f, 0.0f, 1.0f), Vec4(1.0f, 1.0f, 1.0f, 1.0f),
       Vec4(1.0f, 1.0f, 0.0f, 1.0f), Vec3(0.0f, 1.0f, 1.0f),
       Vec3(1.0f, 0.0f, 1.0f), Vec3(1.0f, 1.0f, 1.0f)},

      // BOTTOM
      {Vec4(1.0f, 0.0f, 1.0f, 1.0f), Vec4(0.0f, 0.0f, 1.0f, 1.0f),
       Vec4(0.0f, 0.0f, 0.0f, 1.0f), Vec3(0.0f, 1.0f, 1.0f),
       Vec3(0.0f, 0.0f, 1.0f), Vec3(1.0f, 0.0f, 1.0f)},
      {Vec4(1.0f, 0.0f, 1.0f, 1.0f), Vec4(0.0f, 0.0f, 0.0f, 1.0f),
       Vec4(1.0f, 0.0f, 0.0f, 1.0f), Vec3(0.0f, 1.0f, 1.0f),
       Vec3(1.0f, 0.0f, 1.0f), Vec3(1.0f, 1.0f, 1.0f)},

  };

  int sampler = pipeline.BindTexture(
      "test", IMG_LoadTexture(renderer.renderer, "res/images/test.jpg"));
  mesh3.SetTexUnit(sampler);

  float fTheta = 0.01f;
  while (renderer.isApplicationRunning) {
    renderer.PollEvents();

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

    renderer.NewFrame();
    renderer.Clear(Vec4(0.46f, 0.54f, 0.6f, 1.0f));
    mesh.Draw();
    Mat4 matScale2 = ScalingMat4(0.5f, 0.5f, 0.5f);
    Mat4 matRotZ2 = RotationZ(DegreesToRadians(-10.f));
    Mat4 matRotX2 = RotationX(DegreesToRadians(-10.f));
    Mat4 matTrans2 = TranslationMat4(0.f, -5.f, 9.f);
    Mat4 matWorld2 = matTrans2 * matRotX2 * matRotZ2 * matScale2;
    pipeline.SetPipelineMatrix(matWorld2, matView, matProj);
    mesh2.Draw(false);
    Mat4 matScale3 = ScalingMat4(0.5f, 0.5f, 0.5f);
    Mat4 matRotZ3 = RotationZ(DegreesToRadians(-10.f));
    Mat4 matRotX3 = RotationX(DegreesToRadians(-10.f));
    Mat4 matTrans3 = TranslationMat4(0.f, 4.f, 9.f);
    Mat4 matWorld3 = matTrans3 * matRotX * matRotZ;
    pipeline.SetPipelineMatrix(matWorld3, matView, matProj);
    mesh3.Draw();
    renderer.Present();
  }

  return 0;
}
