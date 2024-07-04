#pragma once
#include "Math.h"

class Camera {
 public:
  Vec3 position;
  float aspectRatio;
  float viewDistance = 1000.0f;
  float yaw;
  float pitch;
  float fov = 45.0f;

  Mat4 GetViewMatrix() const;
  Mat4 GetProjectionMatrix() const;
};