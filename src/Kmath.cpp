#include "Kmath.h"

Vec2::Vec2(const Vec3& vec) : x(vec.x), y(vec.y) {}
Vec3::Vec3(const Vec4& vec) : x(vec.x), y(vec.y), z(vec.z) {}

float Determinant(const Mat2& mat) {
  return mat.m2[0][0] * mat.m2[1][1] - mat.m2[0][1] * mat.m2[1][0];
}

Mat2 Inverse(const Mat2& mat) {
  float det = mat.m2[0][0] * mat.m2[1][1] - mat.m2[0][1] * mat.m2[1][0];
  det /= det;
  Mat2 res;
  res.m2[0][0] = mat.m2[1][1];
  res.m2[0][1] = -mat.m2[0][1];
  res.m2[1][0] = -mat.m2[1][0];
  res.m2[1][1] = mat.m2[0][0];
  res.m[0] *= det;
  res.m[1] *= det;
  res.m[2] *= det;
  res.m[3] *= det;
  return res;
}

Mat2 Transpose(const Mat2& m) {
  return Mat2{{m.m2[0][0], m.m2[1][0]}, {m.m2[0][1], m.m2[1][1]}};
}

float Determinant(const Mat3& mat) {
  float det = mat.m2[0][0] *
                  (mat.m2[1][1] * mat.m2[2][2] - mat.m2[2][1] * mat.m2[1][2]) +
              mat.m2[0][1] *
                  (mat.m2[1][2] * mat.m2[2][0] - mat.m2[1][0] * mat.m2[2][2]) +
              mat.m2[0][2] *
                  (mat.m2[1][0] * mat.m2[2][1] - mat.m2[2][0] * mat.m2[1][1]);
  return det;
}

Mat3 Inverse(const Mat3& mat) {
  Mat3 result;
  result.m2[0][0] = mat.m2[1][1] * mat.m2[2][2] - mat.m2[2][1] * mat.m2[1][2];
  result.m2[0][1] = mat.m2[0][2] * mat.m2[2][1] - mat.m2[0][1] * mat.m2[2][2];
  result.m2[0][2] = mat.m2[0][1] * mat.m2[1][2] - mat.m2[0][2] * mat.m2[1][1];
  result.m2[1][0] = mat.m2[1][2] * mat.m2[2][0] - mat.m2[1][0] * mat.m2[2][2];
  result.m2[1][1] = mat.m2[0][0] * mat.m2[2][2] - mat.m2[0][2] * mat.m2[2][0];
  result.m2[1][2] = mat.m2[1][0] * mat.m2[0][2] - mat.m2[0][0] * mat.m2[1][2];
  result.m2[2][0] = mat.m2[1][0] * mat.m2[2][1] - mat.m2[2][0] * mat.m2[1][1];
  result.m2[2][1] = mat.m2[2][0] * mat.m2[0][1] - mat.m2[0][0] * mat.m2[2][1];
  result.m2[2][2] = mat.m2[0][0] * mat.m2[1][1] - mat.m2[1][0] * mat.m2[0][1];

  float det = mat.m2[0][0] * result.m2[0][0] + mat.m2[0][1] * result.m2[1][0] +
              mat.m2[0][2] * result.m2[2][0];
  det /= det;
  for (int i = 0; i < 3; i++) result.rows[i] = result.rows[i] * det;
  return result;
}

Mat3 Transpose(const Mat3& m) {
  Mat3 res;
  res.rows[0] = Vec3(m.m2[0][0], m.m2[1][0], m.m2[2][0]);
  res.rows[1] = Vec3(m.m2[0][1], m.m2[1][1], m.m2[2][1]);
  res.rows[2] = Vec3(m.m2[0][2], m.m2[1][2], m.m2[2][2]);
  return res;
}

