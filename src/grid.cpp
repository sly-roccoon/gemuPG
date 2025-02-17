#include "grid.h"
#include "text.h"
#include <array>
#include <format>
#include <queue>
#include "util.h"

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

void drawBlockText(Block *block, std::string type, std::string value, SDL_Color color)
{
	SDL_FRect *rect = block->getFRect();
	Vector2f value_size = {rect->w, rect->h / TEXT_VALUE_SIZE_DIV};
	Vector2f type_size = {rect->w, rect->h / TEXT_TYPE_SIZE_DIV};

	if (type != block->getText(0))
	{
		block->setTextTexture(Text::getTexture(type, color), 0);
		block->setText(type, 0);
	}

	if (value != block->getText(1))
	{
		block->setTextTexture(Text::getTexture(value, color), 1);
		block->setValueTextSize(Text::getTextSize(value));
		block->setText(value, 1);
	}

	Vector2f value_pos = {rect->x + rect->w / 2,
						  rect->y + rect->h / 2};
	Text::drawTexture(block->getTextTexture(0), {rect->x, rect->y}, type_size);
	Text::drawTexture(block->getTextTexture(1), value_pos, value_size, true);
}

void drawGenerator(SDL_Renderer *renderer, BlockGenerator *block)
{
	SDL_SetRenderDrawColor(renderer, GENERATOR_COLOR.r, GENERATOR_COLOR.g, GENERATOR_COLOR.b, GENERATOR_COLOR.a);
	SDL_RenderFillRect(renderer, block->getFRect());

	std::string type_text = "";
	std::string value_text = "";

	if (block->isInArea())
		type_text = std::format("{:.2f} * (x + {:.2f}) Hz", block->getFreqFactor(), block->getRelFreq());
	else
		type_text = std::format("{:.2f} Hz", block->getFrequency());

	switch (block->getWave())
	{
	case WAVE_SINE:
		value_text = "SINE";
		break;
	case WAVE_SQUARE:
		value_text = "SQUARE";
		break;
	case WAVE_SAW:
		value_text = "SAW";
		break;
	case WAVE_TRIANGLE:
		value_text = "TRIANG";
		break;
	default:
		break;
	}

	drawBlockText(block, type_text, value_text, invertColor(GENERATOR_COLOR));
}

void drawSequencer(SDL_Renderer *renderer, BlockSequencer *block)
{
	if (!block)
		return;

	SDL_SetRenderDrawColor(renderer, SEQUENCER_COLOR.r, SEQUENCER_COLOR.g, SEQUENCER_COLOR.b, SEQUENCER_COLOR.a);
	SDL_RenderFillRect(renderer, block->getFRect());

	std::string type_text = "";
	std::string value_text = "";

	switch (block->getPitchType())
	{
	case PITCH_ABS_FREQUENCY:
	{
		type_text = "absolute";
		value_text = std::format("{:.2f} Hz", block->getPitch());
		break;
	}
	case PITCH_REL_FREQUENCY:
	{
		type_text = "relative";
		value_text = std::format("{:.2f} Hz", block->getPitch());
		break;
	}
	case PITCH_INTERVAL:
	{
		type_text = "interval";
		auto interval = block->getInterval();
		value_text = std::format("{:.1f} / {:.1f}", interval.first, interval.second);
		break;
	}
	case PITCH_NOTE:
	{
		type_text = "note";
		auto note = freqToNote(block->getPitch());
		value_text = std::format("{}{}", note.first, note.second);
		break;
	}
	}

	drawBlockText(block, type_text, value_text, invertColor(SEQUENCER_COLOR));
}

