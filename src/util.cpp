#include "util.h"

Vector2f floorVec(Vector2f vec)
{
  return {floorf(vec.x), floorf(vec.y)};
}