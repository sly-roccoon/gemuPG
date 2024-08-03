#include "block.h"

Block::Block()
{
	type_ = BLOCK_NONE;
	bypass_ = true;
	pos_ = Vec2(0, 0);
}

BlockGenerator::BlockGenerator(Vec2 pos) : Block()
{
	type_ = BLOCK_GENERATOR;
	pos_ = pos;
	amp_ = 1.0f;
	pan_ = 0.0f;
	pdsp::WaveTable wave;
	wave.setSineWave(0);
	osc_.setTable(wave);
}