#pragma once

#include "stdafx.h"

#define LEFT 100
#define RIGHT 101
#define UP 102
#define DOWN 103

class Object
{
protected:
    RECT location;
    int direction;
    BOOL isLive;
    COLORREF color;
    int speed;

public:
    void move_left();
    void move_right();
    void move_up();
    void move_down();
    void draw();
};