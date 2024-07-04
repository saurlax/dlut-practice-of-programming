#pragma once
#include <graphics.h>

#include <initializer_list>

#define BUFFER_SIZE 800 * 600

class Texture {
  static int count;
  IMAGE image;

 public:
  static Texture byId[];
  static int Create(LPCTSTR path);

  DWORD operator()(int x, int y);
};

class Shader {
  // Vertex Array Object
  float VAO[BUFFER_SIZE];
  int count;

 public:
  void PushVAO(std::initializer_list<float> values);
  void ClearVAO();
  void Draw(DWORD* surface, int width, int height);
};