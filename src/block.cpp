#include "block.h"
#include "raymath.h"
#include "raylib.h"

Block::Block()
{
	type_ = BLOCK_NONE;
	bypass_ = true;
	pos_ = Vector2Zero();
}

void Block::draw()
{
	DrawRectangle(pos_.x, pos_.y, 1, 1, BLUE); //! magic :(
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