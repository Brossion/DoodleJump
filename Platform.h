#pragma once
#include "Framework.h"

class Platform {
public:
    Sprite* sprite;
    int x, y;
    int width, height;
    int platformCount = 2;
    int platformMargin = 40;
    bool hasEnemy;

    Platform(Sprite* sprite, int x, int y);
    void draw(int playerY);
};
