#include <graphics.h>

#include <filesystem>
#include <map>
#include <string>

#include "World.h"

#define FPS_MAX 100
#define ID_MAX 128
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

using namespace std;

HWND hwnd;
ExMessage msg;
ULONGLONG last = GetTickCount();

map<string, int> ID = {{"grass_block", 0}, {"dirt", 1}, {"stone_block", 2}};
IMAGE textures[ID_MAX];

bool running = true;
bool debug = true;
wstring debugText;

void init() {
  hwnd = initgraph(WINDOW_WIDTH, WINDOW_HEIGHT);
  for (auto p : ID) {
    wstring path =
        L"assets/" + wstring(p.first.begin(), p.first.end()) + L".png";
    loadimage(&textures[p.second], path.c_str());
  }
}

void input() {
  while (peekmessage(&msg)) {
    if (msg.message == WM_KEYDOWN || msg.message == WM_KEYUP) {
      switch (msg.vkcode) {
        case VK_ESCAPE:
          running = false;
          break;
      }
    }
  }
}

void update(int delta) {}

void render(int delta) {
  BeginBatchDraw();
  cleardevice();
  putimage(10, 10, &textures[ID["grass_block"]]);
  if (debug) {
    debugText = L"FPS: " + to_wstring(1000 / delta);
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
    update(delta);
    render(delta);
  }
  closegraph();
  return 0;
}