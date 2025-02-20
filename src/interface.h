#pragma once
#include <SDL3/SDL.h>
#include "grid.h"
#include "camera.h"
#include "audio.h"

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
	void closeAllWindows();
	Grid &getGrid() { return grid_; }
	Block *getBlock(Vector2f pos) { return grid_.getBlock(pos); }

	void updateBlockTimes();

	void setSelection(block_type_t selection) { cur_selection_ = selection; }
	block_type_t getSelection() { return cur_selection_; }

	void toggleFullscreen();
	void togglePlayPause();
	void setPlaying(bool playing);
	void updateTitle(std::string path);
	static void updateScaling();

	void stop();

private:
	Interface();
	Interface(const Interface &) = delete;
	Interface &operator=(const Interface &) = delete;

	unsigned int width_ = DEFAULT_WIDTH;
	unsigned int height_ = DEFAULT_HEIGHT;
	bool is_fullscreen_ = false;

	block_type_t cur_selection_ = AREA;
	bool is_playing_ = true;

	Camera &camera_;
	Grid grid_{};

	inline static SDL_Window *window_ = nullptr;
	SDL_Renderer *renderer_ = nullptr;

	SDL_Surface *icon_;

	void drawGrid();
	void drawAreas();
	void drawBlocks();
	void drawSequencerGUI();
	void drawBlockSelection();
	void drawGUI();
	void drawDebug();
};
