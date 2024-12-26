#pragma once
#include <SDL3/SDL.h>
#include "grid.h"
#include "camera.h"

constexpr unsigned int WIDTH = 1280;
constexpr unsigned int HEIGHT = 720;

class Interface
{
public:
	static Interface &getInstance()
	{
		static Interface instance;
		return instance;
	}

	bool addBlock(blockType, Vector2f);
	void addBlock(Block *);
	bool removeBlock(Vector2f);
	void removeBlock(Block *);
	void draw();
	Grid &getGrid() { return grid_; }
	Block *getBlock(Vector2f pos) { return grid_.getBlock(pos); }

private:
	Interface();
	Interface(const Interface &) = delete;
	Interface &operator=(const Interface &) = delete;

	unsigned int width_ = WIDTH;
	unsigned int height_ = HEIGHT;

	Camera &camera_;
	Grid grid_{};

	SDL_Window *window_ = nullptr;
	SDL_Renderer *renderer_ = nullptr;

	void drawGrid();
	void drawAreas();
	void drawBlocks();
	void drawGUI();
	void debug();
};
