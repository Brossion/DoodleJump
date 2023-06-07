#include "Platform.h"

Platform::Platform(Sprite* sprite, int x, int y)
    : sprite(sprite), x(x), y(y)
{
    getSpriteSize(sprite, width, height);
}

void Platform::draw(int playerY) {
    drawSprite(sprite, x, y - playerY);
}
