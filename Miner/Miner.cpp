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

  Vec3(float x = 0, float y = 0, float z = 0);

  Vec3 operator+(const Vec3& other) const;
  Vec3 operator-(const Vec3& other) const;
  Vec3 normalize() const;
  Vec3 cross(Vec3 other) const;
};

class Mat4 {
 public:
  float data[4][4];

  Mat4 operator+(const Mat4& other) const;
  Mat4 operator-(const Mat4& other) const;
  Mat4 operator*(const Mat4& other) const;

  static Mat4 perspective(float fov, float aspectRatio, float nearPlane,
                          float farPlane);
  static Mat4 lookAt(const Vec3& eye, const Vec3& center, const Vec3& up);
};

class Vec4 {
 public:
  float data[4];

  Vec4(float x = 0, float y = 0, float z = 0, float w = 0);

  Vec4 operator+(const Vec4& other) const;
  Vec4 operator-(const Vec4& other) const;
  Vec4 operator*(const Mat4& m) const;

  Vec3 toVec3() const;
  Vec4 perspectiveDivide() const;
};
class Texture {
  IMAGE image;

 public:
  Texture();
  Texture(LPCTSTR path);

  DWORD operator()(int x, int y);
};

class RectFace {
 public:
  Vec3 vertices[4];
  Vec3 normal;
  Texture* texture;

  RectFace();
  RectFace(Vec3 vertices[], Vec3 normal, Texture* texture);
};

class Block {
 public:
  Texture* textures[6];
  Block();
  Block(Texture* top, Texture* bottom, Texture* front, Texture* back,
        Texture* left, Texture* right);
  Block(Texture* top, Texture* bottom, Texture* side);
  Block(Texture* all);

  RectFace* getFaces();
};

Vec3 cameraPos = Vec3(0, 3, 0);
Vec3 cameraDirection = Vec3(0, -1, 0);

Vec3 faceUp[4], faceDown[4], faceFront[4], faceBack[4], faceLeft[4],
    faceRight[4];
Vec3 normalUp, normalDown, normalFront, normalBack, normalLeft, normalRight;

Texture txts[32];
Block blocks[32];
map<pair<int, int>, char[16][16][256]> world;

void init();
void input();
void update(int last);
void render(int last);

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

Vec3::Vec3(float x, float y, float z) {
  data[0] = x;
  data[1] = y;
  data[2] = z;
}

Vec3 Vec3::operator+(const Vec3& other) const {
  return Vec3(data[0] + other.data[0], data[1] + other.data[1],
              data[2] + other.data[2]);
}

Vec3 Vec3::operator-(const Vec3& other) const {
  return Vec3(data[0] - other.data[0], data[1] - other.data[1],
              data[2] - other.data[2]);
}

Vec3 Vec3::normalize() const {
  float length =
      sqrt(data[0] * data[0] + data[1] * data[1] + data[2] * data[2]);
  return Vec3(data[0] / length, data[1] / length, data[2] / length);
}

Vec3 Vec3::cross(Vec3 other) const {
  return Vec3(data[1] * other.data[2] - data[2] * other.data[1],
              data[2] * other.data[0] - data[0] * other.data[2],
              data[0] * other.data[1] - data[1] * other.data[0]);
}

Vec4::Vec4(float x, float y, float z, float w) {
  data[0] = x;
  data[1] = y;
  data[2] = z;
  data[3] = w;
}

Vec4 Vec4::operator+(const Vec4& other) const {
  return Vec4(data[0] + other.data[0], data[1] + other.data[1],
              data[2] + other.data[2], data[3] + other.data[3]);
}

Vec4 Vec4::operator-(const Vec4& other) const {
  return Vec4(data[0] - other.data[0], data[1] - other.data[1],
              data[2] - other.data[2], data[3] - other.data[3]);
}

Vec4 Vec4::operator*(const Mat4& m) const {
  Vec4 result;
  for (int i = 0; i < 4; ++i) {
    result.data[i] = 0;
    for (int j = 0; j < 4; ++j) {
      result.data[i] += data[j] * m.data[j][i];
    }
  }
  return result;
}

Vec3 Vec4::toVec3() const { return Vec3(data[0], data[1], data[2]); }

Vec4 Vec4::perspectiveDivide() const {
  return Vec4(data[0] / data[3], data[1] / data[3], data[2] / data[3], 1.0f);
}

Mat4 Mat4::operator+(const Mat4& other) const {
  Mat4 result;
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      result.data[i][j] = data[i][j] + other.data[i][j];
    }
  }
  return result;
}

Mat4 Mat4::operator-(const Mat4& other) const {
  Mat4 result;
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      result.data[i][j] = data[i][j] - other.data[i][j];
    }
  }
  return result;
}

