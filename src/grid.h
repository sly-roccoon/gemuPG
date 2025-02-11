#pragma once

#include "area.h"
#include "camera.h"

#include <vector>
#include <algorithm>
#include <SDL3/SDL_pixels.h>

class Grid
{
public:
	void update();
	void draw(SDL_Renderer *);
	void drawBlocks(SDL_Renderer *);
	void drawAreas(SDL_Renderer *);

	void addBlock(Block *);
	bool removeBlock(Vector2f);
	void removeBlock(Block *);
	Block *getBlock(Vector2f);
	std::vector<Block *> &getGlobalBlocks() { return blocks_; };
	std::vector<Block *> getBlocks();

	bool addArea(Vector2f);
	bool removeArea(Vector2f);
	void removeArea(Area *);
	Area *getArea(Vector2f);
	std::vector<Area *> &getAreas() { return areas_; };
	void stepSequence();

	void stopSequence();
	void bypassGenerators(bool);

	bool isAreaAdjacent(Vector2f);

private:
	bool sequencerExists(Vector2f);
	bool removeSequencer(Vector2f);
	bool removeGlobalBlock(Vector2f);
	void removeGlobalBlock(Block *);
	bool removeAreaBlock(Vector2f);
	void removeAreaBlock(Block *);

	void mergeAreas(Area *into, Area *from);
	Area *connectAreas(Vector2f);
	void splitAreas(Area *);
	std::array<Area *, 4> getAdjacentAreas(Vector2f);

	std::vector<Area *> areas_;
	std::vector<Block *> blocks_;
};