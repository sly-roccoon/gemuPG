#include "area.h"
#include <algorithm>
#include "util.h"

bool Area::isInside(Vector2f pos)
{
	pos = floorVec(pos);
	for (auto &field : positions_)
		if (field == pos)
			return true;

	return false;
}

void Area::addBlock(Block *block)
{
	blocks_.push_back(block);
}

void Area::removeBlock(Block *block)
{
	blocks_.erase(
		std::remove_if(blocks_.begin(), blocks_.end(),
					   [block](auto b)
					   { return b == block; }),
		blocks_.end());
}

void Area::removeBlock(Vector2f pos)
{
	pos = floorVec(pos);

	blocks_.erase(
		std::remove_if(blocks_.begin(), blocks_.end(),
					   [&pos](auto block)
					   { return block->getPos() == pos; }),
		blocks_.end());
}