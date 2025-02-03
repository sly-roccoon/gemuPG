#pragma once
#include <cmath>
#include <numbers>
#include <SDL3/SDL.h>
#include <imgui.h>

#define ADJUST_AMP_BY_CREST true

constexpr unsigned int WIDTH = 1280;
constexpr unsigned int HEIGHT = 720;

constexpr unsigned int ICON_SIZE = 64;

typedef float pitch_t;

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
constexpr unsigned int SAMPLE_RATE = 44100;
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