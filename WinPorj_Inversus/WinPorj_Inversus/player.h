#include "object.h"

#define MAX_BULLET 6
#define RESPAWN_TIME 500
#define SPEED 50
#define PLAYER_SIZE 28

class Player : public Object
{
private:
    int bulletCount;
    int respawn;

public:
    Player();
    void init(int);
    void move(int);
    void draw(HDC);
};
