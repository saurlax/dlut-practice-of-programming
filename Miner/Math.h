#pragma once

class Vec3 {
 public:
  float data[3];

  Vec3(float x = 0, float y = 0, float z = 0);

  Vec3 operator+(const Vec3& other) const;
  Vec3 operator-(const Vec3& other) const;
  Vec3 normalize() const;
  Vec3 cross(Vec3 other) const;
};

class Mat4 {
 public:
  float data[4][4];

  Mat4 operator+(const Mat4& other) const;
  Mat4 operator-(const Mat4& other) const;
  Mat4 operator*(const Mat4& other) const;

  static Mat4 perspective(float fov, float aspectRatio, float nearPlane,
                          float farPlane);
  static Mat4 lookAt(const Vec3& eye, const Vec3& center, const Vec3& up);
};

class Vec4 {
 public:
  float data[4];

  Vec4(float x = 0, float y = 0, float z = 0, float w = 0);

  Vec4 operator+(const Vec4& other) const;
  Vec4 operator-(const Vec4& other) const;
  Vec4 operator*(const Mat4& m) const;

  Vec3 toVec3() const;
  Vec4 perspectiveDivide() const;
};