float Determinant(const Mat4& mat) {
  float m0 =
      mat.m[5] * mat.m[10] * mat.m[15] - mat.m[5] * mat.m[11] * mat.m[14] -
      mat.m[9] * mat.m[6] * mat.m[15] + mat.m[9] * mat.m[7] * mat.m[14] +
      mat.m[13] * mat.m[6] * mat.m[11] - mat.m[13] * mat.m[7] * mat.m[10];

  float m4 =
      -mat.m[4] * mat.m[10] * mat.m[15] + mat.m[4] * mat.m[11] * mat.m[14] +
      mat.m[8] * mat.m[6] * mat.m[15] - mat.m[8] * mat.m[7] * mat.m[14] -
      mat.m[12] * mat.m[6] * mat.m[11] + mat.m[12] * mat.m[7] * mat.m[10];

  float m8 = mat.m[4] * mat.m[9] * mat.m[15] -
             mat.m[4] * mat.m[11] * mat.m[13] -
             mat.m[8] * mat.m[5] * mat.m[15] + mat.m[8] * mat.m[7] * mat.m[13] +
             mat.m[12] * mat.m[5] * mat.m[11] - mat.m[12] * mat.m[7] * mat.m[9];

  float m12 =
      -mat.m[4] * mat.m[9] * mat.m[14] + mat.m[4] * mat.m[10] * mat.m[13] +
      mat.m[8] * mat.m[5] * mat.m[14] - mat.m[8] * mat.m[6] * mat.m[13] -
      mat.m[12] * mat.m[5] * mat.m[10] + mat.m[12] * mat.m[6] * mat.m[9];

  float det = mat.m[0] * m0 + mat.m[1] * m4 + mat.m[2] * m8 + mat.m[3] * m12;
  return det;
}

