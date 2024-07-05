#include "World.h"

char& World::operator()(int x, int y) {
  char(&chunk)[16][128] = chunks[x / 16];
  return chunk[x % 16][y];
}