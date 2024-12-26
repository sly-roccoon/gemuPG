#include "block.h"
#include "util.h"

class BlockFactory
{
public:
    static BlockFactory &getInstance()
    {
        static BlockFactory instance;
        return instance;
    }

    Block *createBlock(const blockType type, const Vector2f pos)
    {
        switch (type)
        {
        case BLOCK_GENERATOR:
            return new BlockGenerator(pos);
        default:
            return nullptr;
        }
    }

private:
    BlockFactory() = default;
    BlockFactory(const BlockFactory &) = delete;
    BlockFactory &operator=(const BlockFactory &) = delete;
};