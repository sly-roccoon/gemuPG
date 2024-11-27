#include "interface.h"
#include "blockfactory.h"

void Interface::addBlock(blockType type, Vector2 pos)
{
	BlockFactory& block_factory = BlockFactory::getInstance();
	Grid& grid = Grid::getInstance();
	
	std::shared_ptr<Block> block = block_factory.createBlock(type, pos);
	grid.removeBlock(pos);
	grid.addBlock(block);
} 

void Interface::removeBlock(Vector2 pos)
{
	Grid& grid = Grid::getInstance();
	grid.removeBlock(pos);
}

void Interface::removeBlock(std::shared_ptr<Block> block)
{
	Grid& grid = Grid::getInstance();
	grid.removeBlock(block);
}

//--------------------------------------------------------

void Interface::draw()
{
	DrawGrid(128, 64);
	drawAreas();
	drawBlocks();
	drawGUI();
}

void Interface::drawAreas()
{

}

void Interface::drawBlocks()
{

}

void Interface::drawGUI()
{
	
}