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

  bool LoadFromObjectFile(std::string sFileName, bool bHasTexture = false);
  void Draw(bool drawTexture = true);
};

#endif
