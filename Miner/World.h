#pragma once
#include <graphics.h>

#include <map>

#include "Math.h"

#define TYPE_MAX 256

class Texture {
  static int count;
  IMAGE image;

 public:
  static Texture byId[TYPE_MAX];
  static int Create(LPCTSTR path);

  DWORD operator()(int x, int y);
};

class Block {
  static int count;

 public:
  static Block byId[TYPE_MAX];
  Texture* textures[6];
  static int Create(Texture* top, Texture* bottom, Texture* front,
                    Texture* back, Texture* left, Texture* right);
  static int Create(Texture* top, Texture* bottom, Texture* side);
  static int Create(Texture* all);
};

class Chunk {
 public:
  Block blocks[16][16][256];
  Chunk();
};

class World {
  std::map<std::pair<int, int>, Chunk> chunks;

 public:
  World();
};
