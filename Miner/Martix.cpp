#pragma once

class Vec3 {
 public:
  float data[3];

  Vec3(float x = 0, float y = 0, float z = 0) {
    data[0] = x;
    data[1] = y;
    data[2] = z;
  }

  Vec3 operator+(const Vec3 &other) const {
    return Vec3(data[0] + other.data[0], data[1] + other.data[1],
                data[2] + other.data[2]);
  }

  Vec3 operator-(const Vec3 &other) const {
    return Vec3(data[0] - other.data[0], data[1] - other.data[1],
                data[2] - other.data[2]);
  }
};

class Vec4 {
 public:
  float data[4];

  Vec4(float x = 0, float y = 0, float z = 0, float w = 0) {
    data[0] = x;
    data[1] = y;
    data[2] = z;
    data[3] = w;
  }

  Vec4 operator+(const Vec4 &other) const {
    return Vec4(data[0] + other.data[0], data[1] + other.data[1],
                data[2] + other.data[2], data[3] + other.data[3]);
  }

  Vec4 operator-(const Vec4 &other) const {
    return Vec4(data[0] - other.data[0], data[1] - other.data[1],
                data[2] - other.data[2], data[3] - other.data[3]);
  }
};

class Mat3 {
 public:
  float data[3][3];

  Mat3 operator+(const Mat3 &other) const {
    Mat3 result;
    for (int i = 0; i < 3; ++i) {
      for (int j = 0; j < 3; ++j) {
        result.data[i][j] = data[i][j] + other.data[i][j];
      }
    }
    return result;
  }

  Mat3 operator-(const Mat3 &other) const {
    Mat3 result;
    for (int i = 0; i < 3; ++i) {
      for (int j = 0; j < 3; ++j) {
        result.data[i][j] = data[i][j] - other.data[i][j];
      }
    }
    return result;
  }

  Mat3 operator*(const Mat3 &other) const {
    Mat3 result;
    for (int i = 0; i < 3; ++i) {
      for (int j = 0; j < 3; ++j) {
        result.data[i][j] = 0;
        for (int k = 0; k < 3; ++k) {
          result.data[i][j] += data[i][k] * other.data[k][j];
        }
      }
    }
    return result;
  }
};

class Mat4 {
 public:
  float data[4][4];

  Mat4 operator+(const Mat4 &other) const {
    Mat4 result;
    for (int i = 0; i < 4; ++i) {
      for (int j = 0; j < 4; ++j) {
        result.data[i][j] = data[i][j] + other.data[i][j];
      }
    }
    return result;
  }

  Mat4 operator-(const Mat4 &other) const {
    Mat4 result;
    for (int i = 0; i < 4; ++i) {
      for (int j = 0; j < 4; ++j) {
        result.data[i][j] = data[i][j] - other.data[i][j];
      }
    }
    return result;
  }

  Mat4 operator*(const Mat4 &other) const {
    Mat4 result;
    for (int i = 0; i < 4; ++i) {
      for (int j = 0; j < 4; ++j) {
        result.data[i][j] = 0;
        for (int k = 0; k < 4; ++k) {
          result.data[i][j] += data[i][k] * other.data[k][j];
        }
      }
    }
    return result;
  }
};