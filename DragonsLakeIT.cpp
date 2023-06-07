#include "Framework.h"
#include "Player.h"
#include "Platform.h"
#include "Enemy.h"
#include "Projectile.h"
#include <iostream>
#include <list>
#include <random>
#include <algorithm>

#define M_PI 3.14159265358979323846

using namespace std;

class MyFramework : public Framework {
public:
    int width, height;

    // player
    Player* player;

    const int gravity = 2;

    // time frame
    const float timeStep = 1.0f / 60.0f;
    float accumulator = 0.0f;
    int previousTick = 0;

    // camera
    float cameraY;

    // platforms
    list<Platform*> platforms;
    Sprite* platformSprite;
    int platformCounter = 0;
    int platformMargin = 40;

    // enemies
    list<Enemy*> enemies;
    Sprite* enemySprite;
    int enemySpawnInterval = 50; 


    // projectiles
    Projectile* projectile;
    Sprite* projectileSprite;
    const int projectileSpeed = 7;

    // mouse
    int mouseX, mouseY;

    // score
    int score = 0;
    float platformsMoved = 0.0f;

    virtual void PreInit(int& width, int& height, bool& fullScreen)
    {
        if (__argc >= 3) {
            width = atoi(__argv[1]);
            height = atoi(__argv[2]);
        }
        else {
            width = 800;
            height = 600;
        }
        fullScreen = false;

        this->width = width;
        this->height = height;
    }

    virtual bool Init() {
        Sprite* playerSprite = createSprite("data\\blue-lik-left.png");
        if (!playerSprite) {
            cout << "Failed to load player sprite." << endl;
            return false;
        }
        player = new Player(playerSprite, width / 2, height, -400);

        enemySprite = createSprite("data\\enemy.png");
        if (!enemySprite) {
            cout << "Failed to load enemy sprite." << endl;
            return false;
        }

        platformSprite = createSprite("data\\platform.png");
        if (!platformSprite) {
            cout << "Failed to load platform sprite." << endl;
            return false;
        }
        
        projectileSprite = createSprite("data\\projectile.png");
        if (!projectileSprite) {
            cout << "Failed to load projectile sprite." << endl;
            return false;
        }

        projectile = new Projectile(projectileSprite, -50000, -50000, projectileSpeed);
        previousTick = getTickCount();

        startingPlatforms();

        return true;
    }

    virtual void Close() {
        destroySprite(player->sprite);
        delete player;

        for (auto& platform : platforms) {
            destroySprite(platform->sprite);
            delete platform;
        }
        platforms.clear();

        for (auto& enemy : enemies) {
            destroySprite(enemy->sprite);
            delete enemy;
        }
        enemies.clear();

        destroySprite(projectileSprite);
        delete projectile;
    }

    virtual bool Tick() {
        int currentTick = getTickCount();
        float frameTime = (currentTick - previousTick) / 1000.0f;
        previousTick = currentTick;

        accumulator += frameTime;

        while (accumulator >= timeStep) {
            player->y += player->jumpSpeed * timeStep;
            player->jumpSpeed += gravity * timeStep;
            accumulator -= timeStep;
        }

        player->x += player->moveSpeed;
        player->jumpSpeed += gravity;

        if (player->x < 0)
            player->x = width - player->width;
        else if (player->x > width - player->width)
            player->x = 0;

        if (projectile->x < 0)
            projectile->x = width - projectile->width;
        else if (projectile->x > width - projectile->width)
            projectile->x = 0;

        if (player->y > height) {
            cout << "Game Over. Your score was: " << score << "\nYou passed " << platformsMoved << " platforms!\n";

            player->x = (width - player->width) / 2;
            player->y = height - player->height;
            player->jumpSpeed = -400;
            cameraY = 0;
            platforms.clear();
            startingPlatforms();
            enemies.clear();

            platformsMoved = 0.0f;
            score = 0;
        }

        updateCamera();

        showCursor(true);

        handleCollisions();

        handleProjectiles();

        drawTestBackground();

        drawSprite(player->sprite, player->x, player->y);

        if (projectile->active) {
            destroySprite(player->sprite);
            player->sprite = createSprite("data\\blue-lik-shooting.png");
        }     

        for (auto& platform : platforms) {
            platform->draw(cameraY);
        }

        for (auto& enemy : enemies) {
            if (enemy->isHit) {
                enemy->update(cameraY, gravity, timeStep);
            }
            enemy->draw(cameraY);
        }

        projectile->draw(cameraY);

        return false;
    }

