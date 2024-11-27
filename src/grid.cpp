#include "grid.h"

void Grid::update() //will def need optimization	
{
	for (auto& area : areas_)
		for (auto& block : blocks_)
			if (area->isInside(block->getPos()))
				area->addBlock(block);
			else
				area->removeBlock(block);
}

void Grid::addBlock(std::shared_ptr<Block> block)
{
	blocks_.push_back(block);
}

void Grid::removeBlock(Vec2 pos)
{
	blocks_.erase(
		std::remove_if(blocks_.begin(), blocks_.end(),
			[pos](auto block) { return block->getPos() == pos; }),
		blocks_.end());
}