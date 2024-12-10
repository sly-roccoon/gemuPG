#pragma once
#include <cmath>
#include "raymath.h"

constexpr float MIN_CAMERA_ZOOM = 16.0f;
constexpr float MAX_CAMERA_ZOOM = 255.0f;
constexpr float DEFAULT_CAMERA_ZOOM = sqrtf(powf(MIN_CAMERA_ZOOM, 2) + powf(MAX_CAMERA_ZOOM, 2));

constexpr unsigned int BUFFER_SIZE = 512;
constexpr unsigned int SAMPLE_RATE = 44100;
constexpr unsigned int BIT_DEPTH = 16;
constexpr unsigned int CHANNELS = 1;

Vector2 floorVec(Vector2 vec);