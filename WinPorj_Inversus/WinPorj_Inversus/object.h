#pragma once

#include <Windows.h>
#include <iostream>

class Object
{
protected:
    POINT location;
    RECT shape;
    BOOL isLive;
    COLORREF color;
    int speed;

public:
    void object_move_left();
    void object_move_right();
    void object_move_up();
    void object_move_down();
    void object_draw();
};