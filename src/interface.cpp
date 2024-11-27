#include "interface.h"

void Interface::addBlock(blockType type, Vec2 pos)
{
	BlockFactory& block_factory = BlockFactory::getInstance();
	Grid& grid = Grid::getInstance();
	
	std::shared_ptr<Block> block = block_factory.createBlock(type, pos);
	grid.removeBlock(pos);
	grid.addBlock(block);
}