void Grid::drawBlocks(SDL_Renderer *renderer)
{
	SDL_Rect render_rect;
	SDL_FRect render_frect;
	SDL_GetRenderViewport(renderer, &render_rect);
	SDL_RectToFRect(&render_rect, &render_frect);

	for (auto block : blocks_)
		if (block->getType() == BLOCK_GENERATOR)
			if (SDL_HasRectIntersectionFloat(block->getFRect(), &render_frect))
				drawGenerator(renderer, (BlockGenerator *)block);

	for (auto &area : areas_)
	{
		for (auto &block : area->getBlocks())
			if (SDL_HasRectIntersectionFloat(block->getFRect(), &render_frect))
				drawGenerator(renderer, (BlockGenerator *)block);

		for (auto &sequencer : area->getSequence())
			if (sequencer)
				if (SDL_HasRectIntersectionFloat(sequencer->getFRect(), &render_frect))
					drawSequencer(renderer, sequencer);
	}
}

void Grid::drawAreas(SDL_Renderer *renderer)
{
	SDL_SetRenderDrawColor(renderer, AREA_COLOR.r, AREA_COLOR.g, AREA_COLOR.b, AREA_COLOR.a);
	SDL_Rect render_rect;
	SDL_FRect render_frect;
	SDL_GetRenderViewport(renderer, &render_rect);
	SDL_RectToFRect(&render_rect, &render_frect);
	for (auto &area : areas_)
	{
		for (auto &pos : area->getPositions())
		{
			SDL_FRect rect = {Camera::worldToScreen(pos).x, Camera::worldToScreen(pos).y, Camera::getZoom(), Camera::getZoom()};
			if (SDL_HasRectIntersectionFloat(&rect, &render_frect))
				SDL_RenderFillRect(renderer, &rect);
		}
	}
}

Block *Grid::addBlock(Block *block)
{
	if (!block)
		return nullptr;

	for (auto &area : areas_)
		if (area->getSequencer(block->getPos()))
		{
			return nullptr;
		}

	if (block->getType() == BLOCK_GENERATOR)
		((BlockGenerator *)block)->setBypass(bypass_);

	Area *area = getArea(block->getPos());
	if (area)
	{
		return area->addBlock(block);
	}

	if (block->getType() == BLOCK_SEQUENCER)
	{
		for (auto area : getAdjacentAreas(block->getPos()))
			if (area)
				return area->addSequencer((BlockSequencer *)block);

		return nullptr;
	}

	blocks_.push_back(std::move(block));
	block->setInArea(false);
	return block;
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

	if (removeAreaBlock(pos))
		return true;

	return removeSequencer(pos);
}

bool Grid::sequencerExists(Vector2f pos)
{
	for (auto &area : areas_)
		if (area->getSequencer(pos))
			return true;

	return false;
}

bool Grid::removeSequencer(Vector2f pos)
{
	bool removed = false;
	for (Area *area : areas_)
		if (area->removeSequencer(pos))
			removed = true;

	return removed;
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

		block = area->getSequencer(pos);
		if (block)
			return block;
	}

	return nullptr;
}

void Grid::clear()
{
	for (auto &area : areas_)
		removeArea(area);

	for (auto &block : blocks_)
		removeBlock(block);
}