Mat4 Inverse(const Mat4& mat) {
  Mat4 result;

  result.m[0] =
      mat.m[5] * mat.m[10] * mat.m[15] - mat.m[5] * mat.m[11] * mat.m[14] -
      mat.m[9] * mat.m[6] * mat.m[15] + mat.m[9] * mat.m[7] * mat.m[14] +
      mat.m[13] * mat.m[6] * mat.m[11] - mat.m[13] * mat.m[7] * mat.m[10];

  result.m[4] =
      -mat.m[4] * mat.m[10] * mat.m[15] + mat.m[4] * mat.m[11] * mat.m[14] +
      mat.m[8] * mat.m[6] * mat.m[15] - mat.m[8] * mat.m[7] * mat.m[14] -
      mat.m[12] * mat.m[6] * mat.m[11] + mat.m[12] * mat.m[7] * mat.m[10];

  result.m[8] =
      mat.m[4] * mat.m[9] * mat.m[15] - mat.m[4] * mat.m[11] * mat.m[13] -
      mat.m[8] * mat.m[5] * mat.m[15] + mat.m[8] * mat.m[7] * mat.m[13] +
      mat.m[12] * mat.m[5] * mat.m[11] - mat.m[12] * mat.m[7] * mat.m[9];

  result.m[12] =
      -mat.m[4] * mat.m[9] * mat.m[14] + mat.m[4] * mat.m[10] * mat.m[13] +
      mat.m[8] * mat.m[5] * mat.m[14] - mat.m[8] * mat.m[6] * mat.m[13] -
      mat.m[12] * mat.m[5] * mat.m[10] + mat.m[12] * mat.m[6] * mat.m[9];

  result.m[1] =
      -mat.m[1] * mat.m[10] * mat.m[15] + mat.m[1] * mat.m[11] * mat.m[14] +
      mat.m[9] * mat.m[2] * mat.m[15] - mat.m[9] * mat.m[3] * mat.m[14] -
      mat.m[13] * mat.m[2] * mat.m[11] + mat.m[13] * mat.m[3] * mat.m[10];

  result.m[5] =
      mat.m[0] * mat.m[10] * mat.m[15] - mat.m[0] * mat.m[11] * mat.m[14] -
      mat.m[8] * mat.m[2] * mat.m[15] + mat.m[8] * mat.m[3] * mat.m[14] +
      mat.m[12] * mat.m[2] * mat.m[11] - mat.m[12] * mat.m[3] * mat.m[10];

  result.m[9] =
      -mat.m[0] * mat.m[9] * mat.m[15] + mat.m[0] * mat.m[11] * mat.m[13] +
      mat.m[8] * mat.m[1] * mat.m[15] - mat.m[8] * mat.m[3] * mat.m[13] -
      mat.m[12] * mat.m[1] * mat.m[11] + mat.m[12] * mat.m[3] * mat.m[9];

  result.m[13] =
      mat.m[0] * mat.m[9] * mat.m[14] - mat.m[0] * mat.m[10] * mat.m[13] -
      mat.m[8] * mat.m[1] * mat.m[14] + mat.m[8] * mat.m[2] * mat.m[13] +
      mat.m[12] * mat.m[1] * mat.m[10] - mat.m[12] * mat.m[2] * mat.m[9];

  result.m[2] =
      mat.m[1] * mat.m[6] * mat.m[15] - mat.m[1] * mat.m[7] * mat.m[14] -
      mat.m[5] * mat.m[2] * mat.m[15] + mat.m[5] * mat.m[3] * mat.m[14] +
      mat.m[13] * mat.m[2] * mat.m[7] - mat.m[13] * mat.m[3] * mat.m[6];

  result.m[6] =
      -mat.m[0] * mat.m[6] * mat.m[15] + mat.m[0] * mat.m[7] * mat.m[14] +
      mat.m[4] * mat.m[2] * mat.m[15] - mat.m[4] * mat.m[3] * mat.m[14] -
      mat.m[12] * mat.m[2] * mat.m[7] + mat.m[12] * mat.m[3] * mat.m[6];

  result.m[10] =
      mat.m[0] * mat.m[5] * mat.m[15] - mat.m[0] * mat.m[7] * mat.m[13] -
      mat.m[4] * mat.m[1] * mat.m[15] + mat.m[4] * mat.m[3] * mat.m[13] +
      mat.m[12] * mat.m[1] * mat.m[7] - mat.m[12] * mat.m[3] * mat.m[5];

  result.m[14] =
      -mat.m[0] * mat.m[5] * mat.m[14] + mat.m[0] * mat.m[6] * mat.m[13] +
      mat.m[4] * mat.m[1] * mat.m[14] - mat.m[4] * mat.m[2] * mat.m[13] -
      mat.m[12] * mat.m[1] * mat.m[6] + mat.m[12] * mat.m[2] * mat.m[5];

  result.m[3] =
      -mat.m[1] * mat.m[6] * mat.m[11] + mat.m[1] * mat.m[7] * mat.m[10] +
      mat.m[5] * mat.m[2] * mat.m[11] - mat.m[5] * mat.m[3] * mat.m[10] -
      mat.m[9] * mat.m[2] * mat.m[7] + mat.m[9] * mat.m[3] * mat.m[6];

  result.m[7] =
      mat.m[0] * mat.m[6] * mat.m[11] - mat.m[0] * mat.m[7] * mat.m[10] -
      mat.m[4] * mat.m[2] * mat.m[11] + mat.m[4] * mat.m[3] * mat.m[10] +
      mat.m[8] * mat.m[2] * mat.m[7] - mat.m[8] * mat.m[3] * mat.m[6];

  result.m[11] =
      -mat.m[0] * mat.m[5] * mat.m[11] + mat.m[0] * mat.m[7] * mat.m[9] +
      mat.m[4] * mat.m[1] * mat.m[11] - mat.m[4] * mat.m[3] * mat.m[9] -
      mat.m[8] * mat.m[1] * mat.m[7] + mat.m[8] * mat.m[3] * mat.m[5];

  result.m[15] =
      mat.m[0] * mat.m[5] * mat.m[10] - mat.m[0] * mat.m[6] * mat.m[9] -
      mat.m[4] * mat.m[1] * mat.m[10] + mat.m[4] * mat.m[2] * mat.m[9] +
      mat.m[8] * mat.m[1] * mat.m[6] - mat.m[8] * mat.m[2] * mat.m[5];

  float det = mat.m[0] * result.m[0] + mat.m[1] * result.m[4] +
              mat.m[2] * result.m[8] + mat.m[3] * result.m[12];
  det = 1.0f / det;
  for (int i = 0; i < 4; i++) result.rows[i] = result.rows[i] * det;
  return result;
}

Mat4 Transpose(const Mat4& m) {
  Mat4 res;
  res.rows[0] = Vec4(m.m2[0][0], m.m2[1][0], m.m2[2][0], m.m2[3][0]);
  res.rows[1] = Vec4(m.m2[0][1], m.m2[1][1], m.m2[2][1], m.m2[3][1]);
  res.rows[2] = Vec4(m.m2[0][2], m.m2[1][2], m.m2[2][2], m.m2[3][2]);
  res.rows[3] = Vec4(m.m2[0][3], m.m2[1][3], m.m2[2][3], m.m2[3][3]);
  return res;
}

