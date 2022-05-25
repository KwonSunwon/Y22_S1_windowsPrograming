#pragma once

#include "object.h"

typedef class Bullet : public Object
{
private:
    int direction;
    BOOL isSpecial;
    BOOL isShoot;

public:
    void make_bullet(BulletInfo);
    void make_special_bullet(BulletInfo);
    void move();
    void draw(HDC);
    int get_type();
    BOOL get_is_shoot();
} Bullet;