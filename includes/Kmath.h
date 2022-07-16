#ifndef _KMATH
#define _KMATH

#include <float.h>

#include <cmath>
#ifdef _DEBUG
#include <cassert>
#endif

static inline float __fabs(float a) { return (a >= 0) ? a : -a; }

constexpr float PI = 3.1415926535f;
constexpr float PI_INVERSE = 1.0f / PI;
constexpr float TAU = PI / 2;
constexpr float REAL_EPSILON = FLT_EPSILON;
constexpr float REAL_MAX = FLT_MAX;
constexpr float REAL_MIN = FLT_MIN;
#define DegreesToRadians(deg) ((deg) * (PI / 180))
#define RadiansToDegrees(rad) ((rad) * (180 / PI))
#define ArrayCount(a) (sizeof(a) / sizeof((a)[0]))
#define Minimum(a, b) (((a) < (b)) ? (a) : (b))
#define Maximum(a, b) (((a) > (b)) ? (a) : (b))
#define Clamp(a, b, v) Minimum(b, Maximum(a, v))

union Vec2;
union Vec3;
union Vec4;
union Mat2;
union Mat3;
union Mat4;

union Vec2 {
  struct {
    float x, y;
  };
  struct {
    float r, g;
  };
  float m[2];

  Vec2() {}
  Vec2(float a) : x(a), y(a) {}
  Vec2(float a, float b) : x(a), y(b) {}
  Vec2(const Vec2& vec) : x(vec.x), y(vec.y) {}
  Vec2(const Vec3& vec);
};

union Vec3 {
  struct {
    float x, y, z;
  };
  struct {
    float r, g, b;
  };
  float m[3];

  Vec3() {}
  Vec3(float a) : x(a), y(a), z(a) {}
  Vec3(float a, float b, float c) : x(a), y(b), z(c) {}
  Vec3(const Vec3& vec) : x(vec.x), y(vec.y), z(vec.z) {}
  Vec3(const Vec2& xy, float z = 1) : x(xy.x), y(xy.y), z(z) {}
  Vec3(float x, const Vec2& yz) : x(x), y(yz.x), z(yz.y) {}
  Vec3(const Vec4& vec);
};

union Vec4 {
  struct {
    float x, y, z, w;
  };
  struct {
    float r, g, b, a;
  };
  float m[4];

  Vec4() {}
  Vec4(float a) : x(a), y(a), z(a), w(a) {}
  Vec4(float a, float b, float c, float d) : x(a), y(b), z(c), w(d) {}
  Vec4(const Vec4& vec) : x(vec.x), y(vec.y), z(vec.z), w(vec.w) {}
  Vec4(const Vec2& xy, Vec2& zw) : x(xy.x), y(xy.y), z(zw.x), w(zw.y) {}
  Vec4(float x, const Vec3& yzw) : x(x), y(yzw.x), z(yzw.y), w(yzw.z) {}
  Vec4(const Vec3& xyz, float w = 1) : x(xyz.x), y(xyz.y), z(xyz.z), w(w) {}
};

union Mat2 {
  Vec2 rows[2];
  float m[4];
  float m2[2][2];

  Mat2() {}
  Mat2(float a) : m2{{a, 0}, {0, a}} {}
  Mat2(const Mat2& mat) : rows{mat.rows[0], mat.rows[1]} {}
  Mat2(Vec2 a, Vec2 b) : rows{a, b} {}
};

union Mat3 {
  Vec3 rows[3];
  float m[9];
  float m2[3][3];

  Mat3() {}
  Mat3(float a) : m2{{a, 0, 0}, {0, a, 0}, {0, 0, a}} {}
  Mat3(const Mat3& mat) : rows{mat.rows[0], mat.rows[1], mat.rows[2]} {}
  Mat3(Vec3 a, Vec3 b, Vec3 c) : rows{a, b, c} {}
};

union Mat4 {
  Vec4 rows[4];
  float m[16];
  float m2[4][4];

  Mat4() {}
  Mat4(const Mat4& mat)
      : rows{mat.rows[0], mat.rows[1], mat.rows[2], mat.rows[3]} {}
  Mat4(float a) : m2{{a, 0, 0, 0}, {0, a, 0, 0}, {0, 0, a, 0}, {0, 0, 0, a}} {}
  Mat4(Vec4 a, Vec4 b, Vec4 c, Vec4 d) : rows{a, b, c, d} {}
};

union Quat {
  float m[4];
  struct {
    float x, y, z, w;
  };
  struct {
    float i, j, k, real;
  };

