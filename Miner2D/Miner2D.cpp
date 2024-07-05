#include <graphics.h>

#include <cmath>
#include <map>
#include <string>

#include "World.h"

#define FPS_MAX 100
#define ID_MAX 128
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define BG_COLOR 0xc9f1ff

using namespace std;

HWND hwnd;
DWORD* surface;
ExMessage msg;
ULONGLONG last = GetTickCount();
World world;
bool running = true;
bool debug = true;
wstring debugText;

float playerX = 0;
float playerY = 0;
float cameraX = 0;
float cameraY = 0;

IMAGE textures[ID_MAX];

void init() {
  hwnd = initgraph(WINDOW_WIDTH, WINDOW_HEIGHT);
  surface = GetImageBuffer();
  for (auto p : ID) {
    wstring path =
        L"assets/" + wstring(p.first.begin(), p.first.end()) + L".png";
    loadimage(&textures[p.second], path.c_str());
  }
}

void input() {
  while (peekmessage(&msg)) {
    if (msg.message == WM_KEYDOWN) {
      switch (msg.vkcode) {
        case VK_ESCAPE:
          running = false;
          break;
        case VK_TAB:
          debug = !debug;
          break;
        case 'A':
        case VK_LEFT:
          playerX -= 1;
          break;
        case 'D':
        case VK_RIGHT:
          playerX += 1;
          break;
      }
    }
  }
}

void update(int delta) {
  cameraX = playerX;
  cameraY = playerY;
}

void render(int delta) {
  BeginBatchDraw();
  for (int y = 0; y < WINDOW_HEIGHT; y++) {
    setlinecolor(HSVtoRGB(195, 0.23f - 0.23f * y / WINDOW_HEIGHT, 1));
    line(0, y, WINDOW_WIDTH, y);
  }
  for (int x = playerX - WINDOW_WIDTH / 16; x < playerX + WINDOW_WIDTH / 16;
       x++) {
    for (int y = max(0, (int)playerY - WINDOW_HEIGHT / 16);
         y < min(128, (int)playerY + WINDOW_HEIGHT / 16); y++) {
      if (char id = world(x, y)) {
        putimage(WINDOW_WIDTH / 2 + x * 16 - cameraX * 16,
                 WINDOW_HEIGHT / 2 - y * 16 - cameraY * 16, &textures[id]);
      }
    }
  }
  if (debug) {
    debugText = L"FPS: " + to_wstring(1000 / delta) + L"POS: " +
                to_wstring(playerX) + L", " + to_wstring(playerY);
    outtextxy(0, 0, debugText.c_str());
  }
  EndBatchDraw();
}

int main() {
  init();
  while (running) {
    input();
    ULONGLONG now = GetTickCount();
    int delta = now - last;
    if (delta * FPS_MAX < 1000) continue;
    last = now;
    update(delta);
    render(delta);
  }
  closegraph();
  return 0;
}