Mat4 Mat4::operator*(const Mat4& other) const {
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

Mat4 Mat4::perspective(float fov, float aspectRatio, float nearPlane,
                       float farPlane) {
  Mat4 result;
  float q = 1.0f / tan(0.5f * fov);
  float a = q / aspectRatio;
  float b = (nearPlane + farPlane) / (nearPlane - farPlane);
  float c = (2.0f * nearPlane * farPlane) / (nearPlane - farPlane);
  result.data[0][0] = a;
  result.data[1][1] = q;
  result.data[2][2] = b;
  result.data[2][3] = -1.0f;
  result.data[3][2] = c;
  result.data[3][3] = 0.0f;
  return result;
}

Mat4 Mat4::lookAt(const Vec3& eye, const Vec3& center, const Vec3& up) {
  Vec3 f = (center - eye).normalize();
  Vec3 r = up.cross(f).normalize();
  Vec3 u = f.cross(r).normalize();

  Mat4 result;
  result.data[0][0] = r.data[0];
  result.data[0][1] = r.data[1];
  result.data[0][2] = r.data[2];
  result.data[0][3] = -r.data[0] * eye.data[0] - r.data[1] * eye.data[1] -
                      r.data[2] * eye.data[2];

  result.data[1][0] = u.data[0];
  result.data[1][1] = u.data[1];
  result.data[1][2] = u.data[2];
  result.data[1][3] = -u.data[0] * eye.data[0] - u.data[1] * eye.data[1] -
                      u.data[2] * eye.data[2];

  result.data[2][0] = f.data[0];
  result.data[2][1] = f.data[1];
  result.data[2][2] = f.data[2];
  result.data[2][3] = -f.data[0] * eye.data[0] - f.data[1] * eye.data[1] -
                      f.data[2] * eye.data[2];

  result.data[3][0] = 0.0f;
  result.data[3][1] = 0.0f;
  result.data[3][2] = 0.0f;
  result.data[3][3] = 1.0f;

  return result;
}

Texture::Texture() {}

Texture::Texture(LPCTSTR path) { loadimage(&image, path); }

DWORD Texture::operator()(int x, int y) {
  DWORD* buffer = GetImageBuffer(&image);
  x = x % 16;
  y = y % 16;
  return buffer[y * 16 + x];
}

RectFace::RectFace() {}

RectFace::RectFace(Vec3 vertices[], Vec3 normal, Texture* texture) {
  for (int i = 0; i < 4; ++i) {
    this->vertices[i] = vertices[i];
  }
  this->normal = normal;
  this->texture = texture;
}

Block::Block() {}

Block::Block(Texture* top, Texture* bottom, Texture* front, Texture* back,
             Texture* left, Texture* right) {
  textures[0] = top;
  textures[1] = bottom;
  textures[2] = front;
  textures[3] = back;
  textures[4] = left;
  textures[5] = right;
}

Block::Block(Texture* top, Texture* bottom, Texture* side)
    : Block(top, bottom, side, side, side, side) {}

Block::Block(Texture* all) : Block(all, all, all) {}

RectFace* Block::getFaces() {
  return new RectFace[6]{RectFace(faceUp, normalUp, textures[0]),
                         RectFace(faceDown, normalDown, textures[1]),
                         RectFace(faceFront, normalFront, textures[2]),
                         RectFace(faceBack, normalBack, textures[3]),
                         RectFace(faceLeft, normalLeft, textures[4]),
                         RectFace(faceRight, normalRight, textures[5])};
}

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

  faceUp[0] = Vec3(0, 0, 0);
  faceUp[1] = Vec3(1, 0, 0);
  faceUp[2] = Vec3(1, 1, 0);
  faceUp[3] = Vec3(0, 1, 0);

  faceDown[0] = Vec3(0, 0, 0);
  faceDown[1] = Vec3(1, 0, 0);
  faceDown[2] = Vec3(1, 1, 0);
  faceDown[3] = Vec3(0, 1, 0);

  faceFront[0] = Vec3(0, 0, 0);
  faceFront[1] = Vec3(1, 0, 0);
  faceFront[2] = Vec3(1, 1, 0);
  faceFront[3] = Vec3(0, 1, 0);

  faceBack[0] = Vec3(0, 0, 0);
  faceBack[1] = Vec3(1, 0, 0);
  faceBack[2] = Vec3(1, 1, 0);
  faceBack[3] = Vec3(0, 1, 0);

  faceLeft[0] = Vec3(0, 0, 0);
  faceLeft[1] = Vec3(1, 0, 0);
  faceLeft[2] = Vec3(1, 1, 0);
  faceLeft[3] = Vec3(0, 1, 0);

  faceRight[0] = Vec3(0, 0, 0);
  faceRight[1] = Vec3(1, 0, 0);
  faceRight[2] = Vec3(1, 1, 0);
  faceRight[3] = Vec3(0, 1, 0);

  normalUp = Vec3(0, 0, 1);
  normalDown = Vec3(0, 0, -1);
  normalFront = Vec3(0, 1, 0);
  normalBack = Vec3(0, -1, 0);
  normalLeft = Vec3(-1, 0, 0);
  normalRight = Vec3(1, 0, 0);

  // test
}

void input() {
  if (!IsWindow(hwnd)) running = false;

  while (peekmessage(&msg)) {
    if (msg.message == WM_KEYDOWN || msg.message == WM_KEYUP) {
      bool down = (msg.message == WM_KEYDOWN);
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
  RectFace* faces = blocks[0].getFaces();

  Vec3 cameraTarget = cameraPos + cameraDirection;
  Mat4 view = Mat4::lookAt(cameraPos, cameraTarget, Vec3(0, 0, 1));
  Mat4 projection = Mat4::perspective(
      3.14159f / 2.0f, (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f,
      100.0f);
  Mat4 viewProjection = projection * view;

  for (int i = 0; i < 6; i++) {
    RectFace face = faces[i];
    for (int j = 0; j < 4; ++j) {
      Vec4 vertex4(face.vertices[j].data[0], face.vertices[j].data[1],
                   face.vertices[j].data[2], 1.0f);
      vertex4 = vertex4 * viewProjection;
      vertex4 = vertex4.perspectiveDivide();

      int x = (vertex4.data[0] * 0.5f + 0.5f) * WINDOW_WIDTH;
      int y = (1 - (vertex4.data[1] * 0.5f + 0.5f)) * WINDOW_HEIGHT;

      buf[y * WINDOW_WIDTH + x] = txts[1](x % 16, y % 16);
    }
  }

  wchar_t text[100];
  swprintf(text, 100, L"FPS: %d", 1000 / last);
  outtextxy(0, 0, text);
  FlushBatchDraw();
}