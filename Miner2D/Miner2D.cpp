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

#define G 9.8
#define PI 3.1415926
#define MOVE_SPEED 5
#define JUMP_ACCEL 10

using namespace std;

HWND hwnd;
DWORD* surface;
ExMessage msg;
ULONGLONG last = GetTickCount();
World world;
bool running = true;
bool debug = true;
wstring debugText;

int mouseX, mouseY;
bool moveLeft, moveRight, jump;
float playerX, playerY, playerdX, playerdY;

IMAGE textures[ID_MAX];

void init() {
  hwnd = initgraph(WINDOW_WIDTH, WINDOW_HEIGHT);
  surface = GetImageBuffer();
  for (auto p : ID) {
    wstring path =
        L"assets/" + wstring(p.first.begin(), p.first.end()) + L".png";
    loadimage(&textures[p.second], path.c_str());
  }

  moveLeft = moveRight = false;
  playerX = playerdX = playerdY = 0;
  playerY = 10;
}

void input() {
  while (peekmessage(&msg)) {
    if (msg.message == WM_KEYDOWN || msg.message == WM_KEYUP) {
      bool keydown = msg.message == WM_KEYDOWN;
      switch (msg.vkcode) {
        case VK_ESCAPE:
          running = false;
          break;
        case VK_TAB:
          if (keydown) debug = !debug;
          break;
        case 'A':
        case VK_LEFT:
          moveLeft = keydown;
          break;
        case 'D':
        case VK_RIGHT:
          moveRight = keydown;
          break;
        case 'W':
        case VK_UP:
        case VK_SPACE:
          jump = keydown;
          break;
      }
    }
    if (msg.message == WM_MOUSEMOVE) {
      mouseX = msg.x;
      mouseY = msg.y;
    }
  }
}

void update(int delta) {
  playerdX = (moveRight - moveLeft) * MOVE_SPEED;
  if (jump) playerdY += 10;
  playerdY -= G * delta / 1000;

  float playerLeft = playerX;
  float playerRight = playerX + 16;
  float playerTop = playerY;
  float playerBottom = playerY + 16;

  for (int y = floor(playerTop); y < ceil(playerBottom); y++) {
    if (world(floor(playerLeft), y) || world(ceil(playerRight), y)) {
      playerdX = 0;
    }
  }

  for (int x = floor(playerLeft); x < ceil(playerRight); x++) {
    if (world(x, floor(playerTop)) || world(x, ceil(playerBottom))) {
      playerdY = 0;
    }
  }

  playerX += playerdX * delta / 1000;
  playerY += playerdY * delta / 1000;
}

void render(int delta) {
  BeginBatchDraw();
  for (int y = 0; y < WINDOW_HEIGHT; y++) {
    setlinecolor(HSVtoRGB(195, 0.23f - 0.23f * y / WINDOW_HEIGHT, 1));
    line(0, y, WINDOW_WIDTH, y);
  }
  int boundLeft = playerX - WINDOW_WIDTH / 16;
  int boundRight = playerX + WINDOW_WIDTH / 16;
  int boundTop = max(0, (int)playerY - WINDOW_HEIGHT / 16);
  int boundBottom = min(128, (int)playerY + WINDOW_HEIGHT / 16);
  int cameraX = (mouseX - WINDOW_WIDTH / 2) / 4;
  int cameraY = (mouseY - WINDOW_HEIGHT / 2) / 4;
  int offsetX = WINDOW_WIDTH / 2 - playerX * 16 - cameraX;
  int offsetY = WINDOW_HEIGHT / 2 + playerY * 16 - cameraY;

  for (int x = boundLeft; x < boundRight; x++) {
    for (int y = boundTop; y < boundBottom; y++) {
      if (char id = world(x, y)) {
        putimage(offsetX + x * 16, offsetY - y * 16, &textures[id]);
      }
    }
  }
  setfillcolor(RED);
  solidrectangle(offsetX + playerX * 16, offsetY - playerY * 16,
                 offsetX + playerX * 16 + 16, offsetY - playerY * 16 + 16);
  if (debug) {
    debugText = L"FPS: " + to_wstring(1000 / delta) + L"  POS: " +
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