#include "Primitives.h"

Triangle TriangleTransform(Triangle t, Mat4 m) {
  Triangle Transformed;
  Transformed.p[0] = m * t.p[0];
  Transformed.p[1] = m * t.p[1];
  Transformed.p[2] = m * t.p[2];
  Transformed.tex[0] = t.tex[0];
  Transformed.tex[1] = t.tex[1];
  Transformed.tex[2] = t.tex[2];
  Transformed.texUnit = t.texUnit;
  return Transformed;
}
