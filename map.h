#pragma once
#include <SFML/Graphics.hpp>

using namespace sf;

class Map {
public:
    static const int HEIGHT = 20;
    static const int WIDTH = 30;

    int TileMap[HEIGHT][WIDTH];

    static const int TILE_SIZE = 24;

    Texture t_ground, t_grass, t_platform;

    Map();
    void loadLevel(int level);   // новая функция
    void draw(RenderWindow& window);
};