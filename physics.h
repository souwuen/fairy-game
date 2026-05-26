#pragma once
#include "player.h"
#include "map.h"

class physics
{
public:
    float dx = 0;
    float dy = 0;

    float gravity = 1200;     // гравитация (пиксели/сек²)
    float jumpSpeed = -300;   // сила прыжка

    bool onGround = false;

    void update(Player& p, float time, Map& map)
    {
        // гравитация
        dy += gravity * time;

        // ДВИЖЕНИЕ ПО X
        p.x += dx * time;

        // ДВИЖЕНИЕ ПО Y
        p.y += dy * time;

        float w = p.sprite.getGlobalBounds().width;
        float h = p.sprite.getGlobalBounds().height;

        // проверка "под ногами"
        int tileX = (p.x + w / 2) / Map::TILE_SIZE;
        int tileY = (p.y + h) / Map::TILE_SIZE;

        // защита от выхода за массив
        if (tileY >= Map::HEIGHT) tileY = Map::HEIGHT - 1;
        if (tileX >= Map::WIDTH) tileX = Map::WIDTH - 1;
        if (tileX < 0) tileX = 0;

        if (map.TileMap[tileY][tileX] == 1 || map.TileMap[tileY][tileX] == 2)
        {
            p.y = tileY * Map::TILE_SIZE - h;
            dy = 0;
            onGround = true;
        }
        else
        {
            onGround = false;
        }

        p.sprite.setPosition(p.x, p.y);

        dx = 0; // сброс X каждый кадр
    }

    void moveLeft()  { dx = -150; }
    void moveRight() { dx =  150; }

    void jump()
    {
        if (onGround)
        {
            dy = jumpSpeed;
            onGround = false;
        }
    }
};