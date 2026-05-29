#pragma once
#include <SFML/Graphics.hpp>
#include "map.h"

struct PushableBox {
    sf::Sprite sprite;

    float x, y;
    float velocityY = 0.f;

    static constexpr float SCALE = 0.4f;
    static constexpr float MOVE_SPEED = 80.f;
    static constexpr float GRAVITY = 800.f;

    static constexpr float SCREEN_WIDTH = 640.f;
    static constexpr float SCREEN_HEIGHT = 480.f;

    PushableBox(sf::Texture& tex, float startX, float startY)
        : x(startX), y(startY)
    {
        sprite.setTexture(tex);
        sprite.setScale(SCALE, SCALE);
        sprite.setPosition(x, y);
    }

private:
    bool isInsideMap(int tileX, int tileY) const {
        return tileX >= 0 && tileX < Map::WIDTH &&
            tileY >= 0 && tileY < Map::HEIGHT;
    }

    bool isSolidTile(Map& map, int tileX, int tileY) const {
        return isInsideMap(tileX, tileY) &&
            map.TileMap[tileY][tileX] != 0;
    }

public:
    void push(float direction, float deltaTime, Map& map) {
        const float width = sprite.getGlobalBounds().width;
        float newX = x + direction * MOVE_SPEED * deltaTime;

        int checkX = static_cast<int>(
            newX + (direction > 0 ? width - 1 : 0)
            ) / Map::TILE_SIZE;

        int checkY = static_cast<int>(
            y + sprite.getGlobalBounds().height / 2
            ) / Map::TILE_SIZE;

        if (!isSolidTile(map, checkX, checkY)) {
            x = newX;

            // Ограничение экрана
            x = std::clamp(x, 0.f, SCREEN_WIDTH - width);
        }

        sprite.setPosition(x, y);
    }

    void applyGravity(float deltaTime, Map& map) {
        const auto bounds = sprite.getGlobalBounds();
        const float width = bounds.width;
        const float height = bounds.height;

        int tileX = static_cast<int>(x + width / 2) / Map::TILE_SIZE;
        int groundTileY = static_cast<int>(
            y + height + 2
            ) / Map::TILE_SIZE;

        // Проверяем, стоим ли на земле
        bool grounded = isSolidTile(map, tileX, groundTileY);

        if (grounded && std::abs(velocityY) < 5.f) {
            y = groundTileY * Map::TILE_SIZE - height;
            velocityY = 0.f;
            sprite.setPosition(x, y);
            return;
        }

        // Гравитация
        velocityY += GRAVITY * deltaTime;
        float newY = y + velocityY * deltaTime;

        int futureTileY = static_cast<int>(
            newY + height
            ) / Map::TILE_SIZE;

        if (isSolidTile(map, tileX, futureTileY)) {
            y = futureTileY * Map::TILE_SIZE - height;
            velocityY = 0.f;
        }
        else {
            y = newY;
        }

        // Ограничение экрана
        if (y > SCREEN_HEIGHT - height) {
            y = SCREEN_HEIGHT - height;
            velocityY = 0.f;
        }

        sprite.setPosition(x, y);
    }

    bool checkPlayerCollision(
        float px, float py,
        float pw, float ph,
        float& direction)
    {
        const auto bounds = sprite.getGlobalBounds();

        float pRight = px + pw;
        float pBottom = py + ph;

        float bRight = x + bounds.width;
        float bBottom = y + bounds.height;

        // AABB collision
        bool intersect =
            pRight > x &&
            px < bRight &&
            pBottom > y &&
            py < bBottom;

        if (!intersect)
            return false;

        // Игрок сверху коробки
        if (pBottom - y < 10.f && pBottom > y)
            return false;

        // Определяем сторону толчка
        direction =
            (px + pw * 0.5f < x + bounds.width * 0.5f)
            ? 1.f
            : -1.f;

        return true;
    }

    void draw(sf::RenderWindow& window) {
        window.draw(sprite);
    }
};