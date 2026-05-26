#include "player.h"

Player::Player(String F, float X, float Y, float W, float H)
{
    File = F;
    w = W; h = H;

    image.loadFromFile("images/" + File);

    texture.loadFromImage(image);
    sprite.setTexture(texture);
    sprite.setScale(0.3f, 0.3f);   
    x = X; y = Y;
    sprite.setTextureRect(IntRect(0, 0, w, h));
}

void Player::update(float time)
{
    switch (dir)
    {
    case 0: dx = speed; dy = 0; break;
    case 1: dx = -speed; dy = 0; break;
    case 2: dx = 0; dy = speed; break;
    case 3: dx = 0; dy = -speed; break;
    }
    x += dx * time;
    y += dy * time;
    speed = 0;
    sprite.setPosition(x, y);
}