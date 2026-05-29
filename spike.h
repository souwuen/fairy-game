#pragma once
#include <SFML/Graphics.hpp>

struct Spike {
    sf::Sprite sprite;

    Spike(sf::Texture& tex, float x, float y)
    {
        sprite.setTexture(tex);

        float scale = 0.12f;
        sprite.setScale(scale, scale);

        // 🔥 СТАВИМ ТОЧКУ ПРИВЯЗКИ В НИЗ СПРАЙТА
        if (y < 400) {
            y = 410;  // заставляем быть ниже
        }

        sprite.setPosition(x, y);
    }


    bool checkCollision(float px, float py, float pw, float ph)
    {
        sf::FloatRect player(px, py, pw, ph);
        sf::FloatRect spike = sprite.getGlobalBounds();

        spike.top += spike.height * 0.5f;
        spike.height *= 0.5f;

        return spike.intersects(player);
    }

    void draw(sf::RenderWindow& w)
    {
        w.draw(sprite);
    }
};