#include "enemy.h"

Enemy::Enemy()
{
    type = ENEMY;
    isLive = FALSE;
    deadEffect = FALSE;
}

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
    deadEffect = FALSE;

    srand(time(NULL));
    POINT pos = {
        rand() % (_mapSize.right - _mapSize.left) + _mapSize.left,
        rand() % (_mapSize.bottom - _mapSize.top) + _mapSize.top,
    };
    position = pos;
}

void Enemy::draw(HDC mdc)
{
    if (isLive)
    {
        HBRUSH hBrush, h2Brush, oldBrush;
        HPEN hPen, oldPen;

        hBrush = CreateSolidBrush(RGB(20, 90, 100));
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

void Enemy::hatch_draw(HDC mdc, RECT tile)
{
    if (isLive)
    {
        HBRUSH hBrush;
        HPEN hPen, oldPen;
        int height, width;

        height = (tile.bottom - tile.top) / 4;
        width = (tile.right - tile.left) / 4;

        hBrush = CreateSolidBrush(RGB(0, 70, 80));
        FrameRect(mdc, &tile, hBrush);
        hPen = CreatePen(PS_SOLID, 1, RGB(20, 90, 100));
        oldPen = (HPEN)SelectObject(mdc, hPen);

        for (int i = 0; i < 4; ++i)
        {
            MoveToEx(mdc, tile.left, tile.bottom - height * (i + 1), NULL);
            LineTo(mdc, tile.left + width * (i + 1), tile.bottom);
        }
        MoveToEx(mdc, tile.left, tile.top, NULL);
        LineTo(mdc, tile.right, tile.bottom);
        for (int i = 0; i < 4; ++i)
        {
            MoveToEx(mdc, tile.left + width * (i + 1), tile.top, NULL);
            LineTo(mdc, tile.right, tile.bottom - height * (i + 1));
        }

        SelectObject(mdc, oldPen);
        DeleteObject(hPen);
        DeleteObject(hBrush);
    }
}

void Enemy::move(POINT _playerPos)
{
    int moveX, moveY;
    POINT movePos = {_playerPos.x - position.x, _playerPos.y - position.y};
    if (abs(movePos.x) >= abs(movePos.y) * 2)
    {
        if (movePos.x < 0)
            for (int i = 0; i < 2; ++i)
                move_left();
        else if (movePos.x > 0)
            for (int i = 0; i < 2; ++i)
                move_right();
    }
    else if (abs(movePos.x) * 2 <= abs(movePos.y))
    {
        if (movePos.y < 0)
            for (int i = 0; i < 2; ++i)
                move_up();
        else if (movePos.y > 0)
            for (int i = 0; i < 2; ++i)
                move_down();
    }
    else
    {
        if (movePos.x < 0)
            move_left();
        else
            move_right();
        if (movePos.y < 0)
            move_up();
        else
            move_down();
    }
}