#include "object.h"

class Player : public Object
{
public:
    void player_move(int)
    {
        object_move_left();
    }
};