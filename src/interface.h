#pragma once
#include "grid.h"

class Interface
{
public:
    static Interface& getInstance()
    {
        static Interface instance;
        return instance;
    }

    void addBlock(blockType, Vector2);
    void removeBlock(Vector2);
    void removeBlock(std::shared_ptr<Block>);
    void draw();

private:
    Interface() {}
    Interface(const Interface&) = delete;
    Interface& operator=(const Interface&) = delete;

    void drawAreas();
    void drawBlocks();
    void drawGUI();
};
