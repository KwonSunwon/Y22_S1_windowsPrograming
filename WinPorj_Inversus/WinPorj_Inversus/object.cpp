#include "object.h"

RECT Object::get_pos_rect()
{
    RECT tempRT = shape;
    OffsetRect(&tempRT, position.x, position.y);
    return tempRT;
}

int Object::get_speed()
{
    return speed;
}

void Object::move_left()
{
    position.x -= speed;
}
void Object::move_right()
{
    position.x += speed;
}
void Object::move_up()
{
    position.y -= speed;
}
void Object::move_down()
{
    position.y += speed;
}

POINT Object::get_position()
{
    return position;
}

POINT Object::get_point()
{
    POINT temp = {
        (position.x - MAP_LOCATION) / MAP_SIZE,
        (position.y - MAP_LOCATION) / MAP_SIZE,
    };
    return temp;
}

BOOL Object::get_is_live()
{
    return isLive;
}

void Object::dead()
{
    if (isLive)
    {
        isLive = FALSE;
        deadEffect = TRUE;
        deadEffectCount = 30;
    }
}

void Object::dead_effect(HDC mdc)
{
    if (deadEffect)
    {
        HBRUSH hBrush, oldBrush;

        hBrush = CreateSolidBrush(RGB(20, 90, 100));
        oldBrush = (HBRUSH)SelectObject(mdc, hBrush);

        RECT draw;
        RECT circle = {0, 0, 20, 20};
        int x, y, size;
        OffsetRect(&circle, position.x, position.y);

        draw = circle;
        x = (rand() % 30);
        y = (rand() % 30);
        size = rand() % 30;
        InflateRect(&draw, size, size);
        OffsetRect(&draw, x, y);
        Ellipse(mdc, draw.left, draw.top, draw.right, draw.bottom);

        draw = circle;
        x = -(rand() % 30);
        y = -(rand() % 30);
        size = rand() % 30;
        InflateRect(&draw, size, size);
        OffsetRect(&draw, x, y);
        Ellipse(mdc, draw.left, draw.top, draw.right, draw.bottom);

        SelectObject(mdc, oldBrush);
        DeleteObject(hBrush);

        if (deadEffectCount-- == 0)
            deadEffect = FALSE;
    }
}