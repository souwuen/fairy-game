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
                int& coinsCollected, float& startX, float& startY,
                vector<Sprite>& trees, Sprite& house, Texture& treeTexture, Texture& houseTexture)
{
    // Загружаем карту
    map.loadLevel(level);

    // Устанавливаем стартовые координаты игрока
    if (level == 1) {
        startX = 0;
        startY = 300;
    } else if (level == 2) {
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
    } else if (level == 2) {
        coins.push_back(Coin(coinTexture, 300, 350));
        coins.push_back(Coin(coinTexture, 500, 300));
        coins.push_back(Coin(coinTexture, 200, 400));
    }

    // Пересоздаём шипы
    spikes.clear();
    float spikeScale = 0.12f;
    float spikeWidth = spikeTexture.getSize().x * spikeScale;

    if (level == 1) {
        // Шипы на самом низу карты (строка 19, Y = 456)
        float spikeY = 19 * Map::TILE_SIZE;
        float startSpikeX = 260;
        for (int i = 0; i < 2; i++) {
            Spike s(spikeTexture, startSpikeX + i * (spikeWidth + 2), spikeY);
            s.sprite.setScale(spikeScale, spikeScale);
            spikes.push_back(s);
        }
    } else if (level == 2) {
        // Шипы в ямке (дно строки 19, столбцы 6 и 7)
        float spikeY = 19 * Map::TILE_SIZE;
        float spikeX_start = 6 * Map::TILE_SIZE;
        for (int i = 0; i < 2; i++) {
            Spike s(spikeTexture, spikeX_start + i * (spikeWidth + 2), spikeY);
            s.sprite.setScale(spikeScale, spikeScale);
            spikes.push_back(s);
        }
    }

    // Позиция портала (выхода)
    if (level == 1) {
        exitSprite.setPosition(560, 10);
    } else if (level == 2) {
        exitSprite.setPosition(560, 35);
    }

    // 🌲 ДЕКОРАЦИИ (ёлочки и домик)
    trees.clear();
    if (level == 1) {
        // Ёлочки на первом уровне (координаты подберите сами)
        auto addTree = [&](float x, float y, float scale = 0.3f) {
            Sprite s(treeTexture);
            s.setScale(scale, scale);
            s.setPosition(x, y);
            trees.push_back(s);
        };
        addTree(115, 127, 0.2f);
        addTree(150, 93);
        addTree(535, 295, 0.2f);
        addTree(570, 262);

        // Домик
        house.setTexture(houseTexture);
        house.setScale(0.05f, 0.05f);
        house.setPosition(5, 340);
    } else if (level == 2) {
        // Ёлочки на втором уровне
        auto addTree = [&](float x, float y, float scale = 0.2f) {
            Sprite s(treeTexture);
            s.setScale(scale, scale);
            s.setPosition(x, y);
            trees.push_back(s);
        };
        addTree(600, 100);
        addTree(580, 120);
        addTree(300, 450);
        addTree(50, 420);

        // Домик
        house.setTexture(houseTexture);
        house.setScale(0.3f, 0.3f);
        house.setPosition(5, 340);
    }

    coinsCollected = 0;
}

int main()
{
    RenderWindow window(VideoMode(640, 480), "Dota 3 beta");

    Music music;
    if (music.openFromFile("sounds/background_2_music.ogg"))
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

    // Текстуры для декораций
    Texture treeTexture;
    treeTexture.loadFromFile("images/tree.png");
    Texture houseTexture;
    houseTexture.loadFromFile("images/house.png");

    vector<Coin> coins;
    vector<Spike> spikes;
    int coinsCollected = 0;

    // Вектор для ёлочек и спрайт домика
    vector<Sprite> trees;
    Sprite house;

    // Загружаем первый уровень
    resetLevel(currentLevel, map, p, coins, spikes, exitSprite, coinTexture, spikeTexture, exitTexture,
               coinsCollected, startX, startY, trees, house, treeTexture, houseTexture);

    // 📦 ДЕРЕВЯННЫЙ БЛОК (ящик)
    Texture boxTexture;
    boxTexture.loadFromFile("images/box.png");

    vector<PushableBox> boxes;
    boxes.emplace_back(boxTexture, 12 * Map::TILE_SIZE, 10 * Map::TILE_SIZE - 32);
    boxes.emplace_back(boxTexture, 40, 160);

    phys.boxes = &boxes;

    // 💰 ИКОНКИ для меню
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

    Texture bgTexture;
    bgTexture.loadFromFile("images/background.png");
    Sprite background(bgTexture);
    float scaleX = (float)window.getSize().x / bgTexture.getSize().x;
    float scaleY = (float)window.getSize().y / bgTexture.getSize().y;
    float bgScale = max(scaleX, scaleY);
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

                // Кнопка RESTART
                if (restartText.getGlobalBounds().contains(mouse.x, mouse.y))
                {
                    resetLevel(currentLevel, map, p, coins, spikes, exitSprite, coinTexture, spikeTexture, exitTexture,
                               coinsCollected, startX, startY, trees, house, treeTexture, houseTexture);
                    phys = physics();
                    phys.boxes = &boxes; // важно восстановить указатель
                    levelComplete = false;
                }
                // Кнопка NEXT LEVEL
                else if (nextText.getGlobalBounds().contains(mouse.x, mouse.y))
                {
                    if (currentLevel < 2)
                        currentLevel++;
                    else
                        currentLevel = 1;

                    resetLevel(currentLevel, map, p, coins, spikes, exitSprite, coinTexture, spikeTexture, exitTexture,
                               coinsCollected, startX, startY, trees, house, treeTexture, houseTexture);
                    phys = physics();
                    phys.boxes = &boxes;
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

            // Блоки (ящики)
            for (auto& box : boxes)
            {
                box.applyGravity(time, map);

                float pushDir = 0.f;
                if (box.checkPlayerCollision(
                    p.x, p.y,
                    p.sprite.getGlobalBounds().width,
                    p.sprite.getGlobalBounds().height,
                    pushDir))
                {
                    if ((Keyboard::isKeyPressed(Keyboard::A) || Keyboard::isKeyPressed(Keyboard::Left)) && pushDir == -1)
                        box.push(pushDir, time, map);
                    if ((Keyboard::isKeyPressed(Keyboard::D) || Keyboard::isKeyPressed(Keyboard::Right)) && pushDir == 1)
                        box.push(pushDir, time, map);
                }
            }
        }

        // Монеты
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
                           coinsCollected, startX, startY, trees, house, treeTexture, houseTexture);
                phys = physics();
                phys.boxes = &boxes;
            }
        }

        // Выход
        if (!levelComplete && p.sprite.getGlobalBounds().intersects(exitSprite.getGlobalBounds()))
        {
            levelComplete = true;
        }

        window.clear();

        if (!levelComplete)
        {
            window.draw(background);
            map.draw(window);

            // Декорации
            for (auto& t : trees) window.draw(t);
            window.draw(house);

            for (auto& coin : coins) if (!coin.collected) window.draw(coin.sprite);
            for (auto& spike : spikes) spike.draw(window);
            for (auto& box : boxes) box.draw(window);
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