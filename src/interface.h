#pragma once
#include "grid.h"
#include "util.h"

class Interface
{
public:
    static Interface &getInstance()
    {
        static Interface instance;
        return instance;
    }

    bool addBlock(blockType, Vector2);
    void addBlock(std::shared_ptr<Block>);
    bool removeBlock(Vector2);
    void removeBlock(std::shared_ptr<Block>);
    void draw();
    Camera2D &getCamera() { return camera; }
    Grid &getGrid() { return grid; }
    std::shared_ptr<Block> getBlock(Vector2 pos) { return grid.getBlock(pos); }

private:
    Interface() : grid{}
    {
        camera.zoom = DEFAULT_CAMERA_ZOOM;
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
