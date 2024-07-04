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
    Vec3 a = {VAO[offset + 0], VAO[offset + 1], VAO[offset + 2]};
    Vec3 b = {VAO[offset + 3], VAO[offset + 4], VAO[offset + 5]};
    Vec3 c = {VAO[offset + 6], VAO[offset + 7], VAO[offset + 8]};
    Vec3 d = {VAO[offset + 9], VAO[offset + 10], VAO[offset + 11]};
    Texture& texture = Texture::byId[(int)VAO[offset + 12]];
    for (int x = 0; x < 16; x++) {
      for (int y = 0; y < 16; y++) {
        float u1 = x / 16.0f;
        float v1 = y / 16.0f;
        float u2 = (x + 1) / 16.0f;
        float v2 = (y + 1) / 16.0f;
        Vec3 p1 = a + (b - a) * u1 + (d - a) * v1 + (a - b + c - d) * u1 * v1;
        Vec3 p2 = a + (b - a) * u2 + (d - a) * v1 + (a - b + c - d) * u2 * v1;
        Vec3 p3 = a + (b - a) * u2 + (d - a) * v2 + (a - b + c - d) * u2 * v2;
        Vec3 p4 = a + (b - a) * u1 + (d - a) * v2 + (a - b + c - d) * u1 * v2;
        POINT points[4];
        points[0] = {(int)(width * p1[0]), (int)(height * p1[1])};
        points[1] = {(int)(width * p2[0]), (int)(height * p2[1])};
        points[2] = {(int)(width * p3[0]), (int)(height * p3[1])};
        points[3] = {(int)(width * p4[0]), (int)(height * p4[1])};
        setfillcolor(BGR(texture(x, y) & 0xffffff));
        solidpolygon(points, 4);
      }
    }
  }
}