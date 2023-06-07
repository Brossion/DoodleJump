#include "Enemy.h"

Enemy::Enemy(Sprite* sprite, int x, int y)
    : sprite(sprite), x(x), y(y)
{
    getSpriteSize(sprite, width, height);
}

void Enemy::draw(int playerY) {
    drawSprite(sprite, x, y - playerY);
}

void Enemy::update(int playerY, int gravity, int timeStep) {
    if (isHit) {
        fallSpeed += gravity * timeStep;
        y += fallSpeed;
    }
}