#include "interface.h"
#include "blockfactory.h"
#include "gui.h"
#include "text.h"
#include "assets/icon.bmp.h"
#include <format>

Interface::Interface() : camera_{Camera::getInstance()}
{
	if (!SDL_Init(SDL_INIT_VIDEO))
		exit(SDL_APP_FAILURE);

	if (!SDL_CreateWindowAndRenderer(TITLE, width_, height_, 0, &window_, &renderer_))
		exit(SDL_APP_FAILURE);

	if (!TTF_Init())
		exit(SDL_APP_FAILURE);

	Text::init(renderer_);

	// icon_ = SDL_LoadBMP("icon.bmp");
	SDL_IOStream *rw = SDL_IOFromConstMem(icon_bmp, icon_bmp_len);
	icon_ = SDL_LoadBMP_IO(rw, 1);
	SDL_SetWindowIcon(window_, icon_);
	SDL_DestroySurface(icon_);
	SDL_SetWindowResizable(window_, true);
}

void Interface::updateScaling()
{
	RENDER_SCALE = SDL_GetWindowDisplayScale(window_);
	ImGui::GetIO().FontGlobalScale = RENDER_SCALE;
}

void Interface::toggleFullscreen()
{
	is_fullscreen_ = !is_fullscreen_;

	SDL_SetWindowFullscreen(window_, is_fullscreen_);
}

void Interface::togglePlayPause()
{
	is_playing_ = !is_playing_;

	Clock::getInstance().setRunning(is_playing_);
	Interface::getInstance().getGrid().bypassGenerators(!is_playing_);
}

void Interface::setPlaying(bool playing)
{
	is_playing_ = playing;

	Clock::getInstance().setRunning(is_playing_);
	Interface::getInstance().getGrid().bypassGenerators(!is_playing_);
}

void Interface::updateTitle(std::string path)
{
	if (path.empty())
		SDL_SetWindowTitle(window_, TITLE);
	else
		SDL_SetWindowTitle(window_, std::format("{} [{}]", TITLE, path).c_str());
}

void Interface::stop()
{
	setPlaying(false);

	grid_.stopSequence();
}

void Interface::destroy()
{
	SDL_DestroyRenderer(renderer_);
	SDL_DestroyWindow(window_);
	SDL_DestroySurface(icon_);
}

bool Interface::addBlock(Vector2f pos)
{
	pos = floorVec(pos);
	BlockFactory &block_factory = BlockFactory::getInstance();

	if (grid_.getBlock(pos) != nullptr)
		return false;

	Block *block = block_factory.createBlock(cur_selection_, pos);
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

void Interface::updateBlockTimes()
{
	for (auto area : grid_.getAreas())
	{
		area->updateNoteLength();
		area->updateEnvelope();
		area->updateGlissando();
	}
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

	if (DRAW_DEBUG)
		drawDebug();

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
					   {
						   return sequencer == nullptr;
					   }),
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

void Interface::drawBlockSelection()
{
	SDL_FRect rect = {
		ICON_SIZE * RENDER_SCALE / 8, ICON_SIZE * RENDER_SCALE / 8, ICON_SIZE * RENDER_SCALE, ICON_SIZE * RENDER_SCALE};
	SDL_Color col = {};

	switch (cur_selection_)
	{
	case AREA:
		col = AREA_COLOR;
		break;
	case BLOCK_GENERATOR:
		col = GENERATOR_COLOR;
		break;
	case BLOCK_SEQUENCER:
		col = SEQUENCER_COLOR;
		break;
	}
	SDL_SetRenderDrawColor(renderer_, col.r, col.g, col.b, col.a);
	SDL_RenderFillRect(renderer_, &rect);
}

void Interface::drawGUI()
{
	ImGui_ImplSDLRenderer3_NewFrame();
	ImGui_ImplSDL3_NewFrame();
	ImGui::NewFrame();

	for (auto &block : grid_.getBlocks())
		if (block->getType() == BLOCK_GENERATOR)
			block->drawGUI(); // TODO: crackling when changing values

	for (auto &area : grid_.getAreas())
		area->drawGUI();

	drawSequencerGUI();

	drawBlockSelection();

	GUI::drawToolbar();

	GUI::drawOutput(AudioEngine::getInstance().getOutput());

	ImGui::Render();
	ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer_);
}

void Interface::closeAllWindows()
{
	for (auto &area : grid_.getAreas())
		area->setGUI(false);

	for (auto block : grid_.getBlocks())
		block->setGUI(false);
}

void Interface::drawDebug()
{
	int w;
	SDL_GetWindowSizeInPixels(window_, &w, NULL);
	SDL_SetRenderDrawColor(renderer_, 0, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderDebugTextFormat(renderer_, w - 2 * SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE, 0, "%d",
							  (int)Clock::getInstance().getFPS());
}

//--------------------------------------------------------
