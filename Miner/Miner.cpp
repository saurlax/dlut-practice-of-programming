#include <graphics.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <map>

#include "Math.h"
#include "World.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define FPS_MAX 100

using namespace std;

HWND hwnd;
DWORD* surface;
ExMessage msg;
ULONGLONG tick;

bool running = true;

Vec3 cameraPos = Vec3(0, 3, 0);
Vec3 cameraDirection = Vec3(0, -1, 0);

void init() {
  printf("[init]\n");
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
  BeginBatchDraw();
  for (int x = 0; x < WINDOW_WIDTH; x++) {
    for (int y = 0; y < WINDOW_HEIGHT; y++) {
      surface[y * WINDOW_WIDTH + x] =
          RGB(rand() % 256, rand() % 256, rand() % 256);
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
