#include "enemy.h"

void Enemy::enemy_spawn(RECT _mapSize)
{
    RECT rcTemp = {
        -PLAYER_SIZE,
        -PLAYER_SIZE,
        PLAYER_SIZE,
        PLAYER_SIZE,
    };
    shape = rcTemp;
    isLive = TRUE;
    speed = ENEMY_SPEED;

    srand(time(NULL));
    POINT pos = {
        rand() % (_mapSize.right - _mapSize.left + 1) + _mapSize.left,
        rand() % (_mapSize.bottom - _mapSize.top + 1) + _mapSize.top,
    };
    position = pos;
}

void Enemy::draw(HDC mdc)
{
    HBRUSH hBrush, h2Brush, oldBrush;
    HPEN hPen, oldPen;

    if (isLive)
    {
        // Player
        hBrush = CreateSolidBrush(RGB(20, 80, 150));
        hPen = CreatePen(PS_SOLID, 1, RGB(150, 150, 150));
        oldBrush = (HBRUSH)SelectObject(mdc, hBrush);
        oldPen = (HPEN)SelectObject(mdc, hPen);
        RECT drawRT = get_pos_rect();
        RoundRect(mdc, drawRT.left, drawRT.top, drawRT.right, drawRT.bottom, 5, 5);
        SelectObject(mdc, oldBrush);
        SelectObject(mdc, oldPen);
        DeleteObject(hBrush);
        DeleteObject(hPen);
    }
}

void move_to_player(POINT _playerPos)
{
    // TODO
    // 여기서부터 시작, Enemy class 아직 테스트 안함
}