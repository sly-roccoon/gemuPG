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

    void addBlock(blockType, Vec2);
    void removeBlock(Vec2);

private:
    Interface() {}
    Interface(const Interface&) = delete;
    Interface& operator=(const Interface&) = delete;
};
