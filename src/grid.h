#pragma once

#include "area.h"
#include <memory>
#include <vector>
#include <algorithm>

constexpr int GRID_SIZE = 512;
constexpr int GRID_SPACING = 1;
constexpr Color GRID_COLOR = LIGHTGRAY;

class Grid
{
public:
    void update();
    void draw();
    void drawBlocks();

    void addBlock(std::shared_ptr<Block>);
    bool removeBlock(Vector2);
    void removeBlock(std::shared_ptr<Block>);
    std::shared_ptr<Block> getBlock(Vector2);

    std::vector<std::shared_ptr<Block>> &getBlocks() { return blocks_; };

private:
    std::vector<std::unique_ptr<Area>> areas_;
    std::vector<std::shared_ptr<Block>> blocks_;
};