#include "interface.h"
#include "blockfactory.h"
#include <format>

Interface::Interface() : camera_{Camera::getInstance()}
{
	if (!SDL_Init(SDL_INIT_VIDEO))
		exit(SDL_APP_FAILURE);

	if (!SDL_CreateWindowAndRenderer("GemuPG", width_, height_, 0, &window_, &renderer_))
		exit(SDL_APP_FAILURE);
}

bool Interface::addBlock(blockType type, Vector2f pos)
{
	BlockFactory &block_factory = BlockFactory::getInstance();

	if (grid_.getBlock(pos) != nullptr)
		return false;

	Block *block = block_factory.createBlock(type, pos);
	grid_.removeBlock(pos); //? shouldn't be necessary
	grid_.addBlock(block);

	return true;
}

void Interface::addBlock(Block *block)
{
	grid_.addBlock(block);
}

bool Interface::removeBlock(Vector2f pos)
{
	return grid_.removeBlock(pos);
}

void Interface::removeBlock(Block *block)
{
	grid_.removeBlock(block);
}

//--------------------------------------------------------

void Interface::draw()
{
	SDL_SetRenderDrawColor(renderer_, 255, 255, 255, SDL_ALPHA_OPAQUE);
	if (!SDL_RenderClear(renderer_))
		SDL_LogError(SDL_LOG_CATEGORY_RENDER, "FillSurfaceRect");

	grid_.draw(renderer_);

	drawAreas();
	drawBlocks();
	drawGUI();

	debug();

	SDL_RenderPresent(renderer_);
}

void Interface::drawAreas()
{
}

void Interface::drawBlocks()
{
	grid_.drawBlocks(renderer_);
}

void Interface::drawGUI()
{
}

void Interface::debug()
{
	SDL_SetRenderDrawColor(renderer_, 0, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderDebugText(renderer_, 0, 0, std::format("CamPos: {}, {}", std::to_string(Camera::getPos().x), std::to_string(Camera::getPos().y)).c_str());
}