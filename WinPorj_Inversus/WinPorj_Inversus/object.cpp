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
    position.x -= 3;
}
void Object::move_right()
{
    position.x += 3;
}
void Object::move_up()
{
    position.y -= 3;
}
void Object::move_down()
{
    position.y += 3;
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
    std::cout << temp.x << ", " << temp.y << "\n";
    return temp;
}