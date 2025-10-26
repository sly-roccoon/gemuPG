#pragma once

#include "area.h"

#include <SDL3/SDL_pixels.h>
#include <memory>
#include <vector>

class Grid
{
public:
    void draw (SDL_Renderer*);
    void drawBlocks (SDL_Renderer*);
    void drawAreas (SDL_Renderer*);

    void clear();

    Block* addBlock (Block*);
    bool removeBlock (Vector2f);
    void removeBlock (Block*);
    Block* getBlock (Vector2f);
    std::vector<Block*>& getGlobalBlocks() { return blocks_; };
    std::vector<Block*> getBlocks();

    Area* addArea (Vector2f);
    bool removeArea (Vector2f);
    void removeArea (Area*);
    Area* getArea (Vector2f);
    std::vector<Area*>& getAreas() { return areas_; };
    void stepSequence();

    void stopSequence();
    void bypassGenerators (bool);

    bool isAreaAdjacent (Vector2f);
    void copyBlock (Vector2f);
    void pasteBlock (Vector2f);

private:
    bool sequencerExists (Vector2f);
    bool removeSequencer (Vector2f);
    bool removeGlobalBlock (Vector2f);
    void removeGlobalBlock (Block*);
    bool removeAreaBlock (Vector2f);
    void removeAreaBlock (Block*);

    void mergeAreas (Area* into, Area* from);
    Area* connectAreas (Vector2f);
    void splitAreas (Area*);
    std::array<Area*, 4> getAdjacentAreas (Vector2f);

    std::vector<Area*> areas_;
    std::vector<Block*> blocks_;

    std::shared_ptr<Block> copy_block_ = nullptr;

    bool bypass_ = false;
};
