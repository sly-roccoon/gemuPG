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

private:
    Interface() {}
    Interface(const Interface&) = delete;
    Interface& operator=(const Interface&) = delete;
};
