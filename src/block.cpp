#include "block.h"
#include "raymath.h"

Block::Block()
{
	type_ = BLOCK_NONE;
	bypass_ = true;
	pos_ = Vector2Zero();
}

BlockGenerator::BlockGenerator(Vector2 pos) : Block()
{
	type_ = BLOCK_GENERATOR;
	pos_ = pos;
	amp_ = 1.0f;
	pan_ = 0.0f;
	// Wavetable wave;
	// wave.setSineWave(0);
	// osc_.setTable(wave);
}