  Quat() {}
  Quat(Vec4 v) : x(v.x), y(v.y), z(v.z), w(v.w) {}
  Quat(float b, float c, float d, float a) : x(b), y(c), z(d), w(a) {}
};

// ADDITION
static inline Vec2 operator+(Vec2 a, Vec2 b) {
  return Vec2(a.x + b.x, a.y + b.y);
}

static inline Vec3 operator+(Vec3 a, Vec3 b) {
  return Vec3(a.x + b.x, a.y + b.y, a.z + b.z);
}

static inline Vec4 operator+(Vec4 a, Vec4 b) {
  return Vec4(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
}

// SUBTRACTION
static inline Vec2 operator-(Vec2 a, Vec2 b) {
  return Vec2(a.x - b.x, a.y - b.y);
}

static inline Vec3 operator-(Vec3 a, Vec3 b) {
  return Vec3(a.x - b.x, a.y - b.y, a.z - b.z);
}

static inline Vec4 operator-(Vec4 a, Vec4 b) {
  return Vec4(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
}

// SCALAR MULTIPLICATION
static inline Vec2 operator*(float s, Vec2 v) { return Vec2(s * v.x, s * v.y); }
static inline Vec2 operator*(Vec2 v, float s) { return s * v; }

static inline Vec3 operator*(float s, Vec3 v) {
  return Vec3(s * v.x, s * v.y, s * v.z);
}
static inline Vec3 operator*(Vec3 v, float s) { return s * v; }

static inline Vec4 operator*(float s, Vec4 v) {
  return Vec4(s * v.x, s * v.y, s * v.z, s * v.w);
}
static inline Vec4 operator*(Vec4 v, float s) { return s * v; }

// SCALAR DIVISION
static inline Vec2 operator/(Vec2 v, float s) {
  s = 1.0f / s;
#ifdef _DEBUG
  assert(__fabs(s) > FLT_EPSILON);
#endif
  return v * s;
}

static inline Vec3 operator/(Vec3 v, float s) {
  s = 1.0f / s;
#ifdef _DEBUG
  assert(__fabs(s) > FLT_EPSILON);
#endif
  return v * s;
}

static inline Vec4 operator/(Vec4 v, float s) {
  s = 1.0f / s;
#ifdef _DEBUG
  assert(__fabs(s) > FLT_EPSILON);
#endif
  return v * s;
}

// MULTIPLICATION
static inline Vec2 operator*(Vec2 l, Vec2 r) {
  return Vec2(l.x * r.x, l.y * r.y);
}

static inline Vec3 operator*(Vec3 l, Vec3 r) {
  return Vec3(l.x * r.x, l.y * r.y, l.z * r.z);
}

static inline Vec4 operator*(Vec4 l, Vec4 r) {
  return Vec4(l.x * r.x, l.y * r.y, l.z * r.z, l.w * r.w);
}

// DIVISION
static inline Vec2 operator/(Vec2 l, Vec2 r) {
#ifdef _DEBUG
  assert(__fabs(r.x) > FLT_EPSILON);
  assert(__fabs(r.y) > FLT_EPSILON);
#endif
  return Vec2(l.x / r.x, l.y / r.y);
}

static inline Vec3 operator/(Vec3 l, Vec3 r) {
#ifdef _DEBUG
  assert(__fabs(r.x) > FLT_EPSILON);
  assert(__fabs(r.y) > FLT_EPSILON);
  assert(__fabs(r.z) > FLT_EPSILON);
#endif
  return Vec3(l.x / r.x, l.y / r.y, l.z / r.z);
}

static inline Vec4 operator/(Vec4 l, Vec4 r) {
#ifdef _DEBUG
  assert(__fabs(r.x) > FLT_EPSILON);
  assert(__fabs(r.y) > FLT_EPSILON);
  assert(__fabs(r.z) > FLT_EPSILON);
  assert(__fabs(r.w) > FLT_EPSILON);
#endif
  return Vec4(l.x / r.x, l.y / r.y, l.z / r.z, l.w / r.w);
}

// NEGATION
static inline Vec2 operator-(const Vec2& v) { return Vec2(-v.x, -v.y); }

static inline Vec3 operator-(const Vec3& v) { return Vec3(-v.x, -v.y, -v.z); }

static inline Vec4 operator-(const Vec4& v) {
  return Vec4(-v.x, -v.y, -v.z, -v.w);
}

// ADDITION ASSIGNMENT
static inline Vec2& operator+=(Vec2& a, Vec2 b) {
  a = a + b;
  return a;
}

static inline Vec3& operator+=(Vec3& a, Vec3 b) {
  a = a + b;
  return a;
}

static inline Vec4& operator+=(Vec4& a, Vec4 b) {
  a = a + b;
  return a;
}

// SUBTRACTION ASSIGNMENT
static inline Vec2& operator-=(Vec2& a, Vec2 b) {
  a = a - b;
  return a;
}

static inline Vec3& operator-=(Vec3& a, Vec3 b) {
  a = a - b;
  return a;
}

static inline Vec4& operator-=(Vec4& a, Vec4 b) {
  a = a - b;
  return a;
}

// SCALAR MULTIPLICATION ASSIGNMENT
static inline Vec2& operator*=(Vec2& t, float s) {
  t = t * s;
  return t;
}

static inline Vec3& operator*=(Vec3& t, float s) {
  t = t * s;
  return t;
}

static inline Vec4& operator*=(Vec4& t, float s) {
  t = t * s;
  return t;
}

// SCALAR DIVISION ASSIGNMENT
static inline Vec2& operator/=(Vec2& t, float s) {
  t = t / s;
  return t;
}

static inline Vec3& operator/=(Vec3& t, float s) {
  t = t / s;
  return t;
}

static inline Vec4& operator/=(Vec4& t, float s) {
  t = t / s;
  return t;
}

// MULTIPLICATION ASSIGNMENT
static inline Vec2& operator*=(Vec2& t, Vec2 s) {
  t = t * s;
  return t;
}

static inline Vec3& operator*=(Vec3& t, Vec3 s) {
  t = t * s;
  return t;
}

static inline Vec4& operator*=(Vec4& t, Vec4 s) {
  t = t * s;
  return t;
}

// DIVISION ASSIGNMENT
static inline Vec2& operator/=(Vec2& t, Vec2 s) {
  t = t / s;
  return t;
}

static inline Vec3& operator/=(Vec3& t, Vec3 s) {
  t = t / s;
  return t;
}

static inline Vec4& operator/=(Vec4& t, Vec4 s) {
  t = t / s;
  return t;
}

// DOT PRODUCT
static inline float DotProduct(Vec2 a, Vec2 b) { return a.x * b.x + a.y * b.y; }

static inline float DotProduct(Vec3 a, Vec3 b) {
  return a.x * b.x + a.y * b.y + a.z * b.z;
}

static inline float DotProduct(Vec4 a, Vec4 b) {
  return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}
static inline float Determinant(Vec2 a, Vec2 b) {
  return (a.x * b.y) - (a.y * b.x);
}

// CROSS PRODUCT
static inline Vec3 CrossProduct(Vec2 a, Vec2 b) {
  Vec3 res;
  res.x = 0;
  res.y = 0;
  res.z = (a.x * b.y) - (a.y * b.x);
  return res;
}

static inline Vec3 CrossProduct(Vec3 a, Vec3 b) {
  Vec3 res;
  res.x = (a.y * b.z) - (a.z * b.y);
  res.y = (a.z * b.x) - (a.x * b.z);
  res.z = (a.x * b.y) - (a.y * b.x);
  return res;
}

static inline float LengthSq(Vec2 v) { return DotProduct(v, v); }
static inline float LengthSq(Vec3 v) { return DotProduct(v, v); }
static inline float LengthSq(Vec4 v) { return DotProduct(v, v); }
static inline float Length(Vec2 v) { return sqrtf(DotProduct(v, v)); }
static inline float Length(Vec3 v) { return sqrtf(DotProduct(v, v)); }
static inline float Length(Vec4 v) { return sqrtf(DotProduct(v, v)); }
static inline float Distance(float a, float b) { return b - a; }
static inline float Distance(Vec2 a, Vec2 b) { return Length(b - a); }
static inline float Distance(Vec3 a, Vec3 b) { return Length(b - a); }
static inline float Distance(Vec4 a, Vec4 b) { return Length(b - a); }

static inline Vec2 NormalizeChecked(Vec2 v) {
  Vec2 res = {};
  float len = Length(v);
  if (len != 0) res = v / len;
  return res;
}
static inline Vec3 NormalizeChecked(Vec3 v) {
  Vec3 res = {};
  float len = Length(v);
  if (len != 0) res = v / len;
  return res;
}
static inline Vec4 NormalizeChecked(Vec4 v) {
  Vec4 res = {};
  float len = Length(v);
  if (len != 0) res = v * (1.0f / len);
  return res;
}
static inline Vec2 Normalize(Vec2 v) {
  Vec2 res = {};
  float len = Length(v);
#ifdef _DEBUG
  assert(len != 0);
#endif
  res = v / len;
  return res;
}
static inline Vec3 Normalize(Vec3 v) {
  Vec3 res = {};
  float len = Length(v);
#ifdef _DEBUG
  assert(len != 0);
#endif
  res = v / len;
  return res;
}
static inline Vec4 Normalize(Vec4 v) {
  Vec4 res = {};
  float len = Length(v);
#ifdef _DEBUG
  assert(len != 0);
#endif
  res = v * (1.0f / len);
  return res;
}

static inline Vec2 ClampVec(Vec2 min, Vec2 max, Vec2 v) {
  v.x = Clamp(min.x, max.x, v.x);
  v.y = Clamp(min.y, max.y, v.y);
  return v;
}
static inline Vec3 ClampVec(Vec3 min, Vec3 max, Vec3 v) {
  v.x = Clamp(min.x, max.x, v.x);
  v.y = Clamp(min.y, max.y, v.y);
  v.z = Clamp(min.z, max.z, v.z);
  return v;
}
static inline Vec4 ClampVec(Vec4 min, Vec4 max, Vec4 v) {
  v.x = Clamp(min.x, max.x, v.x);
  v.y = Clamp(min.y, max.y, v.y);
  v.z = Clamp(min.z, max.z, v.z);
  v.w = Clamp(min.w, max.w, v.w);
  return v;
}

float Determinant(const Mat2& mat);
Mat2 Inverse(const Mat2& mat);
Mat2 Transpose(const Mat2& m);
float Determinant(const Mat3& mat);
Mat3 Inverse(const Mat3& mat);
Mat3 Transpose(const Mat3& m);
float Determinant(const Mat4& mat);
Mat4 Inverse(const Mat4& mat);
Mat4 Transpose(const Mat4& m);

Mat2 operator*(const Mat2& left, const Mat2& right);
Vec2 operator*(const Mat2& mat, Vec2 vec);
Vec2 operator*(Vec2 vec, const Mat2& mat);
Mat3 operator*(const Mat3& left, const Mat3& right);
Vec3 operator*(const Mat3& mat, Vec3 vec);
Mat4 operator*(const Mat4& left, const Mat4& right);
Vec4 operator*(const Mat4& mat, Vec4 vec);

static inline Mat2& operator*=(Mat2& t, Mat2& o) {
  t = t * o;
  return t;
}
static inline Mat3& operator*=(Mat3& t, Mat3& o) {
  t = t * o;
  return t;
}
static inline Mat4& operator*=(Mat4& t, Mat4& o) {
  t = t * o;
  return t;
}

Mat2 ScalingMat2(float x, float y);
Mat2 ScalingMat2(Vec2 s);
Mat3 ScalingMat3(float S_1, float S_2);
Mat3 ScalingMat3(Vec2 s);
Mat4 ScalingMat4(float S_1, float S_2, float S_3);
Mat4 ScalingMat4(Vec3 s);
Mat2 RotationMat2(float angle);
Mat3 RotationMat3(float angle);
Mat4 RotationX(float angle);
Mat4 RotationY(float angle);
Mat4 RotationZ(float angle);
Mat4 RotationMat4(float x, float y, float z, float angle);
Mat4 RotationMat4(Vec3 axis, float angle);
Mat3 TranslationMat3(float T_x, float T_y);
Mat3 TranslationMat3(Vec2 t);
Mat4 TranslationMat4(float T_x, float T_y, float T_z);
Mat4 TranslationMat4(Vec3 t);
Mat3 LookAt(Vec2 from, Vec2 to, Vec2 forward);
Mat4 LookAtDirection(Vec3 dir, Vec3 world_up);
Mat4 LookAt(Vec3 from, Vec3 to, Vec3 world_up);
Mat4 LookAtChecked(Vec3 from, Vec3 to, Vec3 world_up);
Mat4 OrthographicProjectionRH(float l, float r, float t, float b, float n,
                              float f);
Mat4 OrthographicProjectionLH(float l, float r, float t, float b, float n,
                              float f);
Mat4 PerspectiveProjectionRH(float fov, float aspect_ratio, float n, float f);
Mat4 PerspectiveProjectionLH(float fov, float aspect_ratio, float n, float f);
Vec3 GetRight(const Mat4& m);
Vec3 GetUp(const Mat4& m);
Vec3 GetForward(const Mat4& m);

Mat2 ToMat2(const Mat3& mat);
Mat3 ToMat3(const Mat2& mat);
Mat3 ToMat3(const Mat4& mat);
Mat4 ToMat4(const Mat3& mat);
#endif
