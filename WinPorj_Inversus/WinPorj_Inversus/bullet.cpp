#pragma once

#include "bullet.h"

void Bullet::make_bullet(BulletInfo info)
{
    direction = info._direction;
    position = info._position;
    speed = BULLET_SPEED;
    RECT tempRT = {-BULLET_SIZE * 2, -BULLET_SIZE * 2, BULLET_SIZE * 2, BULLET_SIZE * 2};
    shape = tempRT;
    isLive = TRUE;
    isShoot = TRUE;
}

void Bullet::move()
{
    switch (direction)
    {
    case LEFT:
        move_left();
        break;
    case RIGHT:
        move_right();
        break;
    case UP:
        move_up();
        break;
    case DOWN:
        move_down();
        break;
    }
}

void Bullet::draw(HDC mdc)
{
    HBRUSH hBrush, oldBrush;
    HPEN hPen, oldPen;
    COLORREF shadow(RGB(20, 20, 20));

    RECT drawRT = get_pos_rect();

    switch (direction)
    {
    case LEFT:
        for (int i = 0; i < 15; ++i)
        {
            hPen = CreatePen(PS_SOLID, BULLET_SIZE * 3, shadow);
            oldPen = (HPEN)SelectObject(mdc, hPen);
            MoveToEx(mdc, ((drawRT.left + drawRT.right) / 2), ((drawRT.top + drawRT.bottom) / 2), NULL);
            OffsetRect(&drawRT, 1, 0);
            LineTo(mdc, ((drawRT.left + drawRT.right) / 2), ((drawRT.top + drawRT.bottom) / 2));
            SelectObject(mdc, oldPen);
            DeleteObject(hPen);
            shadow += RGB(15, 15, 15);
        }
        break;
    case RIGHT:
        for (int i = 0; i < 15; ++i)
        {
            hPen = CreatePen(PS_SOLID, BULLET_SIZE * 3, shadow);
            oldPen = (HPEN)SelectObject(mdc, hPen);
            MoveToEx(mdc, ((drawRT.left + drawRT.right) / 2), ((drawRT.top + drawRT.bottom) / 2), NULL);
            OffsetRect(&drawRT, -1, 0);
            LineTo(mdc, ((drawRT.left + drawRT.right) / 2), ((drawRT.top + drawRT.bottom) / 2));
            SelectObject(mdc, oldPen);
            DeleteObject(hPen);
            shadow += RGB(15, 15, 15);
        }
        break;
    case UP:
        for (int i = 0; i < 15; ++i)
        {
            hPen = CreatePen(PS_SOLID, BULLET_SIZE * 3, shadow);
            oldPen = (HPEN)SelectObject(mdc, hPen);
            MoveToEx(mdc, ((drawRT.left + drawRT.right) / 2), ((drawRT.top + drawRT.bottom) / 2), NULL);
            OffsetRect(&drawRT, 0, 1);
            LineTo(mdc, ((drawRT.left + drawRT.right) / 2), ((drawRT.top + drawRT.bottom) / 2));
            SelectObject(mdc, oldPen);
            DeleteObject(hPen);
            shadow += RGB(15, 15, 15);
        }
        break;
    case DOWN:
        for (int i = 0; i < 15; ++i)
        {
            hPen = CreatePen(PS_SOLID, BULLET_SIZE * 3, shadow);
            oldPen = (HPEN)SelectObject(mdc, hPen);
            MoveToEx(mdc, ((drawRT.left + drawRT.right) / 2), ((drawRT.top + drawRT.bottom) / 2), NULL);
            OffsetRect(&drawRT, 0, -1);
            LineTo(mdc, ((drawRT.left + drawRT.right) / 2), ((drawRT.top + drawRT.bottom) / 2));
            SelectObject(mdc, oldPen);
            DeleteObject(hPen);
            shadow += RGB(15, 15, 15);
        }
        break;
    }

    hBrush = CreateSolidBrush(RGB(0, 0, 0));
    oldBrush = (HBRUSH)SelectObject(mdc, hBrush);
    drawRT = get_pos_rect();
    Ellipse(mdc, drawRT.left, drawRT.top, drawRT.right, drawRT.bottom);
    SelectObject(mdc, oldBrush);
    DeleteObject(hBrush);

    SelectObject(mdc, oldBrush);
    DeleteObject(hBrush);
}