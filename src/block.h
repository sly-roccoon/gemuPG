#pragma once
// #include <cstdint>
#include <memory>
#include "raylib.h"
#include "util.h"

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
	blockType getType() { return type_; }
	virtual void draw() = 0;
	virtual std::shared_ptr<Block> clone() = 0;

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
	~BlockGenerator();

	void draw() override;
	std::shared_ptr<Block> clone() override;
	void processAudio();

private:
	AudioStream stream_;

	struct Audio
	{
		float amp;
		float pan;
		float freq;
		float idx;
	};

	Audio audio_;

	void audioCallback(void *bufferData, unsigned int frames);
};