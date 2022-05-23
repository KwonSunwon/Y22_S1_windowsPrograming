#include "object.h"

class Enemy : public Object
{
private:
public:
    void enemy_spawn(RECT);
    void move_to_player(POINT);
    void draw(HDC);
};