    virtual void onMouseMove(int x, int y, int xRelative, int yRelative) {
        mouseX = x;
        mouseY = y;
    }

    virtual void onMouseButtonClick(FRMouseButton button, bool isReleased) {
        if (button == FRMouseButton::LEFT && !isReleased) {
            if (!projectile->active) {
                int dx = mouseX - (player->x + player->width / 2);
                int dy = mouseY - (player->y + player->height / 2);
                float directionMagnitude = sqrt(dx * dx + dy * dy);

                if (directionMagnitude != 0) {
                    float angle = atan2(dy, dx) * (180.0f / M_PI);

                    if (angle < 0) {
                        angle += 360;
                    }

                    if (angle >= 90 && angle < 150) {
                        angle = 360 - angle;
                    }
                    else if (angle >= 150 && angle < 210)
                    {
                        angle = 210;
                    }
                    else if ((angle >= 330 && angle < 360) || (angle >= 0 && angle < 30)) {
                        angle = 330;
                    }
                    else if (angle >= 30 && angle < 90)
                    {
                        angle = 360 - angle;
                    }

                    angle = angle * M_PI / 180.0f;

                    projectile->directionX = cos(angle) * projectileSpeed;
                    projectile->directionY = sin(angle) * projectileSpeed;

                    projectile->x = player->x + (player->width - projectile->width) / 2;
                    projectile->y = player->y;
                    projectile->active = true;
                }
            }
        }
    }


    virtual void onKeyPressed(FRKey k) {
        switch (k) {
        case FRKey::LEFT:
            player->move(-1);
            player->turnedRight = false;
            destroySprite(player->sprite);
            player->sprite = createSprite("data\\blue-lik-left.png");
            break;
        case FRKey::RIGHT:
            player->move(1);
            player->turnedRight = true;
            destroySprite(player->sprite);
            player->sprite = createSprite("data\\blue-lik-right.png");
            break;
        default:
            break;
        }
    }

    virtual void onKeyReleased(FRKey k) {
        switch (k) {
        case FRKey::LEFT:
        case FRKey::RIGHT:
            player->stop();
            break;
        default:
            break;
        }
    }

    virtual const char* GetTitle() override {
        return "Doodle Jump";
    }

    void updateCamera() {
        if (player->y <= height / 2) {
            float deltaY = height / 2 - player->y;
            player->y = height / 2;
            const float platformMarginX = 100.0f;  
            const float platformMarginY = 20.0f;   

            for (auto it = platforms.begin(); it != platforms.end(); ) {
                Platform* platform = *it;
                platform->y += deltaY;

                if (platform->y > height) {
                    platform->y -= height;

                    platformsMoved += abs(deltaY);
                    score = static_cast<int>(platformsMoved) * 8;

                    int maxAttempts = 10;
                    bool foundNonColliding = false;

                    for (int attempt = 0; attempt < maxAttempts; ++attempt) {
                        float randomFloat = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

                        float x = (13 * width / 20 - 6 * width / 20) * randomFloat + 6 * width / 20;

                        bool collides = any_of(platforms.begin(), platforms.end(), [&](Platform* p) {
                            return abs(p->x - x) < platformMarginX && abs(p->y - platform->y) < platformMarginY;
                            });
                        if (!collides) {
                            platform->x = x;
                            foundNonColliding = true;
                            break;
                        }
                    }

                    if (foundNonColliding) {
                        ++it;
                    }
                    else {
                        delete platform;
                        it = platforms.erase(it);
                    }

                    if (platformCounter % enemySpawnInterval == 0) {
                        Sprite* newEnemySprite = createSprite("data\\enemy.png");
                        Enemy* enemy = new Enemy(newEnemySprite, platform->x + 15, platform->y - 43);
                        enemies.push_back(enemy);
                    }
                    platformCounter++;
                }
                else {
                    ++it;
                }
            }
            for (auto it = enemies.begin(); it != enemies.end(); ++it) {
                Enemy* enemy = *it;
                enemy->y += deltaY;
            }
        }
    }

