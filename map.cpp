#include "map.h"

Map::Map()
{
    int temp[HEIGHT][WIDTH] = {
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {2,2,2,2,2,2,2,2,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {1,1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,2,2,2,2,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,2,2,2,2,2,2,2},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,2,1,1,1,1,1,1,1,1,1},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,1,1,1,1,1,1,1,1,1,1,1,1},
        {2,2,2,2,2,2,2,2,2,2,2,0,0,0,2,2,2,1,1,1,1,1,1,1,1,1,1,1,1,1},
        {1,1,1,1,1,1,1,1,1,1,1,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
    };

    for (int i = 0; i < HEIGHT; i++)
        for (int j = 0; j < WIDTH; j++)
            TileMap[i][j] = temp[i][j];

    t_ground.loadFromFile("images/ground.png");
    t_grass.loadFromFile("images/grass_block.png");
    t_platform.loadFromFile("images/platform.png");

    if (t_ground.getSize().x == 0) {
        Image img;
        img.create(24, 24, Color(139, 69, 19));
        t_ground.loadFromImage(img);
    }
    if (t_grass.getSize().x == 0) {
        Image img;
        img.create(24, 24, Color(34, 139, 34));
        t_grass.loadFromImage(img);
    }
    if (t_platform.getSize().x == 0) {
        Image img;
        img.create(24, 24, Color(160, 82, 45));
        t_platform.loadFromImage(img);
    }
} // ← закрывает конструктор Map::Map()

void Map::draw(RenderWindow& window) // БЕЗ точки с запятой!
{
    for (int i = 0; i < HEIGHT; i++)
    {
        for (int j = 0; j < WIDTH; j++)
        {
            if (TileMap[i][j] == 0) continue;

            Sprite tileSprite;
            Texture* currentTexture = nullptr;

            if (TileMap[i][j] == 1) currentTexture = &t_ground;
            if (TileMap[i][j] == 2) currentTexture = &t_grass;
            if (TileMap[i][j] == 3) currentTexture = &t_platform;

            if (currentTexture)
            {
                currentTexture->setSmooth(false);
                tileSprite.setTexture(*currentTexture);

                float scaleX = (float)TILE_SIZE / currentTexture->getSize().x;
                float scaleY = (float)TILE_SIZE / currentTexture->getSize().y;
                tileSprite.setScale(scaleX, scaleY);

                tileSprite.setPosition((float)(j * TILE_SIZE), (float)(i * TILE_SIZE));

                window.draw(tileSprite);
            }
        }
    }
}