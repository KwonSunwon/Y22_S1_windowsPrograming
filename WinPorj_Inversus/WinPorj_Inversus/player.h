#include "object.h"

class Player : public Object
{
public:
    Player();
    void player_move(int);
};

class Bullet : public Player
{
};