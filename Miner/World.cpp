#include "World.h"

Texture Texture::byId[TYPE_MAX];
Block Block::byId[TYPE_MAX];
int Texture::count = 0;
int Block::count = 0;

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

int Block::Create(Texture* top, Texture* bottom, Texture* front, Texture* back,
                  Texture* left, Texture* right) {
  Block* block = &byId[count++];
  block->textures[0] = top;
  block->textures[1] = bottom;
  block->textures[2] = front;
  block->textures[3] = back;
  block->textures[4] = left;
  block->textures[5] = right;
  return count - 1;
}

int Block::Create(Texture* top, Texture* bottom, Texture* side) {
  return Create(top, bottom, side, side, side, side);
}

int Block::Create(Texture* all) { return Create(all, all, all); }
