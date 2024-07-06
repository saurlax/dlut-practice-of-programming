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

#define EPS 1e-3
#define G 9.8
#define PI 3.1415926
#define MOVE_SPEED 10
#define JUMP_ACCEL 10
#define PLAYER_WIDTH 12
#define PLAYER_HEIGHT 12

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
float cameraX, cameraY;
int selectX, selectY;
bool moveLeft, moveRight, jump, onair;
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

  onair = true;
  moveLeft = moveRight = false;
  playerX = playerdX = playerdY = 0;
  playerY = 20;
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
  if (!IsWindow(hwnd)) running = false;
  cameraX = (mouseX - WINDOW_WIDTH / 2.0f) / 4;
  cameraY = (mouseY - WINDOW_HEIGHT / 2.0f) / 4;
  selectX = playerX + floor(cameraX * 5 / 16);
  selectY = playerY + floor(cameraY * 5 / 16);
  printf("%d %d\n", selectX, selectY);

  playerdX = (moveRight - moveLeft) * MOVE_SPEED;
  playerdY += G * delta / 1000;
  if (jump && !onair) playerdY = -JUMP_ACCEL;
  onair = true;

  float playerLeft = playerX;
  float playerRight = playerX + PLAYER_WIDTH / 16.0f;
  float playerTop = playerY;
  float playerBottom = playerY + PLAYER_HEIGHT / 16.0f;

  float deltaX = playerdX * delta / 1000;
  float deltaY = playerdY * delta / 1000;

  bool blockLeft = world.safeRead(floor(playerLeft - 1), floor(playerTop)) ||
                   world.safeRead(floor(playerLeft - 1), floor(playerBottom));
  bool blockRight = world.safeRead(ceil(playerRight), floor(playerTop)) ||
                    world.safeRead(ceil(playerRight), floor(playerBottom));
  bool blockTop = world.safeRead(floor(playerLeft), floor(playerTop - 1)) ||
                  world.safeRead(floor(playerRight), floor(playerTop - 1));
  bool blockBottom = world.safeRead(floor(playerLeft), ceil(playerBottom)) ||
                     world.safeRead(floor(playerRight), ceil(playerBottom));

  bool throughLeft = playerLeft + deltaX <= floor(playerLeft);
  bool throughRight = playerRight + deltaX >= ceil(playerRight);
  bool throughTop = playerTop + deltaY <= floor(playerTop);
  bool throughBottom = playerBottom + deltaY >= ceil(playerBottom);

  if (blockLeft && throughLeft) {
    playerX = floor(playerLeft);
    playerdX = 0;
  } else if (blockRight && throughRight) {
    playerX = ceil(playerRight) - PLAYER_WIDTH / 16.0f - EPS;
    playerdX = 0;
  } else {
    playerX += deltaX;
  }
  if (blockTop && throughTop) {
    playerY = floor(playerTop);
    playerdY = 0;
  } else if (blockBottom && throughBottom) {
    playerY = ceil(playerBottom) - PLAYER_HEIGHT / 16.0f - EPS;
    playerdY = 0;
    onair = false;
  } else {
    playerY += deltaY;
  }
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
  int offsetX = WINDOW_WIDTH / 2 - playerX * 16 - cameraX;
  int offsetY = WINDOW_HEIGHT / 2 - playerY * 16 - cameraY;

  for (int x = boundLeft; x < boundRight; x++) {
    for (int y = boundTop; y < boundBottom; y++) {
      if (char id = world.safeRead(x, y)) {
        putimage(offsetX + x * 16, offsetY + y * 16, &textures[id]);
      }
    }
  }
  setfillcolor(RED);
  solidrectangle(offsetX + playerX * 16, offsetY + playerY * 16,
                 offsetX + playerX * 16 + PLAYER_WIDTH,
                 offsetY + playerY * 16 + PLAYER_HEIGHT);
  if (world.safeRead(selectX, selectY)) {
    setlinecolor(BLACK);
    rectangle(offsetX + selectX * 16, offsetY + selectY * 16,
              offsetX + selectX * 16 + 16, offsetY + selectY * 16 + 16);
  }
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