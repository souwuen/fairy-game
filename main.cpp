#include <SFML/Graphics.hpp>
#include <algorithm>
#include "player.h"
#include "map.h"
#include "physics.h"
#include "coin.h"
#include <vector>

using namespace sf;
using namespace std;

int main()
{
    RenderWindow window(VideoMode(640, 480), "Fairy");

    float startX = 0; 
    float playerH = 144 * 0.3f; 
    float startY = (Map::HEIGHT - 2) * Map::TILE_SIZE - playerH;

    Player p("hero.png", startX, startY, 144, 144);
    
    Map map;
    physics phys;

    // текстура монет
    Texture coinTexture;
    coinTexture.loadFromFile("images/coin.png");

    vector<Coin> coins;
    coins.push_back(Coin(coinTexture, 200, 300));
    coins.push_back(Coin(coinTexture, 300, 250));
    coins.push_back(Coin(coinTexture, 400, 200));

    float CurrentFrame = 0;
    Clock clock;

    Texture bgTexture;
    bgTexture.loadFromFile("images/background.png");
    Sprite background(bgTexture);

    float scaleX = (float)window.getSize().x / bgTexture.getSize().x;
    float scaleY = (float)window.getSize().y / bgTexture.getSize().y;
    float scale = max(scaleX, scaleY);
    background.setScale(scale, scale);

    while (window.isOpen())
    {
        float time = clock.getElapsedTime().asSeconds(); 
        clock.restart();

        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
                window.close();
        }

        bool moving = false;

        // ВПРАВО
        if (Keyboard::isKeyPressed(Keyboard::Right) || Keyboard::isKeyPressed(Keyboard::D))
        {
            phys.moveRight();

            CurrentFrame += 5 * time;
            if (CurrentFrame >= 5) CurrentFrame -= 5;

            p.sprite.setTextureRect(IntRect(144 * int(CurrentFrame), 144, 144, 144));
            moving = true;
        }

        // ВЛЕВО
        if (Keyboard::isKeyPressed(Keyboard::Left) || Keyboard::isKeyPressed(Keyboard::A))
        {
            phys.moveLeft();

            CurrentFrame += 5 * time;
            if (CurrentFrame >= 5) CurrentFrame -= 5;

            p.sprite.setTextureRect(IntRect(144 * int(CurrentFrame), 288, 144, 144));
            moving = true;
        }

        // ПРЫЖОК
        if (Keyboard::isKeyPressed(Keyboard::Up) || Keyboard::isKeyPressed(Keyboard::W))
        {
            phys.jump();
        }

        // ПОКОЙ
        if (!moving)
        {
            CurrentFrame += 2 * time;
            if (CurrentFrame >= 5) CurrentFrame -= 5;

            p.sprite.setTextureRect(IntRect(144 * int(CurrentFrame), 0, 144, 144));
        }

        // ФИЗИКА
        phys.update(p, time, map);

        // МОНЕТЫ
        for (auto& coin : coins)
        {
            coin.update(time, coinTexture);

            if (!coin.collected &&
                coin.sprite.getGlobalBounds().intersects(p.sprite.getGlobalBounds()))
            {
                coin.collected = true;
            }
        }

        // ОГРАНИЧЕНИЕ ЭКРАНА
        float x = p.x;
        float y = p.y;

        float w = p.sprite.getGlobalBounds().width;
        float h = p.sprite.getGlobalBounds().height;

        if (x < 0) x = 0;
        if (x > 640 - w) x = 640 - w;
        if (y < 0) y = 0;
        if (y > 480 - h) y = 480 - h;

        p.x = x;
        p.y = y;
        p.sprite.setPosition(p.x, p.y);

        // РЕНДЕР
        window.clear();
        window.draw(background);
        map.draw(window);

        for (auto& coin : coins)
        {
            if (!coin.collected)
                window.draw(coin.sprite);
        }

        window.draw(p.sprite);
        window.display();
    }

    return 0;
}