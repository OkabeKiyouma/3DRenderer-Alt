#ifndef _MESH
#define _MESH
#include <fstream>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

#include "Camera.h"

struct Triangle {
  Vec4 p[3];
  Vec3 tex[3];
};

struct FinalTriangle {
  Vec4 p[3];
  Vec3 tex[3];
  Vec4 col[3];
};

struct Mesh {
  std::vector<Triangle> tris;
  SDL_Texture* texture = nullptr;
  std::string texFile;

  bool LoadFromObjectFile(std::string sFileName, bool bHasTexture = false) {
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
        std::sregex_token_iterator it(sFileName.begin(), sFileName.end(), re,
                                      -1);
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
            }
          }
        }
        mtl.close();
      }

      if (line[0] == 'v') {
        if (line[1] == 't') {
          Vec2 v;
          s >> junk >> junk >> v.x >> v.y;
          // A little hack for the spyro texture
          // v.u = 1.0f - v.u;
          // v.v = 1.0f - v.v;
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

          tris.push_back({Vec4(verts[stoi(tokens[0]) - 1]),
                          Vec4(verts[stoi(tokens[3]) - 1]),
                          Vec4(verts[stoi(tokens[6]) - 1]),
                          Vec3(texs[stoi(tokens[1]) - 1]),
                          Vec3(texs[stoi(tokens[4]) - 1]),
                          Vec3(texs[stoi(tokens[7]) - 1])});
        }
      }
    }

    return true;
  }
};

#endif
