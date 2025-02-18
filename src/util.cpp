#include "util.h"
#include <map>

Vector2f floorVec(Vector2f vec)
{
  return {floorf(vec.x), floorf(vec.y)};
}

float calcCrest(std::array<float, WAVE_SIZE> wave)
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

std::map<std::string, pitch_t> noteToFreqMap = {
    {"C", 16.35f},
    {"C#", 17.32f},
    {"D", 18.35f},
    {"D#", 19.45f},
    {"E", 20.60f},
    {"F", 21.83f},
    {"F#", 23.12f},
    {"G", 24.50f},
    {"G#", 25.96f},
    {"A", 27.50f},
    {"A#", 29.14f},
    {"B", 30.87f}};

std::pair<std::string, int>
freqToNote(pitch_t freq)
{
  freq = std::abs(freq);
  int octave = 0;
  while (freq > 31.7762f)
  {
    freq /= 2.0f;
    octave++;
  }

  std::pair<std::string, pitch_t> closest = {"C", 16.35f};
  for (auto &note : noteToFreqMap)
    if (std::abs(freq - note.second) < std::abs(freq - closest.second))
      closest = note;

  return {closest.first, octave};
}

pitch_t noteToFreq(std::string note, int octave)
{
  return noteToFreqMap.at(note) * pow(2, octave);
}

pitch_t intervalToRatio(float interval, float octave_subdivision)
{
  return pow(2, interval / octave_subdivision);
}

SDL_Color invertColor(SDL_Color color)
{
  return {(Uint8)(255 - color.r), (Uint8)(255 - color.g), (Uint8)(255 - color.b), color.a};
}

float interpTable(std::array<float, WAVE_SIZE> array, float idx)
{
  int lower = std::floor(idx);
  int upper = std::ceil(idx);

  float lower_factor = idx - lower;
  float upper_factor = upper - idx;

  if (upper > WAVE_SIZE)
    upper = WAVE_SIZE;

  if (lower < 0)
    lower = 0;

  float value = (array[upper] * upper_factor + array[lower] * lower_factor);

  return value;
}