    void startingPlatforms() {
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> xDistance(6 * width / 20, 13 * width / 20);
        uniform_int_distribution<> yDistance(0, height - platformMargin);

        int highest_platform_y = height;

        for (int i = 0; i < 3333; ++i) {
            int x = xDistance(gen);
            int y = yDistance(gen);

            bool collides = any_of(platforms.begin(), platforms.end(), [&](Platform* p) {
                return abs(p->x - x) < 100 && abs(p->y - y) < 50;
                });

            if (!collides) {
                platforms.push_back(new Platform(platformSprite, x, y));
                highest_platform_y = min(highest_platform_y, y);

                if (platformCounter % enemySpawnInterval == 0) {
                    Enemy* enemy = new Enemy(enemySprite, x + 15, y - 43);
                    enemies.push_back(enemy);
                }
                platformCounter++;
            }
        }
    }

    void handleCollisions() {
        static bool collided = false;
        static float collisionTimer = 0.0f;
        const float collisionDuration = 1;

        for (auto& platform : platforms) {
            int platformX, platformY, platformWidth, platformHeight;
            platformX = platform->x;
            platformY = platform->y - cameraY;
            platformWidth = platform->width / 2;
            platformHeight = platform->height / 3;

            if (player->x < platformX + platformWidth &&
                player->x + player->width / 2 > platformX &&
                player->y + player->height > platformY &&
                player->y + player->height < platformY + platformHeight &&
                player->jumpSpeed > 0)
            {
                player->jumpSpeed = -300;
                collided = true;
                collisionTimer = collisionDuration;

                if (!player->turnedRight) {
                    destroySprite(player->sprite);
                    player->sprite = createSprite("data\\blue-lik-left-odskok.png");
                }
                else {
                    destroySprite(player->sprite);
                    player->sprite = createSprite("data\\blue-lik-right-odskok.png");
                }
            }
        }
        auto enemy_it = enemies.begin();
        while (enemy_it != enemies.end()) {
            Enemy* enemy = *enemy_it;
            if (player->x < enemy->x + enemy->width &&
                player->x + player->width > enemy->x &&
                player->y + player->height > enemy->y &&
                player->y < enemy->y + enemy->height)
            {
                // check if player jumped on the enemy
                if (player->jumpSpeed > 0 && player->y <= enemy->y - 20) {
                    // kill the enemy
                    destroySprite(enemy->sprite);
                    delete enemy;
                    enemy_it = enemies.erase(enemy_it);
                    player->jumpSpeed = -300;
                    continue;
                }
                else {
                    // Game Over
                    player->y = height + 1;
                }
            }
            ++enemy_it;
        }

        if (collided) {
            collisionTimer -= timeStep;

            if (collisionTimer <= 0.0f) {
                collided = false;

                if (player->turnedRight) {
                    destroySprite(player->sprite);
                    player->sprite = createSprite("data\\blue-lik-right.png");
                }
                else {
                    destroySprite(player->sprite);
                    player->sprite = createSprite("data\\blue-lik-left.png");
                }
            }
        }
    }

    void handleProjectiles() {
        projectile->update(cameraY, height);

        auto enemy_it = enemies.begin();
        while (enemy_it != enemies.end()) {
            Enemy* enemy = *enemy_it;
            if (projectile->active &&
                projectile->x < enemy->x + enemy->width &&
                projectile->x + projectile->width > enemy->x &&
                projectile->y < enemy->y + enemy->height &&
                projectile->y + projectile->height > enemy->y)
            {
                // enemy is hit by projectile
                destroySprite(enemy->sprite);
                delete enemy;
                enemy_it = enemies.erase(enemy_it);
                projectile->active = false;

                projectile->x = -50000;
                projectile->y = -50000;

                continue;
            }

            ++enemy_it;
        }
    }
};

int main(int argc, char* argv[]) {
    return run(new MyFramework);
}