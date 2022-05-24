#include "object.h"

class Enemy : public Object
{
private:
public:
    Enemy();
    void enemy_spawn(RECT);
    void move(POINT);
    void draw(HDC);
    void hatch_draw(HDC, RECT);
};