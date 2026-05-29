#pragma once
#include <SFML/Graphics.hpp>

using namespace sf;

class Log
{
public:
    Sprite sprite;

    float x, y;
    float startY;
    bool moving;
    bool goDown = true;
    float speed = 65.f;

    Log(Texture& texture,
        float startX,
        float startYPos,
        bool isMoving = false)
    {
        sprite.setTexture(texture);

        sprite.setScale(0.3, 0.3f);

        x = startX;
        y = startYPos;
        startY = startYPos;

        moving = isMoving;

        sprite.setPosition(x, y);
    }

    void update(float time)
    {
        if (!moving)
            return;

        if (goDown)
            y += speed * time;
        else
            y -= speed * time;

        // диапазон движения
        if (y > startY + 60)
            goDown = false;

        if (y < startY - 40)
            goDown = true;

        sprite.setPosition(x, y);
    }

    void draw(RenderWindow& window)
    {
        window.draw(sprite);
    }

    FloatRect getBounds()
    {
        return sprite.getGlobalBounds();
    }
};