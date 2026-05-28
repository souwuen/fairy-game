#pragma once
#include "player.h"
#include "map.h"

class physics
{
public:
    float dx = 0;
    float dy = 0;

    float gravity = 1000;     // гравитация (пиксели/сек²)
    float jumpSpeed = -450;   // сила прыжка

    bool onGround = false;

    void update(Player& p, float time, Map& map)
    {
        // гравитация
        dy += gravity * time;
        float w = p.sprite.getGlobalBounds().width * 0.6f;   // 60% ширины
        float h = p.sprite.getGlobalBounds().height * 0.7f;  // 70% высоты

        // Отступы чтобы hitbox был по центру спрайта
        float offsetX = (p.sprite.getGlobalBounds().width - w) / 2;
        float offsetY = (p.sprite.getGlobalBounds().height - h) / 2;
        // ========== ДВИЖЕНИЕ ПО Y (гравитация) ==========
        p.y += dy * time;

        // Проверяем 3 точки под ногами (левая, центр, правая)
        int tileLeftFoot = (int)(p.x + offsetX + 2) / Map::TILE_SIZE;
        int tileCenterFoot = (int)(p.x + offsetX + w / 2) / Map::TILE_SIZE;
        int tileRightFoot = (int)(p.x + offsetX + w - 2) / Map::TILE_SIZE;
        int tileFeet = (int)(p.y + offsetY + h) / Map::TILE_SIZE;

        onGround = false;

        // Проверка падения вниз
        if (dy >= 0 && tileFeet >= 0 && tileFeet < Map::HEIGHT)
        {
            // Проверяем все 3 точки
            for (int checkX = tileLeftFoot; checkX <= tileRightFoot; checkX++)
            {
                if (checkX >= 0 && checkX < Map::WIDTH)
                {
                    if (map.TileMap[tileFeet][checkX] == 1 ||
                        map.TileMap[tileFeet][checkX] == 2 ||
                        map.TileMap[tileFeet][checkX] == 3)  // ← платформа тоже твёрдая
                    {
                        // Приземляемся РОВНО на блок
                        p.y = tileFeet * Map::TILE_SIZE - h - offsetY;
                        dy = 0;
                        onGround = true;
                        break;
                    }
                }
            }
        }

        // ========== ДВИЖЕНИЕ ПО X ==========
        p.x += dx * time;

        // Проверяем столкновение слева/справа
        int tileTop = (int)(p.y + offsetY + 5) / Map::TILE_SIZE;
        int tileBottom = (int)(p.y + offsetY + h - 5) / Map::TILE_SIZE;

        // Проверка левой стороны
        if (dx < 0)
        {
            int tileLeft = (int)(p.x + offsetX) / Map::TILE_SIZE;
            for (int checkY = tileTop; checkY <= tileBottom; checkY++)
            {
                if (checkY >= 0 && checkY < Map::HEIGHT &&
                    tileLeft >= 0 && tileLeft < Map::WIDTH)
                {
                    if (map.TileMap[checkY][tileLeft] == 1 ||
                        map.TileMap[checkY][tileLeft] == 2 ||
                        map.TileMap[checkY][tileLeft] == 3)  // ← платформа тоже твёрдая
                    {
                        p.x = (tileLeft + 1) * Map::TILE_SIZE - offsetX;
                        dx = 0;
                        break;
                    }
                }
            }
        }

        // Проверка правой стороны
        if (dx > 0)
        {
            int tileRight = (int)(p.x + offsetX + w) / Map::TILE_SIZE;
            for (int checkY = tileTop; checkY <= tileBottom; checkY++)
            {
                if (checkY >= 0 && checkY < Map::HEIGHT &&
                    tileRight >= 0 && tileRight < Map::WIDTH)
                {
                    if (map.TileMap[checkY][tileRight] == 1 ||
                        map.TileMap[checkY][tileRight] == 2 ||
                        map.TileMap[checkY][tileRight] == 3)  // ← платформа тоже твёрдая
                    {
                        p.x = tileRight * Map::TILE_SIZE - w - offsetX;
                        dx = 0;
                        break;
                    }
                }
            }
        }

        // Проверка удара головой в потолок
        if (dy < 0)
        {
            int tileHead = (int)(p.y + offsetY) / Map::TILE_SIZE;
            if (tileHead >= 0 && tileHead < Map::HEIGHT &&
                tileCenterFoot >= 0 && tileCenterFoot < Map::WIDTH)
            {
                if (map.TileMap[tileHead][tileCenterFoot] == 1 ||
                    map.TileMap[tileHead][tileCenterFoot] == 2 ||
                    map.TileMap[tileHead][tileCenterFoot] == 3)  // ← платформа тоже твёрдая
                {
                    p.y = (tileHead + 1) * Map::TILE_SIZE - offsetY;
                    dy = 0;
                }
            }
        }

        // Ограничение по краям экрана
        if (p.x < 0) p.x = 0;
        if (p.x + w > 640) p.x = 640 - w;
        if (p.y < 0) { p.y = 0; dy = 0; }
        if (p.y + h > 480) { p.y = 480 - h; dy = 0; onGround = true; }

        p.sprite.setPosition(p.x, p.y);

        dx = 0; // сброс X каждый кадр
    }

    void moveLeft() { dx = -150; }
    void moveRight() { dx = 150; }

    void jump()
    {
        if (onGround)
        {
            dy = jumpSpeed;
            onGround = false;
        }
    }
};