#include "area.h"
#include <algorithm>
#include "raymath.h"

bool Area::isInside(Vector2 pos)
{
	for (Vector2 field : positions_)
		if (Vector2Equals(field, pos))
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
					   [block](auto b)
					   { return b == block; }),
		blocks_.end());
}

void Area::removeBlock(Vector2 pos)
{
	blocks_.erase(
		std::remove_if(blocks_.begin(), blocks_.end(),
					   [pos](auto block)
					   { return block->getPos() == pos; }),
		blocks_.end());
}