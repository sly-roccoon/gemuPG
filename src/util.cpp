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

ImVec4 toImVec4(SDL_Color color)
{
  return ImVec4(color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f);
}

SDL_Color toSDLColor(ImVec4 color)
{
  return {static_cast<Uint8>(color.x * 255), static_cast<Uint8>(color.y * 255), static_cast<Uint8>(color.z * 255), static_cast<Uint8>(color.w * 255)};
}

std::array<Vector2f, 4> getAdjacentPositions(Vector2f pos) // in order UP, LEFT, DOWN, RIGHT
{
  pos = floorVec(pos);

  std::array<Vector2f, 4> adjacent_pos;
  adjacent_pos[0] = Vector2f({pos.x, pos.y - 1});
  adjacent_pos[1] = Vector2f({pos.x - 1, pos.y});
  adjacent_pos[2] = Vector2f({pos.x, pos.y + 1});
  adjacent_pos[3] = Vector2f({pos.x + 1, pos.y});

  return adjacent_pos;
}