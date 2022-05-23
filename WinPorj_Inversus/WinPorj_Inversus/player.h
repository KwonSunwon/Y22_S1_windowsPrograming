#include "object.h"

class Player : public Object
{
private:
    int bulletCount;
    int specialBulletCount;
    int respawn;
    int bulletAngle;
    RECT bulletRT;

public:
    Player();
    void init(int);
    void move(int, RECT);
    void draw(HDC);
    void in_bullet_rotate();
    RECT pos_bullet(int);
    void bullet_reload();

    BulletInfo shoot(int);
};
