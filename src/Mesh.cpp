#include "Mesh.h"

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
  bool firstTex = true;

  while (!f.eof()) {
    char line[128];
    if (firstTex) f.getline(line, 128);

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
        if (std::regex_match(it->str(), std::regex(".*\\.(obj)"))) break;
        mtlFile += it->str() + "/";
      }
      s >> temp;
      std::ifstream mtl(mtlFile + temp, std::ifstream::in);
      if (!mtl.is_open()) {
        printf("Couldn't open %s", mtlFile.c_str());
        continue;
      };
      while (!mtl.eof()) {
        mtl.getline(line, 128);
        std::stringstream s;
        s << line;
        if (line[0] == 'n') {
          s >> temp;
          if (temp == "newmtl") {
            s >> temp;
            pipeline.texName.push_back(temp);
          }
        }
        if (line[0] == 'm') {
          s >> temp;
          if (temp == "map_Kd") {
            s >> temp;
            pipeline.textureUnit.push_back(
                IMG_LoadTexture(renderer.renderer, (mtlFile + temp).c_str()));
            pipeline.textureCount++;
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
        tris.push_back(
            {verts[f[0] - 1], verts[f[1] - 1], verts[f[2] - 1], 0, 0, 0, -1});
      }
    } else {
      if (line[0] == 'u') {
        firstTex = false;
        std::string temp;
        int sampler = -1;
        s >> temp >> temp;
        for (int i = 0; i < pipeline.texName.size(); i++)
          if (pipeline.texName[i] == temp) sampler = i;
        while (f.getline(line, 128) && line[0] != 'f')
          ;
        do {
          std::stringstream st;
          st << line;
          st >> junk;
          std::string tokens[9];
          int nTokenCount = -1;

          while (!st.eof()) {
            char c = st.get();
            if (c == ' ' || c == '/')
              nTokenCount++;
            else
              tokens[nTokenCount].append(1, c);
          }

          tokens[nTokenCount].pop_back();

          tris.push_back({Vec4(verts[stoi(tokens[0]) - 1]),
                          Vec4(verts[stoi(tokens[3]) - 1]),
                          Vec4(verts[stoi(tokens[6]) - 1]),
                          Vec3(texs[stoi(tokens[1]) - 1]),
                          Vec3(texs[stoi(tokens[4]) - 1]),
                          Vec3(texs[stoi(tokens[7]) - 1]), sampler});
          if (f.eof()) return true;
          f.getline(line, 128);
          if (strlen(line) == 0) return true;
        } while (line[0] == 'f');
      }
    }
  }

  return true;
}

void calculatePointLightsIllum(Triangle& t, Vec3 normal, Vec4 color[3]) {
  float ambientIllum = pipeline.light.Ka;

  Vec3 lightDirection = pipeline.light.Pos - Vec3(t.p[0]);
  Vec3 viewPos = pipeline.camera.Position - Vec3(t.p[0]);
  lightDirection = NormalizeChecked(lightDirection);
  float diffuseIllum =
      pipeline.light.Kd * Maximum(0.0f, DotProduct(lightDirection, normal));
  Vec3 reflectedDir =
      2 * DotProduct(lightDirection, normal) * normal - lightDirection;
  float specularIllum =
      pipeline.light.Ks *
      powf(DotProduct(reflectedDir, viewPos), pipeline.light.alpha);
  color[0] =
      Vec4((ambientIllum + diffuseIllum + specularIllum) * pipeline.light.Color,
           255);

  lightDirection = pipeline.light.Pos - Vec3(t.p[1]);
  viewPos = pipeline.camera.Position - Vec3(t.p[1]);
  lightDirection = NormalizeChecked(lightDirection);
  diffuseIllum =
      pipeline.light.Kd * Maximum(0.0f, DotProduct(lightDirection, normal));
  reflectedDir =
      2 * DotProduct(lightDirection, normal) * normal - lightDirection;
  specularIllum = pipeline.light.Ks *
                  powf(DotProduct(reflectedDir, viewPos), pipeline.light.alpha);
  color[1] =
      Vec4((ambientIllum + diffuseIllum + specularIllum) * pipeline.light.Color,
           255);

  lightDirection = pipeline.light.Pos - Vec3(t.p[2]);
  viewPos = pipeline.camera.Position - Vec3(t.p[2]);
  lightDirection = NormalizeChecked(lightDirection);
  diffuseIllum =
      pipeline.light.Kd * Maximum(0.0f, DotProduct(lightDirection, normal));
  reflectedDir =
      2 * DotProduct(lightDirection, normal) * normal - lightDirection;
  specularIllum = pipeline.light.Ks *
                  powf(DotProduct(reflectedDir, viewPos), pipeline.light.alpha);
  color[2] =
      Vec4((ambientIllum + diffuseIllum + specularIllum) * pipeline.light.Color,
           255);
}

