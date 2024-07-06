#include "World.h"

#include <cstdlib>
#include <ctime>

map<string, int> ID = {{"grass_block", 1}, {"dirt", 2}, {"stone", 3}};

World::World() {
  srand(time(0));
  seed = rand();
}

char& Chunk::operator()(int x, int y) { return data[x][y]; }

char& World::operator()(int x, int y) {
  if (y < 0 || y >= 128) printf("Block out of bound: %d %d\n", x, y);
  if (chunks.find(x / 16) == chunks.end()) {
    Chunk chunk;
    srand(seed * (x / 16 - 1));
    noise[x / 16 - 1] = rand() % 16 + 64;
    srand(seed * x / 16);
    noise[x / 16] = rand() % 16 + 64;
    srand(seed * (x / 16 + 1));
    noise[x / 16 + 1] = rand() % 16 + 64;

    int x1 = x - 16, x2 = x, x3 = x + 16;
    int y1 = noise[x / 16 - 1], y2 = noise[x / 16], y3 = noise[x / 16 + 1];
    float denom = (x1 - x2) * (x1 - x3) * (x2 - x3);
    float a = (x3 * (y2 - y1) + x2 * (y1 - y3) + x1 * (y3 - y2)) / denom;
    float b =
        (x3 * x3 * (y1 - y2) + x2 * x2 * (y3 - y1) + x1 * x1 * (y2 - y3)) /
        denom;
    float c = (x2 * x3 * (x2 - x3) * y1 + x3 * x1 * (x3 - x1) * y2 +
               x1 * x2 * (x1 - x2) * y3) /
              denom;

    for (int cx = 0; cx < 16; cx++) {
      int h = a * (x + cx) * (x + cx) + b * (x + cx) + c;
      for (int cy = h; cy < 128; cy++) {
        if (cy < 0) continue;
        switch (cy - h) {
          case 0:
            chunk(cx, cy) = 1;
            break;
          case 1:
          case 2:
          case 3:
          case 4:
            chunk(cx, cy) = 2;
            break;
          default:
            chunk(cx, cy) = 3;
            break;
        }
      }
    }

    chunks[x / 16] = chunk;
  }
  return chunks[x / 16]((x % 16 + 16) % 16, y);
}

char World::safeRead(int x, int y) {
  if (y < 0 || y >= 128) return 0;
  return this->operator()(x, y);
}