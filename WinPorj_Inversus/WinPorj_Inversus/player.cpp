#include "player.h"

Player::Player()
{
    RECT rcTemp = {0, 0, 0, 0};
    location = rcTemp;
    direction = RIGHT;
    isLive = FALSE;
    speed = SPEED;

    respawn = 0;
    bulletCount = MAX_BULLET;
}

void Player::init(int level)
{
    switch (level)
    {
    case 0:
        location.left = 50 + 30 * 8;
        location.top = 50 + 30 * 8;
        location.right = location.left + PLAYER_SIZE;
        location.bottom = location.top + PLAYER_SIZE;
        isLive = TRUE;
        break;
    case 1:
        location.left = 50 + 30 * 10;
        location.top = 50 + 30 * 8;
        location.right = location.left + PLAYER_SIZE;
        location.bottom = location.top + PLAYER_SIZE;
        isLive = TRUE;
        break;
    case 2:
        location.left = 50 + 30 * 8;
        location.top = 50 + 30 * 8;
        location.right = location.left + PLAYER_SIZE;
        location.bottom = location.top + PLAYER_SIZE;
        isLive = TRUE;
        break;
    }
}