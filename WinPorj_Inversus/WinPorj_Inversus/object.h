#include <Windows.h>

class Object
{
protected:
    POINT location;
    RECT shape;
    BOOL isLive;
    COLORREF color;

public:
    void object_move_left();
    void object_move_right();
    void object_move_up();
    void object_move_down();
    void object_draw();
};