#pragma once
#include <SFML/Graphics.hpp>
#include <cmath>

using namespace sf;

class Coin
{
public:
    Sprite sprite;

    float x, y;
    float frame = 0;
    float animTime = 0;

    static const int FRAME_COUNT = 6;
    bool collected = false;

    // ❗ текстура передаётся снаружи
    Coin(Texture& texture, float X, float Y)
    {
        sprite.setTexture(texture);

        int frameW = texture.getSize().x / FRAME_COUNT;
        int frameH = texture.getSize().y;

        sprite.setTextureRect(IntRect(0, 0, frameW, frameH));

        float scale = 24.0f / frameW;
        sprite.setScale(scale, scale);

        x = X;
        y = Y;
    }

    void update(float time, Texture& texture)
    {
        animTime += time;

        frame += 6 * time;
        if (frame >= FRAME_COUNT)
            frame = 0;

        int frameW = texture.getSize().x / FRAME_COUNT;
        int frameH = texture.getSize().y;

        sprite.setTextureRect(
            IntRect(frameW * (int)frame, 0, frameW, frameH)
        );

        sprite.setPosition(x, y + sin(animTime * 3) * 3);
    }
};