void Mesh::Draw(bool drawTexture) {
  for (auto& tri : tris) {
    Triangle triTransformed, triViewed, triProjected;

    triTransformed = TriangleTransform(tri, pipeline.matWorld);

    // View Co-ordinate
    triViewed = TriangleTransform(triTransformed, pipeline.matView);

    Vec3 normal;
    Vec4 line1, line2;
    line1 = triViewed.p[1] - triViewed.p[0];
    line2 = triViewed.p[2] - triViewed.p[0];

    normal = CrossProduct(Vec3(line1), Vec3(line2));
    normal = NormalizeChecked(normal);
    float decision = DotProduct(
        normal, Vec3(triTransformed.p[0]) - pipeline.camera.Position);

    // Back Face Detection
    if (decision < 0.f) {
      // Color Generation

      Vec4 color[3];
      calculatePointLightsIllum(triTransformed, normal, color);
      // Vec3 lightDirection = pipeline.light.Pos - Vec3(triTransformed.p[0]);
      // lightDirection = NormalizeChecked(lightDirection);
      // float dotProduct = Maximum(0.0f, DotProduct(lightDirection, normal));
      // color[0] = Vec4(dotProduct * pipeline.light.Color, 255);
      //
      // lightDirection = pipeline.light.Pos - Vec3(triTransformed.p[1]);
      // lightDirection = NormalizeChecked(lightDirection);
      // dotProduct = Maximum(0.0f, DotProduct(lightDirection, normal));
      // color[1] = Vec4(dotProduct * pipeline.light.Color, 255);
      //
      // lightDirection = pipeline.light.Pos - Vec3(triTransformed.p[2]);
      // lightDirection = NormalizeChecked(lightDirection);
      // dotProduct = Maximum(0.0f, DotProduct(lightDirection, normal));
      // color[2] = Vec4(dotProduct * pipeline.light.Color, 255);

      int nClippedTriangles = 0;
      Triangle clipped[2];
      nClippedTriangles = TriangleClipAgainstPlane(
          Vec4(pipeline.camera.Position + Vec3{0.0f, 0.0f, 0.0f}),
          Vec4(NormalizeChecked(-pipeline.camera.Front)), triViewed, clipped[0],
          clipped[1]);
      for (int n = 0; n < nClippedTriangles; n++) {
        // Perspective Projection

        triProjected = TriangleTransform(clipped[n], pipeline.matProj);
        // Perspective Division
        triProjected.p[0] /= triProjected.p[0].w;
        triProjected.p[1] /= triProjected.p[1].w;
        triProjected.p[2] /= triProjected.p[2].w;

        triProjected.p[0].x = -triProjected.p[0].x;
        triProjected.p[1].x = -triProjected.p[1].x;
        triProjected.p[2].x = -triProjected.p[2].x;
        triProjected.p[0].y = -triProjected.p[0].y;
        triProjected.p[1].y = -triProjected.p[1].y;
        triProjected.p[2].y = -triProjected.p[2].y;

        // Z-correction
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

        renderer.vecTrianglesToRaster.push_back(FinalTriangle{
            triProjected.p[0], triProjected.p[1], triProjected.p[2],
            triProjected.tex[0], triProjected.tex[1], triProjected.tex[2],
            color[0], color[1], color[2], triProjected.texUnit});
      }
    }
  }
}

void Mesh::SetTexUnit(int index) {
  for (auto& t : tris) t.texUnit = index;
}
