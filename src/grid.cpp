#include "grid.h"
#include "raymath.h"
#include "rlgl.h"

void Grid::update() // will def need optimization
{
	for (auto &area : areas_)
		for (auto &block : blocks_)
			if (area->isInside(block->getPos()))
				area->addBlock(block);
			else
				area->removeBlock(block);
}

void Grid::draw() // TODO direct 2d drawing
{
	rlPushMatrix();
	{
		rlTranslatef(0, 25 * 50, 0);
		rlRotatef(90, 1, 0, 0);
		DrawGrid(GRID_SIZE, GRID_SPACING);
	}
	rlPopMatrix();
}

void Grid::addBlock(std::shared_ptr<Block> block)
{
	blocks_.push_back(block);
}

void Grid::removeBlock(Vector2 pos)
{
	blocks_.erase(
		std::remove_if(blocks_.begin(), blocks_.end(),
					   [pos](auto block)
					   { return Vector2Equals(block->getPos(), pos); }),
		blocks_.end());
}

void Grid::removeBlock(std::shared_ptr<Block> block)
{
	blocks_.erase(
		std::remove_if(blocks_.begin(), blocks_.end(),
					   [block](auto b)
					   { return b == block; }),
		blocks_.end());
}