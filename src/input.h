#pragma once
#include "interface.h"
#include "util.h"
#include "command.h"

class Input
{
public:
	static Input &getInstance()
	{
		static Input instance;
		return instance;
	}

	void handleEvent(SDL_Event *);

private:
	CommandManager cmd_mgr;

	void handleMouseWheel(SDL_Event *);
	void handleMouse(SDL_Event *);
	void handleKeys(SDL_Event *);
	void handleLoseFocus();

	void handleUndoRedo(SDL_Event *);

	bool isKeyDown(SDL_Keycode);
	void updateKeys(SDL_Event *);
	std::vector<SDL_Keycode> keys_down_;

	Input() = default;
	Input(const Input &) = delete;
	Input &operator=(const Input &) = delete;
};