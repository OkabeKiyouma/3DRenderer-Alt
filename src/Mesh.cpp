#include "Mesh.h"

#include < list>
#include <fstream>
#include <regex>
#include <sstream>
bool Mesh::LoadFromObjectFile(std::string sFileName, bool bHasTexture) {
  std::ifstream f(sFileName, std::ifstream::in);
  if (!f.is_open()) {
    printf("Failed to open %s\n", sFileName.c_str());
    return false;
  }

  // Local cache of verts
  std::vector<Vec3> verts;
  std::vector<Vec2> texs;

  while (!f.eof()) {
    char line[128];
    f.getline(line, 128);

    std::stringstream s;
    s << line;

    char junk;

    if (line[0] == 'm') {
      std::string temp;
      s >> temp;
      if (temp != "mtllib") continue;
      std::string mtlFile;
      std::regex re("[/]+");
      std::sregex_token_iterator it(sFileName.begin(), sFileName.end(), re, -1);
      std::sregex_token_iterator reg_end;
      for (; it != reg_end; ++it) {
        if (std::regex_match(it->str(), std::regex(".*\.(obj)"))) break;
        mtlFile += it->str() + "/";
      }
      s >> temp;
      mtlFile += temp;
      std::ifstream mtl(mtlFile, std::ifstream::in);
      if (!mtl.is_open()) {
        printf("Couldn't open %s", mtlFile.c_str());
        continue;
      };
      while (!mtl.eof()) {
        mtl.getline(line, 128);
        std::stringstream s;
        s << line;
        if (line[0] == 'm') {
          s >> temp;
          if (temp == "map_Kd") {
            s >> texFile;
            texture = IMG_LoadTexture(renderer.renderer, texFile.c_str());
          }
        }
      }
      mtl.close();
    }

    if (line[0] == 'v') {
      if (line[1] == 't') {
        Vec2 v;
        s >> junk >> junk >> v.x >> v.y;

        texs.push_back(v);
      } else {
        Vec3 v;
        s >> junk >> v.x >> v.y >> v.z;
        verts.push_back(v);
      }
    }

    if (!bHasTexture) {
      if (line[0] == 'f') {
        int f[3];
        s >> junk >> f[0] >> f[1] >> f[2];
        tris.push_back({verts[f[0] - 1], verts[f[1] - 1], verts[f[2] - 1]});
      }
    } else {
      if (line[0] == 'f') {
        s >> junk;

        std::string tokens[9];
        int nTokenCount = -1;

        while (!s.eof()) {
          char c = s.get();
          if (c == ' ' || c == '/')
            nTokenCount++;
          else
            tokens[nTokenCount].append(1, c);
        }

        tokens[nTokenCount].pop_back();

        tris.push_back(
            {Vec4(verts[stoi(tokens[0]) - 1]), Vec4(verts[stoi(tokens[3]) - 1]),
             Vec4(verts[stoi(tokens[6]) - 1]), Vec3(texs[stoi(tokens[1]) - 1]),
             Vec3(texs[stoi(tokens[4]) - 1]), Vec3(texs[stoi(tokens[7]) - 1])});
      }
    }
  }

  return true;
}

