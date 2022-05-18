#include "player.h"

Player::Player()
{
    RECT tempRT = {-PLAYER_SIZE / 2, -PLAYER_SIZE / 2, PLAYER_SIZE / 2, PLAYER_SIZE / 2};
    shape = tempRT;
    position.x = 0;
    position.y = 0;
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
        position.x = 50 + 30 * 8;
        position.y = 50 + 30 * 8;
        break;
    case 1:
        position.x = 50 + 30 * 10;
        position.y = 50 + 30 * 8 - 15;
        break;
    case 2:
        position.x = 50 + 30 * 10;
        position.y = 50 + 30 * 8 - 15;
        break;
    }

    direction = RIGHT;
    isLive = TRUE;
    bulletCount = MAX_BULLET;
}

void Player::draw(HDC mdc)
{
    HBRUSH hBrush, oldBrush;
    HPEN hPen, oldPen;

    // Player
    hBrush = CreateSolidBrush(RGB(20, 20, 20));
    hPen = CreatePen(PS_SOLID, 1, RGB(20, 20, 20));
    oldBrush = (HBRUSH)SelectObject(mdc, hBrush);
    oldPen = (HPEN)SelectObject(mdc, hPen);
    RECT drawRT = pos_shape();
    RoundRect(mdc, drawRT.left, drawRT.top, drawRT.right, drawRT.bottom, 5, 5);
    SelectObject(mdc, oldBrush);
    SelectObject(mdc, oldPen);

    // Bullet circle
    

    DeleteObject(hBrush);
    DeleteObject(hPen);
}