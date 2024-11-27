#include "block.h"
#include "raymath.h"

class BlockFactory
{
public:
    static BlockFactory& getInstance() {
        static BlockFactory instance;
        return instance;
    }

    std::unique_ptr<Block> createBlock(const blockType type, const Vector2 pos)
    {
        switch (type)
        {
        case BLOCK_GENERATOR:
            return std::make_unique<BlockGenerator>(pos);
        default:
            return nullptr;
        }
    }

private:
    BlockFactory() = default;
    BlockFactory(const BlockFactory&) = delete;
    BlockFactory& operator=(const BlockFactory&) = delete;
};