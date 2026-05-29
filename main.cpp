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
#include "pushable_box.h"

using namespace sf;
using namespace std;

// Вспомогательная функция для перезагрузки текущего уровня
void resetLevel(int level, Map& map, Player& p, vector<Coin>& coins, vector<Spike>& spikes,
    Sprite& exitSprite, Texture& coinTexture, Texture& spikeTexture, Texture& exitTexture,
    int& coinsCollected, float& startX, float& startY, float spikeYBase,
    vector<PushableBox>& boxes, Texture& boxTexture)  // Добавили параметры для блоков
{
    // Загружаем карту
    map.loadLevel(level);

    // Устанавливаем стартовые координаты игрока
    if (level == 1) {
        startX = 0;
        startY = 300;
    }
    else if (level == 2) {
        // Начало на левом нижнем выступе: строка 18, столбец 0 (там тайл 2 - трава)
        startX = 0;
        startY = 18 * Map::TILE_SIZE - p.sprite.getGlobalBounds().height; // 432 - высота
    }
    p.x = startX;
    p.y = startY;
    p.sprite.setPosition(p.x, p.y);

    // Пересоздаём монеты
    coins.clear();
    if (level == 1) {
        coins.push_back(Coin(coinTexture, 280, 330));
        coins.push_back(Coin(coinTexture, 10, 155));
        coins.push_back(Coin(coinTexture, 360, 100));
    }
    else if (level == 2) {
        // Монеты на видных местах (можно изменить)
        coins.push_back(Coin(coinTexture, 300, 350));
        coins.push_back(Coin(coinTexture, 500, 300));
        coins.push_back(Coin(coinTexture, 200, 400));
    }

    // Пересоздаём шипы
    spikes.clear();
    float spikeWidth = spikeTexture.getSize().x * 0.12f;

    if (level == 2) {
        // Шипы в ямке: дно на строке 19, столбцы 6 и 7
        float spikeY = 480 - (spikeTexture.getSize().y * 0.12f) + 10;
        float startSpikeX = 140;
        for (int i = 0; i < 2; i++) {
            Spike s(spikeTexture, startSpikeX + i * (spikeWidth + 2), spikeY);
            s.sprite.setScale(0.12f, 0.12f);
            spikes.push_back(s);
        }
    }
    else if (level == 1) {
        // Шипы для первого уровня (оставляем как было)
        float spikeY = 480 - (spikeTexture.getSize().y * 0.12f) + 10;
        float startSpikeX = 260;
        for (int i = 0; i < 2; i++) {
            Spike s(spikeTexture, startSpikeX + i * (spikeWidth + 2), spikeY);
            s.sprite.setScale(0.12f, 0.12f);
            spikes.push_back(s);
        }
    }

    // 📦 ДЕРЕВЯННЫЕ БЛОКИ - появятся ТОЛЬКО на первом уровне
    boxes.clear();
    if (level == 1) {
        boxes.emplace_back(
            boxTexture,
            12 * Map::TILE_SIZE,
            10 * Map::TILE_SIZE - 32
        );
        boxes.emplace_back(
            boxTexture,
            40,                    // X = 40
            160                    // Y = 160
        );
    }

    // Позиция портала (выхода)
    if (level == 1) {
        exitSprite.setPosition(560, 10);
    }
    else if (level == 2) {
        // Правый верхний угол (чтобы был доступен)
        exitSprite.setPosition(560, 35);
    }

    coinsCollected = 0;
}

