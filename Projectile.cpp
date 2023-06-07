#include "Projectile.h"

Projectile::Projectile(Sprite* sprite, int x, int y, int speed)
    : sprite(sprite), x(x), y(y), speed(speed), active(false), directionX(0), directionY(0)
{
    getSpriteSize(sprite, width, height);
}

void Projectile::draw(int playerY) {
    if (active) {
        drawSprite(sprite, x, y - playerY);
    }
}

void Projectile::update(int cameraY, int screen_height) {
    if (active) {
         x += directionX;
         y += directionY;

         if (y < cameraY || y > cameraY + screen_height) {
             active = false;
             x = -50000;
             y = -50000;
         }
    }
}
