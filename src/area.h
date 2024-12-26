#pragma once
#include "block.h"
#include <SDL3/SDL.h>
#include "util.h"
#include <vector>

class Area
{
public:
    auto getPositions() { return positions_; }
    bool isInside(Vector2f);
    void addBlock(Block *);
    void removeBlock(Block *);
    void removeBlock(Vector2f);
    void setColor(SDL_Color color) { color_ = color; }
    auto getColor() { return color_; }
    auto getBlocks() { return blocks_; }

private:
    std::vector<Vector2f> positions_;
    SDL_Color color_;
    std::vector<Block *> blocks_;
};