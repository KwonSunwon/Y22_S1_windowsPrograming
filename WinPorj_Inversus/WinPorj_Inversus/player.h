#include "object.h"

class Player : public Object
{
private:
    int bulletCount;
    int respawn;
    int bulletAngle;
    RECT bulletRT;

public:
    Player();
    void init(int);
    void move(int, RECT);
    void draw(HDC);
    RECT pos_bullet(int);
};
