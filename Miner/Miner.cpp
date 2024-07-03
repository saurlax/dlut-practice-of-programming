#include <graphics.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <map>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define FPS_MAX 100

using namespace std;

HWND hwnd;
DWORD* buf;
ExMessage msg;
ULONGLONG tick;

bool running = true;

class Vec3 {
 public:
  float data[3];

  Vec3(float x = 0, float y = 0, float z = 0) {
    data[0] = x;
    data[1] = y;
    data[2] = z;
  }

  Vec3 operator+(const Vec3& other) const {
    return Vec3(data[0] + other.data[0], data[1] + other.data[1],
                data[2] + other.data[2]);
  }

  Vec3 operator-(const Vec3& other) const {
    return Vec3(data[0] - other.data[0], data[1] - other.data[1],
                data[2] - other.data[2]);
  }
};

class Vec4 {
 public:
  float data[4];

  Vec4(float x = 0, float y = 0, float z = 0, float w = 0) {
    data[0] = x;
    data[1] = y;
    data[2] = z;
    data[3] = w;
  }

  Vec4 operator+(const Vec4& other) const {
    return Vec4(data[0] + other.data[0], data[1] + other.data[1],
                data[2] + other.data[2], data[3] + other.data[3]);
  }

  Vec4 operator-(const Vec4& other) const {
    return Vec4(data[0] - other.data[0], data[1] - other.data[1],
                data[2] - other.data[2], data[3] - other.data[3]);
  }
};

class Mat3 {
 public:
  float data[3][3];

  Mat3 operator+(const Mat3& other) const {
    Mat3 result;
    for (int i = 0; i < 3; ++i) {
      for (int j = 0; j < 3; ++j) {
        result.data[i][j] = data[i][j] + other.data[i][j];
      }
    }
    return result;
  }

  Mat3 operator-(const Mat3& other) const {
    Mat3 result;
    for (int i = 0; i < 3; ++i) {
      for (int j = 0; j < 3; ++j) {
        result.data[i][j] = data[i][j] - other.data[i][j];
      }
    }
    return result;
  }

  Mat3 operator*(const Mat3& other) const {
    Mat3 result;
    for (int i = 0; i < 3; ++i) {
      for (int j = 0; j < 3; ++j) {
        result.data[i][j] = 0;
        for (int k = 0; k < 3; ++k) {
          result.data[i][j] += data[i][k] * other.data[k][j];
        }
      }
    }
    return result;
  }
};

class Mat4 {
 public:
  float data[4][4];

  Mat4 operator+(const Mat4& other) const {
    Mat4 result;
    for (int i = 0; i < 4; ++i) {
      for (int j = 0; j < 4; ++j) {
        result.data[i][j] = data[i][j] + other.data[i][j];
      }
    }
    return result;
  }

  Mat4 operator-(const Mat4& other) const {
    Mat4 result;
    for (int i = 0; i < 4; ++i) {
      for (int j = 0; j < 4; ++j) {
        result.data[i][j] = data[i][j] - other.data[i][j];
      }
    }
    return result;
  }

  Mat4 operator*(const Mat4& other) const {
    Mat4 result;
    for (int i = 0; i < 4; ++i) {
      for (int j = 0; j < 4; ++j) {
        result.data[i][j] = 0;
        for (int k = 0; k < 4; ++k) {
          result.data[i][j] += data[i][k] * other.data[k][j];
        }
      }
    }
    return result;
  }
};

class Texture {
  IMAGE image;

 public:
  Texture() {}
  Texture(LPCTSTR path) { loadimage(&image, path); }

  DWORD operator()(int x, int y) {
    DWORD* buffer = GetImageBuffer(&image);
    x = x % 16;
    y = y % 16;
    return buffer[y * 16 + x];
  }
};

class Block {
 public:
  Texture* textures[6];
  Block() {}
  Block(Texture* top, Texture* bottom, Texture* front, Texture* back,
        Texture* left, Texture* right) {
    textures[0] = top;
    textures[1] = bottom;
    textures[2] = front;
    textures[3] = back;
    textures[4] = left;
    textures[5] = right;
  }

  Block(Texture* top, Texture* bottom, Texture* side)
      : Block(top, bottom, side, side, side, side) {}

  Block(Texture* all) : Block(all, all, all) {}
};

class Chunk {
 public:
  char blocks[16][16][256];
};

class World {
 public:
  map<pair<int, int>, Chunk> chunks;
};

Texture txts[32];
Block blocks[32];

void init() {
  printf("[init]\n");
  srand(time(NULL));
  tick = GetTickCount();

  hwnd = initgraph(WINDOW_WIDTH, WINDOW_HEIGHT);

  printf("[asset] loading\n");
  txts[0] = Texture(L"assets/dirt.png");
  txts[1] = Texture(L"assets/grass_block_side.png");
  txts[2] = Texture(L"assets/grass_block_top.png");

  blocks[0] = Block(&txts[0]);
  blocks[1] = Block(&txts[2], &txts[0], &txts[1]);
  printf("[asset] loaded\n");

  buf = GetImageBuffer();
}

void input() {
  if (!IsWindow(hwnd)) running = false;
  while (peekmessage(&msg)) {
    if (msg.message == WM_KEYDOWN || msg.message == WM_KEYUP) {
      bool down = msg.message == WM_KEYDOWN;
      switch (msg.vkcode) {
        case VK_ESCAPE:
          running = false;
          break;
      }
    }
  }
}

void update(int last) {}

void render(int last) {
  for (int x = 0; x < WINDOW_WIDTH; x++) {
    for (int y = 0; y < WINDOW_HEIGHT; y++) {
      buf[y * WINDOW_WIDTH + x] = txts[1](x, y);
    }
  }
  wchar_t text[100];
  swprintf(text, 100, L"FPS: %d", 1000 / last);
  outtextxy(0, 0, text);
  FlushBatchDraw();
}

int main() {
  init();
  while (running) {
    ULONGLONG now = GetTickCount();
    input();
    int last = now - tick;
    if (last * FPS_MAX < 900) continue;
    tick = now;
    update(last);
    render(last);
  }
  closegraph();
  return 0;
}