#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <algorithm>
#include <iostream>
#include <vector>

#include "player.h"
#include "map.h"
#include "physics.h"
#include "coin.h"
#include "spike.h"

using namespace sf;
using namespace std;

int main()
{
    RenderWindow window(VideoMode(640, 480), "Fairy");

    Music music;
    if (music.openFromFile("sounds/background_2_music.ogg"))
    {
        music.setLoop(true);
        music.setVolume(30);
        music.play();
    }

    float startX = 0;
    float startY = 300;

    Player p("hero.png", startX, startY, 144, 144);

    Map map;
    physics phys;

    // 💰 МОНЕТЫ
    Texture coinTexture;
    coinTexture.loadFromFile("images/coin.png");

    vector<Coin> coins = {
        Coin(coinTexture, 200, 300),
        Coin(coinTexture, 300, 250),
        Coin(coinTexture, 400, 200)
    };

    int coinsCollected = 0;

    // 💀 ШИПЫ
    Texture spikeTexture;
    spikeTexture.loadFromFile("images/spike.png");

    vector<Spike> spikes;

    float spikeScale = 0.12f;

    // реальный размер
    float spikeWidth = spikeTexture.getSize().x * spikeScale;

    // ❗ СТАВИМ ЧУТЬ НИЖЕ, ЧЕМ ВЫСОТА ЭКРАНА
    float spikeY = 480 - (spikeTexture.getSize().y * spikeScale) + 10;

    // 👉 только 2 шипа (как ты просила)
    float startSpikeX = 260;

    for (int i = 0; i < 2; i++)
    {
        Spike s(spikeTexture, startSpikeX + i * (spikeWidth + 2), spikeY);
        s.sprite.setScale(spikeScale, spikeScale);
        spikes.push_back(s);
    }

    // 💰 ИКОНКИ
    vector<Sprite> coinIcons;

    int frameW = coinTexture.getSize().x / 6;
    int frameH = coinTexture.getSize().y;

    for (int i = 0; i < 3; i++)
    {
        Sprite icon(coinTexture);
        icon.setTextureRect(IntRect(0, 0, frameW, frameH));

        float iconScale = 50.0f / frameW;
        icon.setScale(iconScale, iconScale);

        icon.setPosition(235 + i * 60, 70);
        coinIcons.push_back(icon);
    }

    // 🚪 ПОРТАЛ
    Texture exitTexture;
    exitTexture.loadFromFile("images/exit.png");
    Sprite exitSprite(exitTexture);
    exitSprite.setPosition(10, 10);
    exitSprite.setScale(0.15f, 0.15f);

    // 📜 МЕНЮ
    Texture menuTexture;
    menuTexture.loadFromFile("images/menu.png");
    Sprite menuSprite(menuTexture);

    float menuScale = 0.5f;
    menuSprite.setScale(menuScale, menuScale);

    float menuW = menuTexture.getSize().x * menuScale;
    float menuH = menuTexture.getSize().y * menuScale;

    menuSprite.setPosition((640 - menuW) / 2, (480 - menuH) / 2);

    RectangleShape darkOverlay(Vector2f(640, 480));
    darkOverlay.setFillColor(Color(0, 0, 0, 150));

    Font font;
    font.loadFromFile("fonts/uvKits.ttf");

    Texture& fontTex = const_cast<Texture&>(font.getTexture(32));
    fontTex.setSmooth(false);

    Text nextText("NEXT LEVEL", font, 32);
    nextText.setFillColor(Color(226, 125, 145));
    nextText.setOutlineColor(Color::Black);
    nextText.setOutlineThickness(2);

    Text restartText("RESTART", font, 32);
    restartText.setFillColor(Color(52, 183, 250));
    restartText.setOutlineColor(Color::Black);
    restartText.setOutlineThickness(2);

    FloatRect nb = nextText.getLocalBounds();
    nextText.setOrigin(nb.left + nb.width / 2, nb.top + nb.height / 2);
    nextText.setPosition(320, 187);

    FloatRect rb = restartText.getLocalBounds();
    restartText.setOrigin(rb.left + rb.width / 2, rb.top + rb.height / 2);
    restartText.setPosition(320, 365);

    bool levelComplete = false;

    float CurrentFrame = 0;
    Clock clock;

    // 🌄 ФОН
    Texture bgTexture;
    bgTexture.loadFromFile("images/background.png");
    Sprite background(bgTexture);

    float scaleX = (float)window.getSize().x / bgTexture.getSize().x;
    float scaleY = (float)window.getSize().y / bgTexture.getSize().y;
    float bgScale = max(scaleX, scaleY); // ← фикс имени
    background.setScale(bgScale, bgScale);

    while (window.isOpen())
    {
        float time = clock.restart().asSeconds();

        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
                window.close();

            if (levelComplete && event.type == Event::MouseButtonPressed)
            {
                Vector2i mouse = Mouse::getPosition(window);

                if (restartText.getGlobalBounds().contains(mouse.x, mouse.y))
                {
                    p.x = startX;
                    p.y = startY;
                    p.sprite.setPosition(p.x, p.y);

                    coinsCollected = 0;

                    for (auto& coin : coins)
                        coin.collected = false;

                    phys = physics();
                    levelComplete = false;
                }
            }
        }

        bool moving = false;

        if (!levelComplete)
        {
            if (Keyboard::isKeyPressed(Keyboard::Right) || Keyboard::isKeyPressed(Keyboard::D))
            {
                phys.moveRight();
                CurrentFrame += 5 * time;
                if (CurrentFrame >= 5) CurrentFrame -= 5;
                p.sprite.setTextureRect(IntRect(144 * int(CurrentFrame), 144, 144, 144));
                moving = true;
            }

            if (Keyboard::isKeyPressed(Keyboard::Left) || Keyboard::isKeyPressed(Keyboard::A))
            {
                phys.moveLeft();
                CurrentFrame += 5 * time;
                if (CurrentFrame >= 5) CurrentFrame -= 5;
                p.sprite.setTextureRect(IntRect(144 * int(CurrentFrame), 288, 144, 144));
                moving = true;
            }

            if (Keyboard::isKeyPressed(Keyboard::Up) || Keyboard::isKeyPressed(Keyboard::W))
                phys.jump();
        }

        if (!moving)
        {
            CurrentFrame += 2 * time;
            if (CurrentFrame >= 5) CurrentFrame -= 5;
            p.sprite.setTextureRect(IntRect(144 * int(CurrentFrame), 0, 144, 144));
        }

        if (!levelComplete)
            phys.update(p, time, map);

        // 💰 МОНЕТЫ
        for (auto& coin : coins)
        {
            coin.update(time, coinTexture);

            if (!coin.collected &&
                coin.sprite.getGlobalBounds().intersects(p.sprite.getGlobalBounds()))
            {
                coin.collected = true;
                coinsCollected++;
            }
        }

        // 💀 ШИПЫ
        for (auto& spike : spikes)
        {
            if (spike.checkCollision(
                p.x, p.y,
                p.sprite.getGlobalBounds().width,
                p.sprite.getGlobalBounds().height))
            {
                p.x = startX;
                p.y = startY;
                p.sprite.setPosition(p.x, p.y);

                coinsCollected = 0;

                for (auto& coin : coins)
                    coin.collected = false;
            }
        }

        // 🚪 ВЫХОД
        if (!levelComplete &&
            p.sprite.getGlobalBounds().intersects(exitSprite.getGlobalBounds()))
        {
            levelComplete = true;
        }

        window.clear();

        if (!levelComplete)
        {
            window.draw(background);
            map.draw(window);

            for (auto& coin : coins)
                if (!coin.collected)
                    window.draw(coin.sprite);

            for (auto& spike : spikes)
                spike.draw(window);

            window.draw(exitSprite);
            window.draw(p.sprite);
        }
        else
        {
            window.draw(background);
            window.draw(darkOverlay);
            window.draw(menuSprite);

            for (int i = 0; i < min(coinsCollected, 3); i++)
                window.draw(coinIcons[i]);

            window.draw(nextText);
            window.draw(restartText);
        }

        window.display();
    }

    return 0;
}