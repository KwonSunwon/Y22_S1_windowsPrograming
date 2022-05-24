#include "player.h"

Player::Player()
{
    isOverPower = FALSE;
    RECT tempRT = {-PLAYER_SIZE, -PLAYER_SIZE, PLAYER_SIZE, PLAYER_SIZE};
    shape = tempRT;
    position.x = 0;
    position.y = 0;
    isLive = FALSE;
    speed = SPEED;

    respawn = 0;
    bulletCount = MAX_BULLET;
    specialBulletCount = 0;
    bulletAngle = 0;

    RECT tempBulletRT = {-BULLET_SIZE, -BULLET_SIZE, BULLET_SIZE, BULLET_SIZE};
    bulletRT = tempBulletRT;
}

// TODO
// 맵 모양 바꾸면 거기에 맞춰서 시작 위치 조정해 줄것
// 매개변수로 row, col 받던지 map 받아서 처리 해야할듯?
void Player::init(int level)
{
    switch (level)
    {
    case 0:
        position.x = MAP_LOCATION + MAP_SIZE * (ROW / 2);
        position.y = MAP_LOCATION + MAP_SIZE * (ROW / 2);
        break;
    case 1:
        position.x = MAP_LOCATION + MAP_SIZE * 10;
        position.y = MAP_LOCATION + MAP_SIZE * (ROW / 2) - 15;
        break;
    case 2:
        position.x = MAP_LOCATION + MAP_SIZE * 10;
        position.y = MAP_LOCATION + MAP_SIZE * (ROW / 2) - 15;
        break;
    }

    isLive = TRUE;
}

void Player::draw(HDC mdc)
{
    HBRUSH hBrush, h2Brush, oldBrush;
    HPEN hPen, oldPen;

    if (isLive)
    {
        // Player
        hBrush = CreateSolidBrush(RGB(20, 20, 20));
        hPen = CreatePen(PS_SOLID, 1, RGB(150, 150, 150));
        oldBrush = (HBRUSH)SelectObject(mdc, hBrush);
        oldPen = (HPEN)SelectObject(mdc, hPen);
        RECT drawRT = get_pos_rect();
        RoundRect(mdc, drawRT.left, drawRT.top, drawRT.right, drawRT.bottom, 5, 5);
        SelectObject(mdc, oldBrush);
        SelectObject(mdc, oldPen);
        DeleteObject(hBrush);
        DeleteObject(hPen);

        // Bullet circle
        hBrush = CreateSolidBrush(RGB(200, 200, 200));
        h2Brush = CreateSolidBrush(RGB(0, 100, 200));
        hPen = CreatePen(PS_SOLID, 1, RGB(200, 200, 200));
        oldBrush = (HBRUSH)SelectObject(mdc, hBrush);
        oldPen = (HPEN)SelectObject(mdc, hPen);
        int tempAngle;
        tempAngle = bulletAngle;
        for (int i = 0; i < bulletCount; ++i)
        {
            drawRT = pos_bullet(tempAngle);
            Ellipse(mdc, drawRT.left, drawRT.top, drawRT.right, drawRT.bottom);
            tempAngle = (tempAngle + 60) % 360;
        }
        oldBrush = (HBRUSH)SelectObject(mdc, h2Brush);
        for (int i = 0; i < specialBulletCount; ++i)
        {
            drawRT = pos_bullet(tempAngle);
            Ellipse(mdc, drawRT.left, drawRT.top, drawRT.right, drawRT.bottom);
            tempAngle = (tempAngle + 60) % 360;
        }
        SelectObject(mdc, oldBrush);
        SelectObject(mdc, oldPen);
        DeleteObject(hBrush);
        DeleteObject(h2Brush);
        DeleteObject(hPen);
    }
}

RECT Player::pos_bullet(int _angle)
{
    RECT temp = {
        bulletRT.left + position.x + 8 * cos(_angle * M_PI / 180),
        bulletRT.top + position.y + 8 * sin(_angle * M_PI / 180),
        bulletRT.right + position.x + 8 * cos(_angle * M_PI / 180),
        bulletRT.bottom + position.y + 8 * sin(_angle * M_PI / 180),
    };
    return temp;
}

void Player::in_bullet_rotate()
{
    bulletAngle = (bulletAngle + 5) % 360;
}

void Player::move(int _direction, RECT _mapSize)
{
    switch (_direction)
    {
    case LEFT:
        if (position.x <= _mapSize.left)
            break;
        else
            move_left();
        break;
    case RIGHT:
        if (position.x >= _mapSize.right)
            break;
        else
            move_right();
        break;
    case UP:
        if (position.y <= _mapSize.top)
            break;
        else
            move_up();
        break;
    case DOWN:
        if (position.y >= _mapSize.bottom)
            break;
        else
            move_down();
        break;
    }
}

BulletInfo Player::shoot(int _direction)
{
    BulletInfo info = {{-1, -1}, -1};
    if (bulletCount)
    {
        --bulletCount;
        info._position = position;
        info._direction = _direction;
    }
    return info;
}

void Player::bullet_reload()
{
    if (bulletCount < MAX_BULLET)
        ++bulletCount;
}

void Player::over_power_on()
{
    isOverPower = TRUE;
}