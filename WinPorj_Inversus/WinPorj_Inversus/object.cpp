#include "object.h"

RECT Object::pos_shape()
{
    RECT tempRT = shape;
    OffsetRect(&tempRT, position.x, position.y);
    return tempRT;
}