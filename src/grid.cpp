#include "grid.h"

void Grid::update() // will def need optimization
{
	// for (auto &area : areas_)
	// 	for (auto &block : blocks_)
	// 		if (area->isInside(block->getPos()))
	// 			area->addBlock(block);
	// 		else
	// 			area->removeBlock(block);
}

void Grid::draw(SDL_Renderer *renderer)
{
	SDL_SetRenderDrawColor(renderer, GRID_COLOR.r, GRID_COLOR.g, GRID_COLOR.b, GRID_COLOR.a);
	for (int i = 0; i < GRID_SIZE; ++i)
	{
		// horizontal lines
		Vector2f hori_from = {(float)-GRID_SIZE / 2, (float)-GRID_SIZE / 2 + i};
		Vector2f hori_to = {(float)GRID_SIZE / 2, (float)-GRID_SIZE / 2 + i};

		SDL_RenderLine(renderer,
					   Camera::worldToScreen(hori_from).x,
					   Camera::worldToScreen(hori_from).y,
					   Camera::worldToScreen(hori_to).x,
					   Camera::worldToScreen(hori_to).y);

		// vertical lines
		Vector2f vert_from = {(float)-GRID_SIZE / 2 + i, (float)-GRID_SIZE / 2};
		Vector2f vert_to = {(float)-GRID_SIZE / 2 + i, (float)+GRID_SIZE / 2};

		SDL_RenderLine(renderer,
					   Camera::worldToScreen(vert_from).x,
					   Camera::worldToScreen(vert_from).y,
					   Camera::worldToScreen(vert_to).x,
					   Camera::worldToScreen(vert_to).y);
	}
}

void Grid::drawBlocks(SDL_Renderer *renderer)
{
	SDL_SetRenderDrawColor(renderer, 0, 0, 255, SDL_ALPHA_OPAQUE);
	for (auto block : blocks_)
	{
		SDL_RenderFillRect(renderer, block->getFRect());
	}
}

void Grid::addBlock(Block *block)
{
	blocks_.push_back(std::move(block));
}

bool Grid::removeBlock(Vector2f pos)
{
	return (blocks_.end() == blocks_.erase(
								 std::remove_if(blocks_.begin(), blocks_.end(),
												[pos](auto block)
												{ return block->getPos() == pos; }),
								 blocks_.end()));
}

void Grid::removeBlock(Block *block)
{
	blocks_.erase(
		std::remove_if(blocks_.begin(), blocks_.end(),
					   [block](auto b)
					   { return b == block; }),
		blocks_.end());
}

Block *Grid::getBlock(Vector2f pos)
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