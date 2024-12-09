#include "util.h"

Vector2 floorVec(Vector2 vec)
{
    return {floorf(vec.x), floorf(vec.y)};
}