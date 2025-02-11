#include "interface.h"
#include "blockfactory.h"
#include "gui.h"
#include <format>

Interface::Interface() : camera_{Camera::getInstance()}
{
	if (!SDL_Init(SDL_INIT_VIDEO))
		exit(SDL_APP_FAILURE);

	if (!SDL_CreateWindowAndRenderer("GemuPG", width_, height_, 0, &window_, &renderer_))
		exit(SDL_APP_FAILURE);

	SDL_SetWindowResizable(window_, true);
}

void Interface::destroy()
{
	SDL_DestroyRenderer(renderer_);
	SDL_DestroyWindow(window_);
	renderer_, window_ = nullptr;
}

bool Interface::addBlock(Vector2f pos)
{
	BlockFactory &block_factory = BlockFactory::getInstance();

	if (grid_.getBlock(pos) != nullptr)
		return false;

	Block *block = block_factory.createBlock(cur_selection_, pos);
	// grid_.removeBlock(pos); //? shouldn't be necessary, but causes area drawing over blocks if removed??
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

	drawGrid();
	drawAreas();
	drawBlocks();
	drawGUI();

	debug();

	SDL_RenderPresent(renderer_);
}

void Interface::drawGrid()
{
	grid_.draw(renderer_);
}

void Interface::drawAreas()
{
	grid_.drawAreas(renderer_);
}

void Interface::drawBlocks()
{
	grid_.drawBlocks(renderer_);
}

void Interface::drawSequencerGUI()
{
	std::vector<BlockSequencer *> sequencers;
	for (auto area : grid_.getAreas())
		for (auto sequencer : area->getSequence())
			sequencers.push_back(sequencer);

	// remove nullptrs
	sequencers.erase(
		std::remove_if(sequencers.begin(), sequencers.end(),
					   [](BlockSequencer *sequencer)
					   { return sequencer == nullptr; }),
		sequencers.end());

	// remove duplicates
	std::sort(sequencers.begin(), sequencers.end());
	sequencers.erase(
		std::unique(sequencers.begin(), sequencers.end()),
		sequencers.end());

	for (auto sequencer : sequencers)
		if (sequencer)
			sequencer->drawGUI();
}

void Interface::drawGUI()
{
	ImGui_ImplSDLRenderer3_NewFrame();
	ImGui_ImplSDL3_NewFrame();
	ImGui::NewFrame();

	for (auto &block : grid_.getBlocks())
		block->drawGUI(); // TODO: crackling when changing values

	for (auto &area : grid_.getAreas())
		area->drawGUI();

	drawSequencerGUI();

	GUI::drawToolbar();

	ImGui::Render();
	ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer_);
}

void Interface::debug()
{
	SDL_SetRenderDrawColor(renderer_, 0, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderDebugTextFormat(renderer_, 0, 0, "CamPos: %.2f, %.2f", Camera::getPos().x, Camera::getPos().y);
	SDL_RenderDebugTextFormat(renderer_, 0, 20, "Current Selection: %s", cur_selection_ == BLOCK_GENERATOR ? "Generator" : cur_selection_ == AREA		   ? "Area"
																													   : cur_selection_ == BLOCK_SEQUENCER ? "Sequencer"
																																						   : "Unknown");
}