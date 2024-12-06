#pragma once
// #include <cstdint>
#include <memory>
#include "raylib.h"

typedef enum
{
	BLOCK_NONE,
	BLOCK_GENERATOR,
	BLOCK_SEQUENCER,
	BLOCK_SETTINGS,
	BLOCK_EFFECT,
	BLOCK_MODULATOR
} blockType;

class Block
{
public:
	Block();
	Vector2 getPos() { return pos_; }

protected:
	blockType type_;
	Vector2 pos_;
	bool bypass_;
};

#pragma endregion

#pragma region GENERATOR BLOCK

typedef enum
{
	WAVE_SAMPLE,
	WAVE_SAW,
	WAVE_SINE,
	WAVE_SQUARE,
	WAVE_TRIANGLE
} WAVE_FORMS;

class BlockGenerator : public Block
{
public:
	BlockGenerator(Vector2);

private:
	// Oscillator				osc_;
	// ADSR					adsr_;
	float amp_;
	float pan_;
};