//
//
//

Mat2 operator*(const Mat2& left, const Mat2& right) {
  Mat2 res;
  Mat2 tras = Transpose(right);

  res.m2[0][0] = DotProduct(left.rows[0], tras.rows[0]);
  res.m2[0][1] = DotProduct(left.rows[0], tras.rows[1]);
  res.m2[1][0] = DotProduct(left.rows[1], tras.rows[0]);
  res.m2[1][1] = DotProduct(left.rows[1], tras.rows[1]);

  return res;
}

Vec2 operator*(const Mat2& mat, Vec2 vec) {
  Vec2 res;
  res.m[0] = DotProduct(vec, mat.rows[0]);
  res.m[1] = DotProduct(vec, mat.rows[1]);
  return res;
}

Vec2 operator*(Vec2 vec, const Mat2& mat) {
  Vec2 res;
  res.m[0] = DotProduct(vec, Vec2(mat.m2[0][0], mat.m2[1][0]));
  res.m[1] = DotProduct(vec, Vec2(mat.m2[0][1], mat.m2[1][1]));
  return res;
}

Mat3 operator*(const Mat3& left, const Mat3& right) {
  Mat3 res;
  Mat3 tras = Transpose(right);

  res.m2[0][0] = DotProduct(left.rows[0], tras.rows[0]);
  res.m2[0][1] = DotProduct(left.rows[0], tras.rows[1]);
  res.m2[0][2] = DotProduct(left.rows[0], tras.rows[2]);

  res.m2[1][0] = DotProduct(left.rows[1], tras.rows[0]);
  res.m2[1][1] = DotProduct(left.rows[1], tras.rows[1]);
  res.m2[1][2] = DotProduct(left.rows[1], tras.rows[2]);

  res.m2[2][0] = DotProduct(left.rows[2], tras.rows[0]);
  res.m2[2][1] = DotProduct(left.rows[2], tras.rows[1]);
  res.m2[2][2] = DotProduct(left.rows[2], tras.rows[2]);

  return res;
}

Vec3 operator*(const Mat3& mat, Vec3 vec) {
  Vec3 res;
  res.m[0] = DotProduct(vec, mat.rows[0]);
  res.m[1] = DotProduct(vec, mat.rows[1]);
  res.m[2] = DotProduct(vec, mat.rows[2]);
  return res;
}

Mat4 operator*(const Mat4& left, const Mat4& right) {
  Mat4 res;
  Mat4 tras = Transpose(right);

  res.m2[0][0] = DotProduct(left.rows[0], tras.rows[0]);
  res.m2[0][1] = DotProduct(left.rows[0], tras.rows[1]);
  res.m2[0][2] = DotProduct(left.rows[0], tras.rows[2]);
  res.m2[0][3] = DotProduct(left.rows[0], tras.rows[3]);

  res.m2[1][0] = DotProduct(left.rows[1], tras.rows[0]);
  res.m2[1][1] = DotProduct(left.rows[1], tras.rows[1]);
  res.m2[1][2] = DotProduct(left.rows[1], tras.rows[2]);
  res.m2[1][3] = DotProduct(left.rows[1], tras.rows[3]);

  res.m2[2][0] = DotProduct(left.rows[2], tras.rows[0]);
  res.m2[2][1] = DotProduct(left.rows[2], tras.rows[1]);
  res.m2[2][2] = DotProduct(left.rows[2], tras.rows[2]);
  res.m2[2][3] = DotProduct(left.rows[2], tras.rows[3]);

  res.m2[3][0] = DotProduct(left.rows[3], tras.rows[0]);
  res.m2[3][1] = DotProduct(left.rows[3], tras.rows[1]);
  res.m2[3][2] = DotProduct(left.rows[3], tras.rows[2]);
  res.m2[3][3] = DotProduct(left.rows[3], tras.rows[3]);

  return res;
}

Vec4 operator*(const Mat4& mat, Vec4 vec) {
  Vec4 res;
  res.m[0] = DotProduct(vec, mat.rows[0]);
  res.m[1] = DotProduct(vec, mat.rows[1]);
  res.m[2] = DotProduct(vec, mat.rows[2]);
  res.m[3] = DotProduct(vec, mat.rows[3]);
  return res;
}

