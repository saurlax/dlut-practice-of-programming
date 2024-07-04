#pragma once
#include <graphics.h>

class Texture {
  static int count;
  IMAGE image;

 public:
  static Texture byId[];
  static int Create(LPCTSTR path);

  DWORD operator()(int x, int y);
};
