#pragma once

#include "stdafx.h"

class Object
{
protected:
    POINT position;
    RECT shape;
    BOOL isLive;
    COLORREF color;
    int speed;

public:
    void move_left();
    void move_right();
    void move_up();
    void move_down();

    RECT get_pos_rect();
    int get_speed();

    POINT get_position();
    POINT get_point();
};