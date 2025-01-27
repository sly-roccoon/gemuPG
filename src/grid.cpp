#include "grid.h"
#include <array>
#include <queue>

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
	SDL_SetRenderDrawColor(renderer, GENERATOR_COLOR.r, GENERATOR_COLOR.g, GENERATOR_COLOR.b, GENERATOR_COLOR.a);
	for (auto block : blocks_)
	{
		SDL_RenderFillRect(renderer, block->getFRect());
	}

	for (auto &area : areas_)
	{
		for (auto &block : area->getBlocks())
		{
			SDL_RenderFillRect(renderer, block->getFRect());
		}
	}
}

void Grid::drawAreas(SDL_Renderer *renderer)
{
	SDL_SetRenderDrawColor(renderer, AREA_COLOR.r, AREA_COLOR.g, AREA_COLOR.b, AREA_COLOR.a);
	for (auto &area : areas_)
	{
		for (auto &pos : area->getPositions())
		{
			SDL_FRect rect = {Camera::worldToScreen(pos).x, Camera::worldToScreen(pos).y, Camera::getZoom(), Camera::getZoom()};
			SDL_RenderFillRect(renderer, &rect);
		}
	}
}

void Grid::addBlock(Block *block)
{
	if (!block)
		return;

	Area *area = getArea(block->getPos());
	if (area)
	{
		area->addBlock(block);
		return;
	}
	blocks_.push_back(std::move(block));
}

bool Grid::removeGlobalBlock(Vector2f pos)
{
	pos = floorVec(pos);
	return (blocks_.end() != blocks_.erase(
								 std::remove_if(blocks_.begin(), blocks_.end(),
												[pos](auto block)
												{ return block->getPos() == pos; }),
								 blocks_.end()));
}

bool Grid::removeAreaBlock(Vector2f pos)
{
	pos = floorVec(pos);

	for (auto &area : areas_)
		if (area->removeBlock(pos))
			return true;

	return false;
}

bool Grid::removeBlock(Vector2f pos)
{
	if (removeGlobalBlock(pos))
		return true;

	return removeAreaBlock(pos);
}

void Grid::removeGlobalBlock(Block *block)
{
	blocks_.erase(
		std::remove_if(blocks_.begin(), blocks_.end(),
					   [block](auto b)
					   { return b == block; }),
		blocks_.end());
}

void Grid::removeAreaBlock(Block *block)
{
	for (auto &area : areas_)
		area->removeBlock(block);
}

void Grid::removeBlock(Block *block)
{
	removeGlobalBlock(block);
	removeAreaBlock(block);
}

Block *Grid::getBlock(Vector2f pos)
{
	pos = floorVec(pos);
	for (auto &block : blocks_)
		if (block->getPos() == pos)
			return block;

	for (auto &area : areas_)
	{
		Block *block = area->getBlock(pos);
		if (block)
			return block;
	}

	return nullptr;
}

std::vector<Block *> Grid::getBlocks()
{
	std::vector<Block *> blocks = blocks_;
	for (auto &area : areas_)
	{
		auto area_blocks = area->getBlocks();
		blocks.insert(std::end(blocks), std::begin(area_blocks), std::end(area_blocks));
	}

	return blocks;
}

//--------------------------------------------------------
Area *Grid::getArea(Vector2f pos)
{
	pos = floorVec(pos);

	for (auto &area : areas_)
		if (area->isInside(pos))
			return area;

	return nullptr;
}

std::array<Area *, 4> Grid::getAdjacentAreas(Vector2f pos) // in order UP, LEFT, DOWN, RIGHT
{
	pos = floorVec(pos);

	std::array<Area *, 4> adjacentAreas;
	adjacentAreas[0] = getArea({pos.x, pos.y - 1});
	adjacentAreas[1] = getArea({pos.x - 1, pos.y});
	adjacentAreas[2] = getArea({pos.x, pos.y + 1});
	adjacentAreas[3] = getArea({pos.x + 1, pos.y});

	return adjacentAreas;
}

void Grid::mergeAreas(Area *into, Area *from)
{
	into->addPositions(from->getPositions());
	for (auto &block : from->getBlocks())
	{
		into->addBlock(block);
		from->removeBlock(block);
	}
	removeArea(from);
}

Area *Grid::connectAreas(Vector2f pos)
{
	pos = floorVec(pos);

	std::array<Area *, 4> adjacent = getAdjacentAreas(pos);
	Area *area = nullptr;

	int first_neighbour;
	// sets first_neighbour to the first index of the adjacent array that isn't nullptr: UP, LEFT, DOWN, RIGHT
	for (first_neighbour = 0; first_neighbour < 4; first_neighbour++)
		if (adjacent.at(first_neighbour))
		{
			area = adjacent.at(first_neighbour);
			break;
		}

	// adds new position to first found neighbour, if exists
	if (area)
		area->addPosition(pos);

	// merges all consecutive neighbours with the first previously found neighbour
	for (int i = first_neighbour + 1; i < 4; i++)
		if (adjacent.at(i))
			mergeAreas(area, adjacent.at(i));

	return area;
}

bool Grid::addArea(Vector2f pos)
{
	if (getArea(pos))
		return false;

	Area *area = connectAreas(pos); // adds position already
	if (!area)
	{
		area = new Area();
		area->addPosition(pos);
		areas_.push_back(area);
	}

	area->addBlock(getBlock(pos));
	removeGlobalBlock(pos);

	area->updateSequence();
	return true;
}

void Grid::splitAreas(Area *area) // TODO: this is probably terrible
{
	std::vector<Vector2f> remaining_pos = area->getPositions();
	std::vector<Area *> new_areas;

	while (!remaining_pos.empty())
	{
		std::vector<Vector2f> connected;
		std::queue<Vector2f> visit_queue;

		visit_queue.push(remaining_pos.at(0));

		while (!visit_queue.empty())
		{
			Vector2f current_pos = visit_queue.front();
			visit_queue.pop();

			if (std::find(connected.begin(), connected.end(), current_pos) != connected.end())
				continue;

			connected.push_back(current_pos);

			std::array<Area *, 4> adjacent_areas = getAdjacentAreas(current_pos);
		}
	}

	area->updateSequence();
}

bool Grid::removeArea(Vector2f pos)
{
	Area *area = getArea(pos);
	if (!area)
		return false;

	area->removePosition(pos);
	splitAreas(area);
	addBlock(area->getBlock(pos));

	if (area->getPositions().empty())
		removeArea(area);

	return true;
}

void Grid::removeArea(Area *area)
{
	areas_.erase(
		std::remove_if(areas_.begin(), areas_.end(),
					   [area](auto a)
					   { return a == area; }),
		areas_.end());

	delete area;
}