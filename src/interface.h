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

	bool addBlock(blockType, Vector2f);
	void addBlock(Block *);
	bool removeBlock(Vector2f);
	void removeBlock(Block *);
	void draw();
	Grid &getGrid() { return grid_; }
	Block *getBlock(Vector2f pos) { return grid_.getBlock(pos); }

	void setSelection(selectionType selection) { cur_selection_ = selection; }
	selectionType getSelection() { return cur_selection_; }

private:
	Interface();
	Interface(const Interface &) = delete;
	Interface &operator=(const Interface &) = delete;

	unsigned int width_ = WIDTH;
	unsigned int height_ = HEIGHT;

	selectionType cur_selection_ = SELECT_GENERATOR;

	Camera &camera_;
	Grid grid_{};

	SDL_Window *window_ = nullptr;
	SDL_Renderer *renderer_ = nullptr;

	void drawGrid();
	void drawAreas();
	void drawBlocks();
	void drawGUI();
	void drawToolbar();
	void debug();
};
