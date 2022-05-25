#include "object.h"

class Player : public Object
{
private:
    int bulletCount;
    int specialBulletCount;
    int respawn;
    int bulletAngle;
    RECT bulletRT;
    int heart;
    BOOL isOverPower;

public:
    Player();
    void init(int);
    void init(POINT);

    void move(int, RECT);
    void draw(HDC);

    void in_bullet_rotate();
    RECT pos_bullet(int);
    void bullet_reload();

    BulletInfo shoot(int);
    void over_power_on();

    void heart_down();
    int get_heart();

    void pickup_special_bullet();
};