void Mesh::Draw() {
  std::vector<FinalTriangle> vecTrianglesToRaster;
  for (auto& tri : tris) {
    Triangle triTransformed, triViewed, triProjected;

    // World Co-ordinate

    triTransformed.p[0] = pipeline.matWorld * tri.p[0];
    triTransformed.p[1] = pipeline.matWorld * tri.p[1];
    triTransformed.p[2] = pipeline.matWorld * tri.p[2];
    triTransformed.tex[0] = tri.tex[0];
    triTransformed.tex[1] = tri.tex[1];
    triTransformed.tex[2] = tri.tex[2];

    // View Co-ordinate

    triViewed.p[0] = pipeline.matView * triTransformed.p[0];
    triViewed.p[1] = pipeline.matView * triTransformed.p[1];
    triViewed.p[2] = pipeline.matView * triTransformed.p[2];
    triViewed.tex[0] = triTransformed.tex[0];
    triViewed.tex[1] = triTransformed.tex[1];
    triViewed.tex[2] = triTransformed.tex[2];

    Vec3 normal;
    Vec4 line1, line2;
    line1 = triViewed.p[1] - triViewed.p[0];
    line2 = triViewed.p[2] - triViewed.p[0];

    normal = CrossProduct(Vec3(line1), Vec3(line2));
    normal = NormalizeChecked(normal);

    // float decision =  // DotProduct(normal, (Vec3(-camera.Front)));
    float decision = DotProduct(
        normal, Vec3(triTransformed.p[0]) - pipeline.camera.Position);
    //  DotProduct(normal, {0.f,0.f,-1.f});
    if (decision < 0.f) {
      // Color Generation
      //{
      Vec3 lightDirection = {0.0f, 1.0f, -1.0f};
      lightDirection = NormalizeChecked(lightDirection);

      float dotProduct = Maximum(0.0f, DotProduct(lightDirection, normal));

      Vec4 color[3];
      color[0] =
          Vec4(Vec3{255 * dotProduct, 255 * dotProduct, 255 * dotProduct}, 255);
      color[1] =
          Vec4(Vec3{255 * dotProduct, 255 * dotProduct, 255 * dotProduct}, 255);
      color[2] =
          Vec4(Vec3{255 * dotProduct, 255 * dotProduct, 255 * dotProduct}, 255);

      int nClippedTriangles = 0;
      Triangle clipped[2];
      nClippedTriangles = TriangleClipAgainstPlane(
          Vec4(pipeline.camera.Position + Vec3{0.0f, 0.0f, 0.0f}),
          Vec4(NormalizeChecked(-pipeline.camera.Front)), triViewed, clipped[0],
          clipped[1]);
      for (int n = 0; n < nClippedTriangles; n++) {
        // Perspective Projection

        triProjected.p[0] = pipeline.matProj * clipped[n].p[0];
        triProjected.p[1] = pipeline.matProj * clipped[n].p[1];
        triProjected.p[2] = pipeline.matProj * clipped[n].p[2];
        triProjected.tex[0] = clipped[n].tex[0];
        triProjected.tex[1] = clipped[n].tex[1];
        triProjected.tex[2] = clipped[n].tex[2];

        // Perspective Division
        triProjected.tex[0] /= triProjected.p[0].w;
        triProjected.tex[1] /= triProjected.p[1].w;
        triProjected.tex[2] /= triProjected.p[2].w;
        triProjected.p[0] /= triProjected.p[0].w;
        triProjected.p[1] /= triProjected.p[1].w;
        triProjected.p[2] /= triProjected.p[2].w;

        triProjected.p[0].x = -triProjected.p[0].x;
        triProjected.p[1].x = -triProjected.p[1].x;
        triProjected.p[2].x = -triProjected.p[2].x;
        triProjected.p[0].y = -triProjected.p[0].y;
        triProjected.p[1].y = -triProjected.p[1].y;
        triProjected.p[2].y = -triProjected.p[2].y;

        triProjected.p[0].z = (pipeline.fFar + pipeline.fNear) /
                                  (pipeline.fFar - pipeline.fNear) +
                              1 / clipped[n].p[0].z *
                                  (-2 * pipeline.fFar * pipeline.fNear /
                                   (pipeline.fFar - pipeline.fNear));
        triProjected.p[1].z = (pipeline.fFar + pipeline.fNear) /
                                  (pipeline.fFar - pipeline.fNear) +
                              1 / clipped[n].p[1].z *
                                  (-2 * pipeline.fFar * pipeline.fNear /
                                   (pipeline.fFar - pipeline.fNear));
        triProjected.p[2].z = (pipeline.fFar + pipeline.fNear) /
                                  (pipeline.fFar - pipeline.fNear) +
                              1 / clipped[n].p[2].z *
                                  (-2 * pipeline.fFar * pipeline.fNear /
                                   (pipeline.fFar - pipeline.fNear));

        ConvertNDCToViewportPixels(triProjected, pipeline.viewportWidth,
                                   pipeline.viewportHeight);

        vecTrianglesToRaster.push_back(FinalTriangle{
            triProjected.p[0], triProjected.p[1], triProjected.p[2],
            triProjected.tex[0], triProjected.tex[1], triProjected.tex[2],
            color[0], color[1], color[2]});
      }
    }
  }
  sort(vecTrianglesToRaster.begin(), vecTrianglesToRaster.end(),
       [](FinalTriangle& t1, FinalTriangle& t2) {
         float z1 = (t1.p[0].z + t1.p[1].z + t1.p[2].z) / 3.0f;
         float z2 = (t2.p[0].z + t2.p[1].z + t2.p[2].z) / 3.0f;
         return z1 > z2;
       });

  // Clear Depth Buffer
  for (int i = 0; i < pipeline.viewportWidth * pipeline.viewportHeight; ++i)
    pipeline.pDepthBuffer[i] = -10000000.f;

  for (auto& triProjected : vecTrianglesToRaster) {
    // Clip triangles against all four screen edges, this could yield
    // a bunch of triangles, so create a queue that we traverse to
    //  ensure we only test new triangles generated against planes
    Triangle clipped[2];
    std::list<Triangle> listTriangles;

    // Add initial triangle
    listTriangles.push_back({triProjected.p[0], triProjected.p[1],
                             triProjected.p[2], triProjected.tex[0],
                             triProjected.tex[1], triProjected.tex[2]});
    int nNewTriangles = 1;

    for (int p = 0; p < 4; p++) {
      int nTrisToAdd = 0;
      while (nNewTriangles > 0) {
        // Take triangle from front of queue
        Triangle test = listTriangles.front();
        listTriangles.pop_front();
        nNewTriangles--;

        // Clip it against a plane. We only need to test each
        // subsequent plane, against subsequent new triangles
        // as all triangles after a plane clip are guaranteed
        // to lie on the inside of the plane. I like how this
        // comment is almost completely and utterly justified
        switch (p) {
          case 0:
            nTrisToAdd = TriangleClipAgainstPlane(Vec4({0.0f, 0.0f, 0.0f}),
                                                  Vec4({0.0f, 1.0f, 0.0f}),
                                                  test, clipped[0], clipped[1]);
            break;
          case 1:
            nTrisToAdd = TriangleClipAgainstPlane(
                Vec4({0.0f, (float)pipeline.viewportHeight - 1, 0.0f}),
                Vec4({0.0f, -1.0f, 0.0f}), test, clipped[0], clipped[1]);
            break;
          case 2:
            nTrisToAdd = TriangleClipAgainstPlane(Vec4({0.0f, 0.0f, 0.0f}),
                                                  Vec4({1.0f, 0.0f, 0.0f}),
                                                  test, clipped[0], clipped[1]);
            break;
          case 3:
            nTrisToAdd = TriangleClipAgainstPlane(
                Vec4({(float)pipeline.viewportWidth - 1, 0.0f, 0.0f}),
                Vec4({-1.0f, 0.0f, 0.0f}), test, clipped[0], clipped[1]);
            break;
        }

        // Clipping may yield a variable number of triangles, so
        // add these new ones to the back of the queue for subsequent
        // clipping against next planes
        for (int w = 0; w < nTrisToAdd; w++)
          listTriangles.push_back(clipped[w]);
      }
      nNewTriangles = listTriangles.size();
    }
    for (auto& t : listTriangles) {
      SDL_Vertex vert[3];
      vert[0].position.x = t.p[0].x;
      vert[0].position.y = t.p[0].y;
      vert[0].tex_coord.x = t.tex[0].x / t.tex[0].z;
      vert[0].tex_coord.y = t.tex[0].y / t.tex[0].z;
      vert[0].color.r = triProjected.col[0].r;
      vert[0].color.g = triProjected.col[0].g;
      vert[0].color.b = triProjected.col[0].b;
      vert[0].color.a = triProjected.col[0].a;

      vert[1].position.x = t.p[1].x;
      vert[1].position.y = t.p[1].y;
      vert[1].tex_coord.x = t.tex[1].x / t.tex[1].z;
      vert[1].tex_coord.y = t.tex[1].y / t.tex[1].z;
      vert[1].color.r = triProjected.col[1].r;
      vert[1].color.g = triProjected.col[1].g;
      vert[1].color.b = triProjected.col[1].b;
      vert[1].color.a = triProjected.col[1].a;

      vert[2].position.x = t.p[2].x;
      vert[2].position.y = t.p[2].y;
      vert[2].tex_coord.x = t.tex[2].x / t.tex[2].z;
      vert[2].tex_coord.y = t.tex[2].y / t.tex[2].z;
      vert[2].color.r = triProjected.col[2].r;
      vert[2].color.g = triProjected.col[2].g;
      vert[2].color.b = triProjected.col[2].b;
      vert[2].color.a = triProjected.col[2].a;

      FinalTriangle tri;
      tri.p[0].x = t.p[0].x;
      tri.p[0].y = t.p[0].y;
      tri.p[0].z = t.p[0].z;
      tri.p[0].w = t.p[0].w;
      tri.tex[0].x = t.tex[0].x;
      tri.tex[0].y = t.tex[0].y;
      tri.tex[0].z = t.tex[0].z;
      tri.col[0].r = 255;  // triProjected.col[0].r;
      tri.col[0].g = 000;  // triProjected.col[0].g;
      tri.col[0].b = 000;  // triProjected.col[0].b;
      tri.col[0].a = 255;  // triProjected.col[0].a;

      tri.p[1].x = t.p[1].x;
      tri.p[1].y = t.p[1].y;
      tri.p[1].z = t.p[1].z;
      tri.p[1].w = t.p[1].w;
      tri.tex[1].x = t.tex[1].x;
      tri.tex[1].y = t.tex[1].y;
      tri.tex[1].z = t.tex[1].z;
      tri.col[1].r = 000;  // triProjected.col[1].r;
      tri.col[1].g = 255;  // triProjected.col[1].g;
      tri.col[1].b = 000;  // triProjected.col[1].b;
      tri.col[1].a = 255;  // triProjected.col[1].a;

      tri.p[2].x = t.p[2].x;
      tri.p[2].y = t.p[2].y;
      tri.p[2].z = t.p[2].z;
      tri.p[2].w = t.p[2].w;
      tri.tex[2].x = t.tex[2].x;
      tri.tex[2].y = t.tex[2].y;
      tri.tex[2].z = t.tex[2].z;
      tri.col[2].r = 000;  // triProjected.col[2].r;
      tri.col[2].g = 000;  // triProjected.col[2].g;
      tri.col[2].b = 255;  // triProjected.col[2].b;
      tri.col[2].a = 255;  // triProjected.col[2].a;
                           // TexturedTriangle(tri);
      // SDL_RenderGeometry(renderer.renderer, pipeline.model.texture, vert, 3,
      //                    nullptr, 0);
      SDL_RenderGeometry(renderer.renderer, texture, vert, 3, nullptr, 0);
      SDL_SetRenderDrawColor(renderer.renderer, 0, 0, 0, 0);
      SDL_RenderDrawLineF(renderer.renderer, t.p[0].m[0], t.p[0].m[1],
                          t.p[1].m[0], t.p[1].m[1]);
      SDL_RenderDrawLineF(renderer.renderer, t.p[1].m[0], t.p[1].m[1],
                          t.p[2].m[0], t.p[2].m[1]);
      SDL_RenderDrawLineF(renderer.renderer, t.p[2].m[0], t.p[2].m[1],
                          t.p[0].m[0], t.p[0].m[1]);
    }
  }
}

