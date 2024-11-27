#pragma once
#include "block.h"

class Area
{
public:
	auto getPositions() { return positions_; }
	bool isInside(Vec2);
	void addBlock(std::shared_ptr<Block>);
	void removeBlock(std::shared_ptr<Block>);
	void removeBlock(Vec2);
	void setColor(ofColor color) { color_ = color; }
	auto getColor() { return color_; }

private:
	std::vector<Vec2> positions_;
	ofColor color_;
	std::vector < std::shared_ptr<Block> > blocks_;
};