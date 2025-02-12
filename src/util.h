#pragma once
#include <cmath>
#include <numbers>
#include <SDL3/SDL.h>
#include <imgui.h>
#include <array>
#include <string>

#define ADJUST_AMP_BY_CREST true

constexpr unsigned int DEFAULT_WIDTH = 1280;
constexpr unsigned int DEFAULT_HEIGHT = 960;

constexpr unsigned int ICON_SIZE = 64;

constexpr float DEFAULT_BPM = 60.0f;
constexpr unsigned int MAX_SUBDIVISION = 64;

constexpr bool SEQUENCER_RANDOMIZED = true;

using pitch_t = float;

typedef enum
{
	PITCH_REL_FREQUENCY,
	PITCH_ABS_FREQUENCY,
	PITCH_INTERVAL,
	PITCH_NOTE
} pitch_type_t;

typedef enum
{
	AREA,
	BLOCK_GENERATOR,
	BLOCK_SEQUENCER,
	// BLOCK_SETTINGS,
	// BLOCK_EFFECT,
	// BLOCK_MODULATOR
} block_type_t;

constexpr SDL_Color GENERATOR_COLOR = {0, 0, 255, SDL_ALPHA_OPAQUE};
constexpr SDL_Color AREA_COLOR = {128, 128, 128, 128};
constexpr SDL_Color SEQUENCER_COLOR = {255, 255, 0, SDL_ALPHA_OPAQUE};

constexpr float BLOCK_SIZE_FACTOR = 0.8f;
constexpr int GRID_SIZE = 512;
constexpr SDL_Color GRID_COLOR = {128, 128, 128, SDL_ALPHA_OPAQUE};

constexpr float MIN_CAMERA_ZOOM = 16.0f;
constexpr float MAX_CAMERA_ZOOM = 255.0f;
const float DEFAULT_CAMERA_ZOOM = std::sqrt(std::pow(MIN_CAMERA_ZOOM, 2) + std::pow(MAX_CAMERA_ZOOM, 2));

constexpr unsigned int BUFFER_SIZE = 512;
constexpr unsigned int SAMPLE_RATE = 96000;
constexpr unsigned int BIT_DEPTH = 16;
constexpr unsigned int CHANNELS = 1;

constexpr unsigned int WAVE_SIZE = 512;
constexpr double TWOPI = 2.0f * std::numbers::pi;

constexpr SDL_AudioSpec DEFAULT_SPEC = {
	.format = SDL_AUDIO_F32,
	.channels = CHANNELS,
	.freq = SAMPLE_RATE};

template <typename T>
struct Vector2
{
	T x{};
	T y{};

	constexpr Vector2() = default;
	constexpr Vector2(T x, T y) : x(x), y(y) {}

	Vector2 operator/(T scalar) const { return Vector2(x / scalar, y / scalar); }
	Vector2 operator+(const Vector2 &other) const { return Vector2(x + other.x, y + other.y); }
	Vector2 operator-(const Vector2 &other) const { return Vector2(x - other.x, y - other.y); }
	Vector2 operator*(T scalar) const { return Vector2(x * scalar, y * scalar); }
	bool operator==(const Vector2 &other) const { return x == other.x && y == other.y; }
	Vector2 operator+=(const Vector2 &other) const { return {x + other.x, y + other.y}; }
};

using Vector2f = Vector2<float>;
using Vector2i = Vector2<int>;

Vector2f floorVec(Vector2f vec);

float calcCrest(float *wave);

ImVec4 toImVec4(SDL_Color color);

std::array<Vector2f, 4> getAdjacentPositions(Vector2f pos);

std::pair<std::string, int> freqToNote(pitch_t);
pitch_t noteToFreq(std::string, int);
pitch_t intervalToRatio(float interval, float octave_subdivision);
