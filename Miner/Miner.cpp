#include <graphics.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "Renderer.cpp"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define FPS_MAX 100

using namespace std;

HWND hwnd;
DWORD* buf;
ExMessage msg;
ULONGLONG tick;

bool running = true;

void init() {
  srand(time(NULL));
  tick = GetTickCount();

  hwnd = initgraph(WINDOW_WIDTH, WINDOW_HEIGHT);
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

void update(int last) {

}

void render(int last) {
  for (int x = 0; x < WINDOW_WIDTH; x++) {
    for (int y = 0; y < WINDOW_HEIGHT; y++) {
      buf[y * WINDOW_WIDTH + x] = x*tick % 0xffffff;
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