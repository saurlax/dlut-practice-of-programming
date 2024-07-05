#pragma once
#include <map>
using namespace std;

class World {
  map<int, char[16][128]> chunks;

 public:
  char& operator()(int x, int y);
};