int main()
{
    RenderWindow window(VideoMode(640, 480), "Fairy");

    Music music;
    if (music.openFromFile("C:/Users/RemSot/fairy-game/sounds/background_2_music.ogg"))
    {
        music.setLoop(true);
        music.setVolume(30);
        music.play();
    }

    int currentLevel = 1;
    float startX = 0, startY = 300;
    Player p("hero.png", startX, startY, 144, 144);

    Map map;
    physics phys;

    // Текстуры
    Texture coinTexture;
    coinTexture.loadFromFile("images/coin.png");
    Texture spikeTexture;
    spikeTexture.loadFromFile("images/spike.png");
    Texture exitTexture;
    exitTexture.loadFromFile("images/exit.png");
    Sprite exitSprite(exitTexture);
    exitSprite.setScale(0.1f, 0.1f);

    // 📦 Текстура деревянного блока
    Texture boxTexture;
    boxTexture.loadFromFile("C:/Users/RemSot/fairy-game/images/box.png");

    vector<Coin> coins;
    vector<Spike> spikes;
    vector<PushableBox> boxes;
    int coinsCollected = 0;
    float spikeYBase = 480 - (spikeTexture.getSize().y * 0.12f) + 10;

    // Загружаем первый уровень
    resetLevel(currentLevel, map, p, coins, spikes, exitSprite, coinTexture, spikeTexture, exitTexture,
        coinsCollected, startX, startY, spikeYBase, boxes, boxTexture);  // Передаём блоки

    phys.boxes = &boxes;

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

  
    // Меню
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
    font.loadFromFile("C:/Users/RemSot/fairy-game/fonts/uvKits.ttf");
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

    Texture bgTexture;
    bgTexture.loadFromFile("images/background.png");
    Sprite background(bgTexture);
    float scaleX = (float)window.getSize().x / bgTexture.getSize().x;
    float scaleY = (float)window.getSize().y / bgTexture.getSize().y;
    float bgScale = max(scaleX, scaleY);
    background.setScale(bgScale, bgScale);
    // ☁️ ОБЛАКА
    Texture cloudTexture;
    cloudTexture.loadFromFile("images/cloud.png");

    vector<Sprite> clouds;

    for (int i = 0; i < 8; i++)
    {
        Sprite cloud(cloudTexture);

        // случайный размер
        float scale = 0.06f + (rand() % 7) / 100.f;
        cloud.setScale(scale, scale);

        // случайная позиция
        float x = rand() % 800 - 100; // даже за экраном
        float y = rand() % 80 - 40;   // выше и не в линию

        cloud.setPosition(x, y);

        clouds.push_back(cloud);
    }

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

                // Кнопка RESTART
                if (restartText.getGlobalBounds().contains(mouse.x, mouse.y))
                {
                    resetLevel(currentLevel, map, p, coins, spikes, exitSprite, coinTexture, spikeTexture, exitTexture,
                        coinsCollected, startX, startY, spikeYBase, boxes, boxTexture);
                    phys = physics(); // сброс физики
                    phys.boxes = &boxes; // ВАЖНО: переназначаем указатель
                    levelComplete = false;
                }
                // Кнопка NEXT LEVEL
                else if (nextText.getGlobalBounds().contains(mouse.x, mouse.y))
                {
                    if (currentLevel < 2)   // максимум 2 уровня
                        currentLevel++;
                    else
                        currentLevel = 1;   // зацикливание

                    resetLevel(currentLevel, map, p, coins, spikes, exitSprite, coinTexture, spikeTexture, exitTexture,
                        coinsCollected, startX, startY, spikeYBase, boxes, boxTexture);
                    phys = physics();
                    phys.boxes = &boxes; // ВАЖНО: переназначаем указатель
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
        {
            // Игрок
            phys.update(p, time, map);

            // Блоки - обновляем только если они есть
            for (auto& box : boxes)
            {
                box.applyGravity(time, map);

                float pushDir = 0.f;

                if (box.checkPlayerCollision(
                    p.x,
                    p.y,
                    p.sprite.getGlobalBounds().width,
                    p.sprite.getGlobalBounds().height,
                    pushDir))
                {
                    // Толкание только при движении
                    if ((Keyboard::isKeyPressed(Keyboard::A) ||
                        Keyboard::isKeyPressed(Keyboard::Left))
                        && pushDir == -1)
                    {
                        box.push(pushDir, time, map);
                    }

                    if ((Keyboard::isKeyPressed(Keyboard::D) ||
                        Keyboard::isKeyPressed(Keyboard::Right))
                        && pushDir == 1)
                    {
                        box.push(pushDir, time, map);
                    }
                }
            }
        }

        // 💰 МОНЕТЫ
        for (auto& coin : coins)
        {
            coin.update(time, coinTexture);
            if (!coin.collected && coin.sprite.getGlobalBounds().intersects(p.sprite.getGlobalBounds()))
            {
                coin.collected = true;
                coinsCollected++;
            }
        }

        // Шипы
        for (auto& spike : spikes)
        {
            if (spike.checkCollision(p.x, p.y, p.sprite.getGlobalBounds().width, p.sprite.getGlobalBounds().height))
            {
                resetLevel(currentLevel, map, p, coins, spikes, exitSprite, coinTexture, spikeTexture, exitTexture,
                    coinsCollected, startX, startY, spikeYBase, boxes, boxTexture);
                phys = physics();
                phys.boxes = &boxes; // ВАЖНО: переназначаем указатель
                // не сбрасываем levelComplete, просто перезапускаем уровень
            }
        }

        // Выход
        if (!levelComplete && p.sprite.getGlobalBounds().intersects(exitSprite.getGlobalBounds()))
        {
            levelComplete = true;
        }

        // ☁️ ДВИЖЕНИЕ ОБЛАКОВ
        for (auto& cloud : clouds)
        {
            // разная скорость в зависимости от размера
            float speed = 50.f + cloud.getScale().x * 120.f;

            cloud.move(speed * time, 0);

            // если улетело вправо
            if (cloud.getPosition().x > 750)
            {
                float scale = 0.03f + (rand() % 4) / 100.f;
                cloud.setScale(scale, scale);

                cloud.setPosition(
                    -100.f - rand() % 200,
                    rand() % 80 - 40
                );
            }
        }
        window.clear();

        if (!levelComplete)
        {
            window.draw(background);

            // рисуем облака
            for (auto& cloud : clouds)
            {
                window.draw(cloud);
            }

            map.draw(window);

            for (auto& coin : coins)
                if (!coin.collected)
                    window.draw(coin.sprite);

            for (auto& spike : spikes)
                spike.draw(window);

            for (auto& box : boxes)  // Блоки рисуются только если есть
                box.draw(window);

            window.draw(exitSprite);
            window.draw(p.sprite);
        }
        else
        {
            window.draw(background);

            for (auto& cloud : clouds)
            {
                window.draw(cloud);
            }

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