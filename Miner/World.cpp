#pragma once
#include <map>

using namespace std;

typedef char block_t;

class Chunk {
 public:
  block_t blocks[16][16][256];
};

class World {
 public:
  map<pair<int, int>, Chunk> chunks;
};