// SCALING MATRIX
Mat2 ScalingMat2(float x, float y) {
  Mat2 m;
  m.rows[0] = Vec2(x, 0.0f);
  m.rows[1] = Vec2(0.0f, y);
  return m;
}

Mat2 ScalingMat2(Vec2 s) {
  Mat2 m;
  m.rows[0] = Vec2(s.x, 0.0f);
  m.rows[1] = Vec2(0.0f, s.y);
  return m;
}

Mat3 ScalingMat3(float S_1, float S_2) {
  Mat3 m;
  m.rows[0] = Vec3(S_1, 0.0f, 0.0f);
  m.rows[1] = Vec3(0.0f, S_2, 0.0f);
  m.rows[2] = Vec3(0.0f, 0.0f, 1.0f);
  return m;
}
Mat3 ScalingMat3(Vec2 s) { return ScalingMat3(s.x, s.y); }

Mat4 ScalingMat4(float S_1, float S_2, float S_3) {
  Mat4 m;
  m.rows[0] = Vec4(S_1, 0.0f, 0.0f, 0.0f);
  m.rows[1] = Vec4(0.0f, S_2, 0.0f, 0.0f);
  m.rows[2] = Vec4(0.0f, 0.0f, S_3, 0.0f);
  m.rows[3] = Vec4(0.0f, 0.0f, 0.0f, 1.0f);
  return m;
}
Mat4 ScalingMat4(Vec3 s) { return ScalingMat4(s.x, s.y, s.z); }

// ROTATION
Mat2 RotationMat2(float angle) {
  float c = cosf(angle);
  float s = sinf(angle);

  Mat2 mat;
  mat.rows[0] = Vec2(c, -s);
  mat.rows[1] = Vec2(s, c);
  return mat;
}

Mat3 RotationMat3(float angle) {
  Mat3 m;
  float c = cosf(angle);
  float s = sinf(angle);
  m.rows[0] = Vec3(c, -s, 0.0f);
  m.rows[1] = Vec3(s, c, 0.0f);
  m.rows[2] = Vec3(0.0f, 0.0f, 1.0f);
  return m;
}

Mat4 RotationX(float angle) {
  Mat4 m;
  m.rows[0] = Vec4(1.0f, 0.0f, 0.0f, 0.0f);
  m.rows[1] = Vec4(0.0f, cosf(angle), -sinf(angle), 0.0f);
  m.rows[2] = Vec4(0.0f, sinf(angle), cosf(angle), 0.0f);
  m.rows[3] = Vec4(0.0f, 0.0f, 0.0f, 1.0f);
  return m;
}

Mat4 RotationY(float angle) {
  Mat4 m;
  m.rows[0] = Vec4(cosf(angle), 0.0f, sinf(angle), 0.0f);
  m.rows[1] = Vec4(0.0f, 1.0f, 0.0f, 0.0f);
  m.rows[2] = Vec4(-sinf(angle), 0.0f, cosf(angle), 0.0f);
  m.rows[3] = Vec4(0.0f, 0.0f, 0.0f, 1.0f);
  return m;
}

Mat4 RotationZ(float angle) {
  Mat4 m;
  m.rows[0] = Vec4(cosf(angle), -sinf(angle), 0.0f, 0.0f);
  m.rows[1] = Vec4(sinf(angle), cosf(angle), 0.0f, 0.0f);
  m.rows[2] = Vec4(0.0f, 0.0f, 1.0f, 0.0f);
  m.rows[3] = Vec4(0.0f, 0.0f, 0.0f, 1.0f);
  return m;
}

Mat4 RotationMat4(float x, float y, float z, float angle) {
  float c = cosf(angle);
  float s = sinf(angle);

  float x2 = x * x;
  float xy = x * y;
  float zx = x * z;
  float y2 = y * y;
  float yz = y * z;
  float z2 = z * z;

  Mat4 m;
  m.rows[0] =
      Vec4(c + x2 * (1 - c), xy * (1 - c) - z * s, zx * (1 - c) + y * s, 0.0f);
  m.rows[1] =
      Vec4(xy * (1 - c) + z * s, c + y2 * (1 - c), yz * (1 - c) - x * s, 0.0f);
  m.rows[2] =
      Vec4(zx * (1 - c) - y * s, yz * (1 - c) + x * s, c + z2 * (1 - c), 0.0f);
  m.rows[3] = Vec4(0.0f, 0.0f, 0.0f, 1.0f);
  return m;
}

