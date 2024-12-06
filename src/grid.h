#pragma once

#include "area.h"
#include <memory>
#include <vector>
#include <algorithm>

constexpr int GRID_SIZE = 256;
constexpr int GRID_SPACING = 64;

class Grid
{
public:
    void update();
    void draw();

    void addBlock(std::shared_ptr<Block>);
    void removeBlock(Vector2);
    void removeBlock(std::shared_ptr<Block>);

private:
    std::vector<std::unique_ptr<Area>> areas_;
    std::vector<std::shared_ptr<Block>> blocks_;
};