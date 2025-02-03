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

    Block *createBlock(const block_type_t type, const Vector2f pos)
    {
        switch (type)
        {
        case BLOCK_GENERATOR:
        {
            auto blocks = Interface::getInstance().getGrid().getBlocks();
            for (auto block : blocks)
            {
                if (block->getType() == BLOCK_GENERATOR)
                {
                    float phase = ((BlockGenerator *)block)->getData().phase;
                    return new BlockGenerator(pos, phase);
                }
            }
            return new BlockGenerator(pos);
        }
        case BLOCK_SEQUENCER:
        {
            if (Interface::getInstance().getGrid().isAreaAdjacent(pos) && !Interface::getInstance().getGrid().getArea(pos))
                return new BlockSequencer(pos);
            return nullptr;
        }
        default:
            return nullptr;
        }
    }

private:
    BlockFactory() = default;
    BlockFactory(const BlockFactory &) = delete;
    BlockFactory &operator=(const BlockFactory &) = delete;
};