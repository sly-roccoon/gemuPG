#pragma once
#include "block.h"
#include "raylib.h"
#include <vector>

class Area
{
public:
	auto getPositions() { return positions_; }
	bool isInside(Vector2);
	void addBlock(std::shared_ptr<Block>);
	void removeBlock(std::shared_ptr<Block>);
	void removeBlock(Vector2);
	void setColor(Color color) { color_ = color; }
	auto getColor() { return color_; }
	auto getBlocks() { return blocks_; }

private:
	std::vector<Vector2> positions_;
	Color color_;
	std::vector<std::shared_ptr<Block>> blocks_;
};