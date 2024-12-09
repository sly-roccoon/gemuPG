#include "grid.h"
#include "raymath.h"
#include "rlgl.h"

void Grid::update() // will def need optimization
{
	// for (auto &area : areas_)
	// 	for (auto &block : blocks_)
	// 		if (area->isInside(block->getPos()))
	// 			area->addBlock(block);
	// 		else
	// 			area->removeBlock(block);
}

void Grid::draw() // TODO direct 2d drawing
{
	for (int i = 0; i < GRID_SIZE; ++i)
	{
		// horizontal lines
		DrawLine(-GRID_SIZE / 2, -GRID_SIZE / 2 + GRID_SPACING * i, GRID_SIZE / 2, -GRID_SIZE / 2 + GRID_SPACING * i, GRID_COLOR);
		// vertical lines
		DrawLine(-GRID_SIZE / 2 + GRID_SPACING * i, -GRID_SIZE / 2, -GRID_SIZE / 2 + GRID_SPACING * i, +GRID_SIZE / 2, GRID_COLOR);
	}
}

void Grid::drawBlocks()
{
	for (auto block : blocks_)
	{
		block->draw();
	}
}

void Grid::addBlock(std::shared_ptr<Block> block)
{
	blocks_.push_back(std::move(block));
}

bool Grid::removeBlock(Vector2 pos)
{
	return (blocks_.end() == blocks_.erase(
								 std::remove_if(blocks_.begin(), blocks_.end(),
												[pos](auto block)
												{ return Vector2Equals(block->getPos(), pos); }),
								 blocks_.end()));
}

void Grid::removeBlock(std::shared_ptr<Block> block)
{
	blocks_.erase(
		std::remove_if(blocks_.begin(), blocks_.end(),
					   [block](auto b)
					   { return b == block; }),
		blocks_.end());
}

std::shared_ptr<Block> Grid::getBlock(Vector2 pos)
{
	for (auto &block : blocks_)
		if (block->getPos() == pos)
			return block;

	for (auto &area : areas_)
		for (auto &block : area->getBlocks())
			if (block->getPos() == pos)
				return block;

	return nullptr;
}