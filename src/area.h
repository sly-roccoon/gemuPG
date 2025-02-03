#pragma once
#include "block.h"
#include <SDL3/SDL.h>
#include "util.h"
#include <vector>

class Area
{
public:
    auto getPositions() { return positions_; }
    void addPositions(std::vector<Vector2f>);
    bool addPosition(Vector2f);
    void removePosition(Vector2f);
    bool isInside(Vector2f);

    void addBlock(Block *);
    void removeBlock(Block *);
    bool removeBlock(Vector2f);
    auto getBlocks() { return blocks_; }
    Block *getBlock(Vector2f);

    void addSequencer(BlockSequencer *);
    void removeSequencer(BlockSequencer *);
    void updateSequence();
    bool sequencerExists(Vector2f);
    std::vector<BlockSequencer *> getSequencer() { return sequence_; }

private:
    void removeDanglingSequencers();

    std::vector<Vector2f> positions_;
    std::vector<Block *> blocks_;
    std::vector<BlockSequencer *> sequence_;
};