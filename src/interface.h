#pragma once
#include "grid.h"

class Interface
{
public:
    static Interface &getInstance()
    {
        static Interface instance;
        return instance;
    }

    void addBlock(blockType, Vector2);
    void removeBlock(Vector2);
    void removeBlock(std::shared_ptr<Block>);
    void draw();
    Camera2D &getCamera() { return camera; }

private:
    Interface() : grid{}
    {
        camera.zoom = 1.0f;
        camera.offset = {0.0f, 0.0f};
        camera.rotation = 0.0f;
    }
    Interface(const Interface &) = delete;
    Interface &operator=(const Interface &) = delete;

    Camera2D camera = {};
    Grid grid;

    void drawGrid();
    void drawAreas();
    void drawBlocks();
    void drawGUI();
};
