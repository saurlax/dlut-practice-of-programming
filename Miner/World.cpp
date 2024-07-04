#include "World.h"

Block Block::byId[256];
int Block::count = 0;

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
