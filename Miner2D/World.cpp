#include "World.h"

map<string, int> ID = {{"grass_block", 1}, {"dirt", 2}, {"stone", 3}};

char& Chunk::operator()(int x, int y) { return data[x][y]; }

char& World::operator()(int x, int y) {
  if (chunks.find(x / 16) == chunks.end()) {
    Chunk chunk;
    for (int u = 0; u < 16; u++) {
      chunk(u, u) = ID["dirt"];
    }
    chunks[x / 16] = chunk;
  }
  return chunks[x / 16]((x % 16 + 16) % 16, y);
}