Mat4 RotationMat4(Vec3 axis, float angle) {
  return RotationMat4(axis.x, axis.y, axis.z, angle);
}

// TRANSLATION
Mat3 TranslationMat3(float T_x, float T_y) {
  Mat3 m;
  m.rows[0] = Vec3(1.0f, 0.0f, T_x);
  m.rows[1] = Vec3(0.0f, 1.0f, T_y);
  m.rows[2] = Vec3(0.0f, 0.0f, 1.0f);
  return m;
}

Mat3 TranslationMat3(Vec2 t) { return TranslationMat3(t.x, t.y); }

Mat4 TranslationMat4(float T_x, float T_y, float T_z) {
  Mat4 m;
  m.rows[0] = Vec4(1.0f, 0.0f, 0.0f, T_x);
  m.rows[1] = Vec4(0.0f, 1.0f, 0.0f, T_y);
  m.rows[2] = Vec4(0.0f, 0.0f, 1.0f, T_z);
  m.rows[3] = Vec4(0.0f, 0.0f, 0.0f, 1.0f);
  return m;
}

Mat4 TranslationMat4(Vec3 t) { return TranslationMat4(t.x, t.y, t.z); }

Mat3 LookAt(Vec2 from, Vec2 to, Vec2 forward) {
  Vec2 dir = Normalize(to - from);
  float cos_theta = DotProduct(dir, forward);
  float sin_theta = sqrtf(1.0f - cos_theta * cos_theta);

  Mat3 m;
  m.rows[0] = Vec3(cos_theta, -sin_theta, from.x);
  m.rows[1] = Vec3(sin_theta, cos_theta, from.y);
  m.rows[2] = Vec3(0.0f, 0.0f, 1.0f);
  return m;
}

Mat4 LookAt(Vec3 from, Vec3 to, Vec3 world_up) {
  Vec3 direction = Normalize(from - to);
  Vec3 right = Normalize(CrossProduct(world_up, direction));
  Vec3 up = CrossProduct(direction, right);

  Mat4 lookat;
  lookat.rows[0] = Vec4(right.x, right.y, right.z, -from.x);
  lookat.rows[1] = Vec4(up.x, up.y, up.z, -from.y);
  lookat.rows[2] = Vec4(direction.x, direction.y, direction.z, -from.z);
  lookat.rows[3] = Vec4(0, 0, 0, 1);

  return lookat;
}

Mat4 LookAtChecked(Vec3 from, Vec3 to, Vec3 world_up) {
  Vec3 direction = NormalizeChecked(from - to);
  Vec3 right = NormalizeChecked(CrossProduct(world_up, direction));
  Vec3 up = NormalizeChecked(CrossProduct(direction, right));

  Mat4 lookat;
  lookat.rows[0] = Vec4(right.x, right.y, right.z, -from.x);
  lookat.rows[1] = Vec4(up.x, up.y, up.z, -from.y);
  lookat.rows[2] = Vec4(direction.x, direction.y, direction.z, -from.z);
  lookat.rows[3] = Vec4(0, 0, 0, 1);

  return lookat;
}

Mat4 LookAtDirection(Vec3 dir, Vec3 world_up) {
  // assert(!IsNull(dir));

  Vec3 forward = dir;
  Vec3 right = Normalize(CrossProduct(world_up, forward));
  Vec3 up = CrossProduct(right, forward);

  Mat4 lookat;
  lookat.rows[0] = Vec4(right.x, up.x, forward.x, 0);
  lookat.rows[1] = Vec4(right.y, up.y, forward.y, 0);
  lookat.rows[2] = Vec4(right.z, up.z, forward.z, 0);
  lookat.rows[3] = Vec4(0, 0, 0, 1);

  return lookat;
}

