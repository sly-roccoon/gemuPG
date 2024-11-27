#pragma once

#include "area.h"

class Grid
{
public:
    static Grid& getInstance()
    {
        static Grid instance;
        return instance;
    }

    void update();
    void addBlock(std::shared_ptr<Block>);
    void removeBlock(Vec2);

private:
    Grid() {} // private constructor to prevent instantiation
    Grid(const Grid&) = delete; // delete copy constructor
    Grid& operator=(const Grid&) = delete; // delete assignment operator

	std::vector<std::unique_ptr<Area>> areas_;
	std::vector<std::shared_ptr<Block>> blocks_;
};

