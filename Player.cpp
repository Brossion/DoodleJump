#include "Player.h"

Player::Player(Sprite* sprite, int x, int y, int jumpSpeed)
    : sprite(sprite), x(x), y(y), jumpSpeed(jumpSpeed), moveSpeed(0), turnedRight(false) {
    getSpriteSize(sprite, width, height);
}

void Player::move(int speed) {
    moveSpeed = speed;
    if (speed < 0) {
        turnedRight = false;
        destroySprite(sprite);
        sprite = createSprite("data\\blue-lik-left.png");
    }
    else if (speed > 0) {
        turnedRight = true;
        destroySprite(sprite);
        sprite = createSprite("data\\blue-lik-right.png");
    }
}

void Player::stop() {
    moveSpeed = 0;
}
