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

	void addBlock(Block *);
	bool removeBlock(Vector2f);
	void removeBlock(Block *);
	Block *getBlock(Vector2f);

	std::vector<Block *> &getBlocks() { return blocks_; };

private:
	std::vector<Area *> areas_;
	std::vector<Block *> blocks_;
};