void ConvertNDCToViewportPixels(Triangle& triProjected, int ViewportWidth,
                                int ViewportHeight) {
  // Scale into view
  triProjected.p[0] += Vec4{1.0f, 1.0f, 0.0f, 0.0f};
  triProjected.p[1] += Vec4{1.0f, 1.0f, 0.0f, 0.0f};
  triProjected.p[2] += Vec4{1.0f, 1.0f, 0.0f, 0.0f};

  triProjected.p[0].x *= 0.5f * ViewportWidth;
  triProjected.p[0].y *= 0.5f * ViewportHeight;
  triProjected.p[1].x *= 0.5f * ViewportWidth;
  triProjected.p[1].y *= 0.5f * ViewportHeight;
  triProjected.p[2].x *= 0.5f * ViewportWidth;
  triProjected.p[2].y *= 0.5f * ViewportHeight;
}

Vec4 VectorIntersectPlane(Vec4& planeP, Vec4& planeN, Vec4& lineStart,
                          Vec4& lineEnd, float& t) {
  planeN = NormalizeChecked(Vec3(planeN));
  float planeD = -DotProduct(Vec3(planeN), Vec3(planeP));
  float ad = DotProduct(Vec3(lineStart), Vec3(planeN));
  float bd = DotProduct(Vec3(lineEnd), Vec3(planeN));
  t = (-planeD - ad) / (bd - ad);
  Vec4 lineStartToEnd = Vec3(lineEnd) - Vec3(lineStart);
  Vec4 lineToIntersect = Vec3(lineStartToEnd) * t;
  return Vec3(lineStart) + Vec3(lineToIntersect);
}