std::vector<Block *> Grid::getBlocks()
{
	std::vector<Block *> blocks = blocks_;
	for (auto &area : areas_)
	{
		auto area_blocks = area->getBlocks();
		blocks.insert(std::end(blocks), std::begin(area_blocks), std::end(area_blocks));

		auto sequence = area->getSequence();
		// remove nullptrs
		sequence.erase(
			std::remove_if(sequence.begin(), sequence.end(),
						   [](BlockSequencer *sequencer)
						   { return sequencer == nullptr; }),
			sequence.end());

		// remove duplicates
		std::sort(sequence.begin(), sequence.end());
		sequence.erase(
			std::unique(sequence.begin(), sequence.end()),
			sequence.end());

		blocks.insert(std::end(blocks), std::begin(sequence), std::end(sequence));
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

bool Grid::isAreaAdjacent(Vector2f pos)
{
	pos = floorVec(pos);

	std::array<Area *, 4> adjacent = getAdjacentAreas(pos);
	for (auto &area : adjacent)
		if (area)
			return true;

	return false;
}

std::array<Area *, 4> Grid::getAdjacentAreas(Vector2f pos) // in order UP, LEFT, DOWN, RIGHT
{
	pos = floorVec(pos);

	auto adjacent_pos = getAdjacentPositions(pos);
	std::array<Area *, 4> adjacent_areas;
	for (int i = 0; i < 4; i++)
		adjacent_areas[i] = getArea(adjacent_pos.at(i));

	return adjacent_areas;
}

void Grid::mergeAreas(Area *into, Area *from)
{
	if (!from || into == from)
		return;

	into->addPositions(from->getPositions());
	for (auto &block : from->getBlocks())
	{
		into->addBlock(block);
		from->removeBlock(block);
	}

	for (auto &sequencer : from->getSequence())
	{
		if (!sequencer)
			continue;
		into->addSequencer(sequencer);
		sequencer->addArea(into);
		from->removeSequencer(sequencer);
		sequencer->removeArea(from);
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

Area *Grid::addArea(Vector2f pos)
{
	if (getArea(pos))
		return nullptr;

	for (auto area : areas_)
		if (area->getSequencer(pos))
			return nullptr;

	Area *area = connectAreas(pos); // adds position already
	if (!area)
	{
		area = new Area();
		area->addPosition(pos);
		areas_.push_back(area);
	}

	for (auto adj_pos : getAdjacentPositions(pos))
		if (sequencerExists(adj_pos))
			area->addSequencer((BlockSequencer *)getBlock(adj_pos));

	area->addBlock(getBlock(pos));
	removeGlobalBlock(pos);

	area->updateSequence();
	return area;
}

void Grid::splitAreas(Area *area) // TODO: this is probably terrible
{
	std::vector<Vector2f> remaining_pos = area->getPositions();

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

			std::array<Vector2f, 4> adjacent_positions = {
				Vector2f{current_pos.x, current_pos.y - 1},
				Vector2f{current_pos.x - 1, current_pos.y},
				Vector2f{current_pos.x, current_pos.y + 1},
				Vector2f{current_pos.x + 1, current_pos.y}};

			for (auto &adj_pos : adjacent_positions)
				if (area->isInside(adj_pos))
					visit_queue.push(adj_pos);
		}

		Area *new_area = new Area();

		for (auto &pos : connected)
		{
			remaining_pos.erase(std::remove(remaining_pos.begin(), remaining_pos.end(), pos), remaining_pos.end());
			Block *block = area->getBlock(pos);
			area->removePosition(pos);
			new_area->addPosition(pos);
			if (block)
				new_area->addBlock(block);
		}
		for (auto sequencer : area->getSequence())
		{
			if (!sequencer)
				continue;
			new_area->addSequencer(sequencer);
		}

		new_area->updateSequence();
		areas_.push_back(new_area);
	}

	removeArea(area);
}

bool Grid::removeArea(Vector2f pos)
{
	Area *area = getArea(pos);
	if (!area)
		return false;

	area->removePosition(pos);
	addBlock(area->getBlock(pos));
	splitAreas(area);

	return true;
}

void Grid::removeArea(Area *area)
{
	areas_.erase(
		std::remove_if(areas_.begin(), areas_.end(),
					   [area](auto a)
					   { return a == area; }),
		areas_.end());

	// delete area; TODO: why
}

void Grid::stepSequence()
{
	for (auto &area : areas_)
		area->stepSequence();
}

void Grid::stopSequence()
{
	for (auto &area : areas_)
		area->stopSequence();
}

void Grid::bypassGenerators(bool bypass)
{
	bypass_ = bypass;

	for (auto &block : blocks_)
		if (block->getType() == BLOCK_GENERATOR)
			((BlockGenerator *)block)->setBypass(bypass);

	for (auto &area : areas_)
		for (auto &block : area->getBlocks())
			if (block->getType() == BLOCK_GENERATOR)
				((BlockGenerator *)block)->setBypass(bypass);
}