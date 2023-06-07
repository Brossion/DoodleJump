#pragma once
#include "Framework.h"

class Projectile {
public:
    Sprite* sprite;
    int x, y;
    int width, height;
    float directionX, directionY;
    int speed;
    bool active;

    Projectile(Sprite* sprite, int x, int y, int speed);
    void draw(int playerY);
    void update(int cameraY, int screen_height);
};
