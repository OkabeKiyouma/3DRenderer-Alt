#ifndef _PRIMITIVES
#define _PRIMITIVES

#include "Kmath.h"
struct Triangle {
  Vec4 p[3];
  Vec3 tex[3];
};

struct FinalTriangle {
  Vec4 p[3];
  Vec3 tex[3];
  Vec4 col[3];
};

Triangle TriangleTransform(Triangle t, Mat4 m);
#endif
