#pragma once

#include "stdafx.h"

#define LEFT 100
#define RIGHT 101
#define UP 102
#define DOWN 103

class Object
{
protected:
    POINT position;
    RECT shape;
    int direction;
    BOOL isLive;
    COLORREF color;
    int speed;

public:
    void move_left();
    void move_right();
    void move_up();
    void move_down();
    RECT pos_shape();
};