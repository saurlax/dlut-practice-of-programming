#include <graphics.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <map>

#include "Math.h"
#include "Shader.h"
#include "World.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define FPS_MAX 60
#define DEBUG_MAX 1000

HWND hwnd;
DWORD* surface;
ExMessage msg;
ULONGLONG tick;

World world;
Camera camera;
Shader shader;

bool running = true;
bool debug = true;
wchar_t debugText[DEBUG_MAX];

const Face FACE_UP = {{0, 1, 0}, {1, 1, 0}, {1, 1, 1}, {0, 1, 1}};
const Face FACE_DOWN = {{0, 0, 1}, {1, 0, 1}, {1, 0, 0}, {0, 0, 0}};
const Face FACE_FRONT = {{0, 0, 1}, {1, 0, 1}, {1, 1, 1}, {0, 1, 1}};
const Face FACE_BACK = {{0, 0, 0}, {0, 1, 0}, {1, 1, 0}, {1, 0, 0}};
const Face FACE_LEFT = {{0, 0, 0}, {0, 0, 1}, {0, 1, 1}, {0, 1, 0}};
const Face FACE_RIGHT = {{1, 0, 0}, {1, 1, 0}, {1, 1, 1}, {1, 0, 1}};

void init() {
  srand(time(NULL));
  tick = GetTickCount();

  hwnd = initgraph(WINDOW_WIDTH, WINDOW_HEIGHT);

  printf("[asset] loading\n");
  Texture::Create(L"assets/dirt.png");
  Texture::Create(L"assets/grass_block_top.png");
  Texture::Create(L"assets/grass_block_side.png");
  printf("[asset] loaded\n");

  Block::Create(&Texture::byId[1], &Texture::byId[0], &Texture::byId[2]);
  Block::Create(&Texture::byId[0]);
  surface = GetImageBuffer();
  shader.width = WINDOW_WIDTH;
  shader.height = WINDOW_HEIGHT;
  shader.surface = surface;
  camera.aspectRatio = WINDOW_WIDTH / WINDOW_HEIGHT;

  // test world
  shader.PushBuffer(
      {FACE_UP, FACE_DOWN, FACE_FRONT, FACE_BACK, FACE_LEFT, FACE_RIGHT});
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
        case 'W':
        case VK_UP:
          camera.position[2]++;
          break;
        case 'S':
        case VK_DOWN:
          camera.position[2]--;
          break;
        case 'A':
        case VK_LEFT:
          camera.position[0]--;
          break;
        case 'D':
        case VK_RIGHT:
          camera.position[0]++;
          break;
      }
    }
  }
}

void update(int delta) {}

void render(int delta) {
  BeginBatchDraw();
  shader.Draw(camera);
  swprintf(debugText, DEBUG_MAX, L"FPS(%d)    POS(%f,%f,%f)", 1000 / delta,
           camera.position[0], camera.position[1], camera.position[2]);
  outtextxy(0, 0, debugText);
  FlushBatchDraw();
}

int main() {
  init();
  while (running) {
    ULONGLONG now = GetTickCount();
    int delta = now - tick;
    input();
    if (delta * FPS_MAX < 1000) continue;
    tick = now;
    update(delta);
    render(delta);
  }
  closegraph();
  return 0;
}
