#pragma once
#include "Framework.h"

class Enemy {
public:
    Sprite* sprite;
    int x, y;
    int width, height;
    float fallSpeed = 0;
    bool isHit = false;

    Enemy(Sprite* sprite, int x, int y);
    void draw(int playerY);
    void update(int playerY, int gravity, int timeStep);
};
