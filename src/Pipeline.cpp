#include "Pipeline.h"

#include <string>
#include <cstring>

void Pipeline::InitPipeline(int viewportWidth, int viewportHeight,
                            Camera camera, float fNear, float fFar,
                            float fFov) {
  //  pipeline.model.LoadFromObjectFile(modelPath, bHasTexture);
  //  pipeline.model.texture =
  //      IMG_LoadTexture(renderer.renderer, pipeline.model.texFile.c_str());
  this->camera = camera;
  this->fNear = fNear;
  this->fFar = fFar;
  this->fFov = fFov;
  this->viewportWidth = viewportWidth;
  this->viewportHeight = viewportHeight;
  this->fAspectRatio = (float)viewportWidth / (float)viewportHeight;
  this->pDepthBuffer = new float[this->viewportWidth * this->viewportHeight];
  memset(this->pDepthBuffer, 0,
         this->viewportWidth * this->viewportHeight * sizeof(float));
}

void Pipeline::SetPipelineMatrix(Mat4 matWorld, Mat4 matView, Mat4 matProj) {
  this->matWorld = matWorld;
  this->matView = matView;
  this->matProj = matProj;
}

int Pipeline::BindTexture(std::string texName, SDL_Texture* texture) {
  textureUnit.push_back(texture);
  this->texName.push_back(texName);
  textureCount++;
  return textureCount - 1;
}
