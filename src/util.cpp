#include "util.h"

Vector2f floorVec(Vector2f vec)
{
  return {floorf(vec.x), floorf(vec.y)};
}

float calcCrest(float *wave)
{
  float crest = 0.0f;
  float peak = 0.0f;
  float sq_sum = 0.0f;
  for (int i = 0; i < WAVE_SIZE; i++)
  {
    peak = wave[i] > peak ? wave[i] : peak;
    sq_sum += powf(wave[i], 2);
  }
  crest = peak / sqrtf(sq_sum / WAVE_SIZE);
  return crest;
}