int TriangleClipAgainstPlane(Vec4 planeP, Vec4 planeN, Triangle& inTri,
                             Triangle& outTri1, Triangle& outTri2) {
  planeN = NormalizeChecked(Vec3(planeN));
  auto dist = [&](Vec4& p) {
    Vec4 n = NormalizeChecked(Vec3(p));
    return (DotProduct(Vec3(planeN), Vec3(p)) -
            DotProduct(Vec3(planeN), Vec3(planeP)));
  };

  // Create two temporary storage arrays to classify points either side of plane
  // If distance sign is positive, point lies on "inside" of plane
  Vec4* insidePoints[3];
  int nInsidePointCount = 0;
  Vec4* outsidePoints[3];
  int nOutsidePointCount = 0;
  Vec3* insideTex[3];
  int nInsideTexCount = 0;
  Vec3* outsideTex[3];
  int nOutsideTexCount = 0;

  // Get signed distance of each point in triangle to plane
  float d0 = dist(inTri.p[0]);
  float d1 = dist(inTri.p[1]);
  float d2 = dist(inTri.p[2]);

  if (d0 >= 0) {
    insidePoints[nInsidePointCount++] = &inTri.p[0];
    insideTex[nInsideTexCount++] = &inTri.tex[0];
  } else {
    outsidePoints[nOutsidePointCount++] = &inTri.p[0];
    outsideTex[nOutsideTexCount++] = &inTri.tex[0];
  }
  if (d1 >= 0) {
    insidePoints[nInsidePointCount++] = &inTri.p[1];
    insideTex[nInsideTexCount++] = &inTri.tex[1];
  } else {
    outsidePoints[nOutsidePointCount++] = &inTri.p[1];
    outsideTex[nOutsideTexCount++] = &inTri.tex[1];
  }
  if (d2 >= 0) {
    insidePoints[nInsidePointCount++] = &inTri.p[2];
    insideTex[nInsideTexCount++] = &inTri.tex[2];
  } else {
    outsidePoints[nOutsidePointCount++] = &inTri.p[2];
    outsideTex[nOutsideTexCount++] = &inTri.tex[2];
  }

  // Now classify triangle points, and break the input triangle into
  // smaller output triangles if required. There are four possible
  // outcomes...

  if (nInsidePointCount == 0) {
    // All points lie on the outside of plane, so clip whole triangle
    // It ceases to exist

    return 0;  // No returned triangles are valid
  }

  if (nInsidePointCount == 3) {
    // All points lie on the inside of plane, so do nothing
    // and allow the triangle to simply pass through
    outTri1 = inTri;

    return 1;  // Just the one returned original triangle is valid
  }

  if (nInsidePointCount == 1 && nOutsidePointCount == 2) {
    // Triangle should be clipped. As two points lie outside
    // the plane, the triangle simply becomes a smaller triangle

    // The inside point is valid, so keep that...
    outTri1.p[0] = *insidePoints[0];
    outTri1.tex[0] = *insideTex[0];

    // but the two new points are at the locations where the
    // original sides of the triangle (lines) intersect with the plane

    float t;
    outTri1.p[1] = VectorIntersectPlane(planeP, planeN, *insidePoints[0],
                                        *outsidePoints[0], t);
    outTri1.tex[1] = t * (*outsideTex[0] - *insideTex[0]) + *insideTex[0];

    outTri1.p[2] = VectorIntersectPlane(planeP, planeN, *insidePoints[0],
                                        *outsidePoints[1], t);
    outTri1.tex[2] = t * (*outsideTex[1] - *insideTex[0]) + *insideTex[0];

    return 1;  // Return the newly formed single triangle
  }

  if (nInsidePointCount == 2 && nOutsidePointCount == 1) {
    // Triangle should be clipped. As two points lie inside the plane,
    // the clipped triangle becomes a "quad". Fortunately, we can
    // represent a quad with two new triangles

    // The first triangle consists of the two inside points and a new
    // point determined by the location where one side of the triangle
    // intersects with the plane
    outTri1.p[0] = *insidePoints[0];
    outTri1.tex[0] = *insideTex[0];
    outTri1.p[1] = *insidePoints[1];
    outTri1.tex[1] = *insideTex[1];

    float t;
    outTri1.p[2] = VectorIntersectPlane(planeP, planeN, *insidePoints[0],
                                        *outsidePoints[0], t);
    outTri1.tex[2] = t * (*outsideTex[0] - *insideTex[0]) + *insideTex[0];

    // The second triangle is composed of one of he inside points, a
    // new point determined by the intersection of the other side of the
    // triangle and the plane, and the newly created point above
    outTri2.p[0] = *insidePoints[1];
    outTri2.tex[0] = *insideTex[1];
    outTri2.p[1] = outTri1.p[2];
    outTri2.tex[1] = outTri1.tex[2];

    outTri2.p[2] = VectorIntersectPlane(planeP, planeN, *insidePoints[1],
                                        *outsidePoints[0], t);
    outTri2.tex[2] = t * (*outsideTex[0] - *insideTex[1]) + *insideTex[1];

    return 2;  // Return two newly formed triangles which form a quad
  }
}
