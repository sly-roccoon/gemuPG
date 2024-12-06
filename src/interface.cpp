#include "interface.h"
#include "blockfactory.h"

void Interface::addBlock(blockType type, Vector2 pos)
{
	BlockFactory &block_factory = BlockFactory::getInstance();

	std::shared_ptr<Block> block = block_factory.createBlock(type, pos);
	grid.removeBlock(pos);
	grid.addBlock(block);
}

void Interface::removeBlock(Vector2 pos)
{
	grid.removeBlock(pos);
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
}

void Interface::drawGUI()
{
}