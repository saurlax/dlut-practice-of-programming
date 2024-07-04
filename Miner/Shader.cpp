#include "Shader.h"

Texture Texture::byId[256];
int Texture::count = 0;

int Texture::Create(LPCTSTR path) {
  Texture* texture = &byId[count++];
  loadimage(&texture->image, path);
  return count - 1;
}

DWORD Texture::operator()(int x, int y) {
  DWORD* buffer = GetImageBuffer(&image);
  x = x % 16;
  y = y % 16;
  return buffer[y * 16 + x];
}
