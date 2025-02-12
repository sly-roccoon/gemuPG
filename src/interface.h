#pragma once
#include <SDL3/SDL.h>
#include "grid.h"
#include "camera.h"

class Interface
{
public:
	static Interface &getInstance()
	{
		static Interface instance;
		return instance;
	}
	void destroy();

	SDL_Window *getWindow() { return window_; }
	SDL_Renderer *getRenderer() { return renderer_; }

	bool addBlock(Vector2f);
	void addBlock(Block *);
	bool removeBlock(Vector2f);
	void removeBlock(Block *);
	void draw();
	Grid &getGrid() { return grid_; }
	Block *getBlock(Vector2f pos) { return grid_.getBlock(pos); }

	void setSelection(block_type_t selection) { cur_selection_ = selection; }
	block_type_t getSelection() { return cur_selection_; }

	void toggleFullscreen();
	void togglePlayPause();

private:
	Interface();
	Interface(const Interface &) = delete;
	Interface &operator=(const Interface &) = delete;

	unsigned int width_ = DEFAULT_WIDTH;
	unsigned int height_ = DEFAULT_HEIGHT;
	bool is_fullscreen_ = false;

	block_type_t cur_selection_ = BLOCK_GENERATOR;
	bool is_playing_ = true;

	Camera &camera_;
	Grid grid_{};

	SDL_Window *window_ = nullptr;
	SDL_Renderer *renderer_ = nullptr;

	void drawGrid();
	void drawAreas();
	void drawBlocks();
	void drawSequencerGUI();
	void drawGUI();
	void debug();
};
