#ifndef _MESH
#define _MESH

#include <string>
#include <vector>

#include "Camera.h"
#include "Pipeline.h"
#include "Primitives.h"
#include "Renderer.h"
extern Pipeline pipeline;
extern Renderer renderer;

struct Mesh {
  std::vector<Triangle> tris;
  SDL_Texture* texture = nullptr;
  std::string texFile;

  bool LoadFromObjectFile(std::string sFileName, bool bHasTexture = false);
  void Draw(bool drawTexture = true);
};
void ConvertNDCToViewportPixels(Triangle& triProjected, int ViewportWidth,
                                int ViewportHeight);
Vec4 VectorIntersectPlane(Vec4& planeP, Vec4& planeN, Vec4& lineStart,
                          Vec4& lineEnd, float& t);
int TriangleClipAgainstPlane(Vec4 planeP, Vec4 planeN, Triangle& inTri,
                             Triangle& outTri1, Triangle& outTri2);

#endif