Mat4 OrthographicProjectionRH(float l, float r, float t, float b, float n,
                              float f) {
  float iwidth = 1 / (r - l);
  float iheight = 1 / (t - b);
  float range = 1 / (n - f);

  Mat4 m;
  m.rows[0] = Vec4(2 * iwidth, 0.0f, 0.0f, -(l + r) * iwidth);
  m.rows[1] = Vec4(0.0f, 2 * iheight, 0.0f, -(t + b) * iheight);
  m.rows[2] = Vec4(0.0f, 0.0f, range, -n * range);
  m.rows[3] = Vec4(0.0f, 0.0f, 0.0f, 1.0f);
  return m;
}

Mat4 OrthographicProjectionLH(float l, float r, float t, float b, float n,
                              float f) {
  float iwidth = 1 / (r - l);
  float iheight = 1 / (t - b);
  float range = 1 / (f - n);

  Mat4 m;
  m.rows[0] = Vec4(2 * iwidth, 0.0f, 0.0f, -(l + r) * iwidth);
  m.rows[1] = Vec4(0.0f, 2 * iheight, 0.0f, -(t + b) * iheight);
  m.rows[2] = Vec4(0.0f, 0.0f, range, -n * range);
  m.rows[3] = Vec4(0.0f, 0.0f, 0.0f, 1.0f);
  return m;
}

Mat4 PerspectiveProjectionRH(float fov, float aspect_ratio, float n, float f) {
  float height = 1.0f / tanf(fov * 0.5f);
  float width = height / aspect_ratio;
  float range = 1 / (n - f);

  Mat4 m;
  m.rows[0] = Vec4(width, 0.0f, 0.0f, 0.0f);
  m.rows[1] = Vec4(0.0f, height, 0.0f, 0.0f);
  m.rows[2] = Vec4(0.0f, 0.0f, f * range, -1.0f * f * n * range);
  m.rows[3] = Vec4(0.0f, 0.0f, -1.0f, 0.0f);
  return m;
}

Mat4 PerspectiveProjectionLH(float fov, float aspect_ratio, float n, float f) {
  float height = 1.0f / tanf(fov * 0.5f);
  float width = height / aspect_ratio;
  float range = 1 / (f - n);

  Mat4 m;
  m.rows[0] = Vec4(width, 0.0f, 0.0f, 0.0f);
  m.rows[1] = Vec4(0.0f, height, 0.0f, 0.0f);
  m.rows[2] = Vec4(0.0f, 0.0f, f * range, -1.0f * f * n * range);
  m.rows[3] = Vec4(0.0f, 0.0f, 1.0f, 0.0f);
  return m;
}

Vec3 GetRight(const Mat4& m) {
  Vec3 v;
  v.x = m.m2[0][0];
  v.y = m.m2[1][0];
  v.z = m.m2[2][0];
  return v;
}

Vec3 GetUp(const Mat4& m) {
  Vec3 v;
  v.x = m.m2[0][2];
  v.y = m.m2[1][2];
  v.z = m.m2[2][2];
  return v;
}

Vec3 GetForward(const Mat4& m) {
  Vec3 v;
  v.x = m.m2[0][1];
  v.y = m.m2[1][1];
  v.z = m.m2[2][1];
  return v;
}

Mat2 ToMat2(const Mat3& mat) {
  Mat2 result;
  result.rows[0] = {mat.m2[0][0], mat.m2[0][1]};
  result.rows[1] = {mat.m2[1][0], mat.m2[1][1]};
  return result;
}

Mat3 ToMat3(const Mat2& mat) {
  Mat3 result;
  result.rows[0] = Vec3(mat.rows[0], 0);
  result.rows[1] = Vec3(mat.rows[1], 0);
  result.rows[2] = Vec3(0, 0, 1);
  return result;
}

Mat3 ToMat3(const Mat4& mat) {
  Mat3 result;
  result.rows[0] = {mat.m2[0][0], mat.m2[0][1], mat.m2[0][2]};
  result.rows[1] = {mat.m2[1][0], mat.m2[1][1], mat.m2[1][2]};
  result.rows[2] = {mat.m2[2][0], mat.m2[2][1], mat.m2[2][2]};
  return result;
}

Mat4 ToMat4(const Mat3& mat) {
  Mat4 result;
  result.rows[0] = Vec4(mat.rows[0], 0);
  result.rows[1] = Vec4(mat.rows[1], 0);
  result.rows[2] = Vec4(mat.rows[2], 0);
  result.rows[3] = Vec4(0, 0, 0, 1);
  return result;
}
