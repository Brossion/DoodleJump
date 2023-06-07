#pragma once
#include "Framework.h"

class Player {
public:
    Sprite* sprite;
    int width, height;
    int x, y;
    int jumpSpeed;
    int moveSpeed;
    bool turnedRight;
    bool isControlled;

    Player(Sprite* sprite, int x, int y, int jumpSpeed);

    void move(int speed);
    void stop();
};
