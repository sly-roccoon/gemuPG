#include "area.h"

bool Area::isInside(Vec2 pos)
{
	for (Vec2 field : positions_)
		if (pos == field)
			return true;

	return false;
}

void Area::addBlock(std::shared_ptr<Block> block)
{
	blocks_.push_back(block);
}

void Area::removeBlock(std::shared_ptr<Block> block)
{
	blocks_.erase(
		std::remove_if(blocks_.begin(), blocks_.end(),
			[block](auto b) { return b == block; }),
		blocks_.end());
}

void Area::removeBlock(Vec2 pos)
{
	blocks_.erase(
		std::remove_if(blocks_.begin(), blocks_.end(),
			[pos](auto block) { return block->getPos() == pos; }),
		blocks_.end());
}