#include "Shader.h"

#include <stdio.h>

#include "Math.h"
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

void Shader::PushVAO(std::initializer_list<float> values) {
  for (float value : values) {
    VAO[count++] = value;
  }
}

void Shader::ClearVAO() { count = 0; }

void Shader::Draw(DWORD* surface, int width, int height) {
  for (int offset = 0; offset < count; offset += 13) {
    Vec3 base = Vec3(VAO[offset], VAO[offset + 1], VAO[offset + 2]);
    Vec3 u = Vec3(VAO[offset + 3], VAO[offset + 4], VAO[offset + 5]) - base;
    Vec3 v = Vec3(VAO[offset + 6], VAO[offset + 7], VAO[offset + 8]) - base;
    Texture& texture = Texture::byId[(int)VAO[offset + 12]];
    POINT points[4];
    for (int x = 0; x < 16; x++) {
      for (int y = 0; y < 16; y++) {
        Vec3 from = base + u * x / 16 + v * y / 16;
        Vec3 toX = base + u * (x + 1) / 16 + v * y / 16;
        Vec3 toY = base + u * x / 16 + v * (y + 1) / 16;
        Vec3 to = base + u * (x + 1) / 16 + v * (y + 1) / 16;
        points[0] = {(int)(width * from[0]), (int)(height * from[1])};
        points[1] = {(int)(width * toX[0]), (int)(height * toX[1])};
        points[2] = {(int)(width * to[0]), (int)(height * to[1])};
        points[3] = {(int)(width * toY[0]), (int)(height * toY[1])};
        setfillcolor(BGR(texture(x, y) & 0xffffff));
        solidpolygon(points, 4);
      }
    }
  }
}