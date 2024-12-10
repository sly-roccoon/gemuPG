#include "block.h"
#include "raymath.h"
#include "raylib.h"

Block::Block()
{
	type_ = BLOCK_NONE;
	bypass_ = true;
	pos_ = Vector2Zero();
}

void BlockGenerator::draw()
{
	DrawRectangle(pos_.x, pos_.y, 1, 1, BLUE); //! magic :(
}

std::shared_ptr<Block> BlockGenerator::clone()
{
	std::shared_ptr<BlockGenerator> copy = std::make_shared<BlockGenerator>(pos_);

	copy->audio_ = audio_;
	return copy;
}

BlockGenerator::BlockGenerator(Vector2 pos) : Block()
{
	type_ = BLOCK_GENERATOR;
	pos_ = pos;
	audio_.amp = 32000.0f;
	audio_.freq = 440.0f;
	audio_.pan = 0.0f;

	stream_ = LoadAudioStream(SAMPLE_RATE, BIT_DEPTH, CHANNELS);
	SetAudioStreamCallback(stream_, (AudioCallback)&audioCallback);

	PlayAudioStream(stream_);
}

BlockGenerator::~BlockGenerator()
{
	UnloadAudioStream(stream_);
}

void BlockGenerator::processAudio()
{
}

void BlockGenerator::audioCallback(void *buffer, unsigned int frames)
{
	float delta = audio_.freq / SAMPLE_RATE;
	short *d = (short *)buffer;
	for (int i = 0; i < frames; i++)
	{
		d[i] = audio_.amp * sin(2 * PI * audio_.idx);
		audio_.idx += delta;
		if (audio_.idx >= 1.0f)
			audio_.idx -= 1.0f;
	}
}