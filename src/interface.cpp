#include "interface.h"
#include "blockfactory.h"

bool Interface::addBlock(blockType type, Vector2 pos)
{
	BlockFactory &block_factory = BlockFactory::getInstance();

	if (grid.getBlock(pos) != nullptr)
		return false;

	std::shared_ptr<Block> block = block_factory.createBlock(type, pos);
	grid.removeBlock(pos); //? shouldn't be necessary
	grid.addBlock(block);

	return true;
}

void Interface::addBlock(std::shared_ptr<Block> block)
{
	grid.addBlock(block);
}

bool Interface::removeBlock(Vector2 pos)
{
	return grid.removeBlock(pos);
}

void Interface::removeBlock(std::shared_ptr<Block> block)
{
	grid.removeBlock(block);
}

//--------------------------------------------------------

void Interface::draw()
{
	BeginMode2D(camera);
	{
		drawGrid();
		drawAreas();
		drawBlocks();
		drawGUI();
	}
	EndMode2D();
}

void Interface::drawGrid()
{
	grid.draw();
}

void Interface::drawAreas()
{
}

void Interface::drawBlocks()
{
	grid.drawBlocks();
}

void Interface::drawGUI()
{
}