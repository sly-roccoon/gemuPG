#pragma once
#include "interface.h"
#include "util.h"
#include "command.h"

class InputHandler
{
public:
	static InputHandler &getInstance()
	{
		static InputHandler instance;
		return instance;
	}

	void handleEvent(SDL_Event *);

private:
	CommandManager cmd_mgr;
	Camera &camera_;

	void handleKeys();

	void handleCamera();
	void handleEdit();

	void handleUndoRedo();

	void handleMouseWheel(SDL_Event *);
	void handleMouse(SDL_Event *);

	InputHandler() : camera_(Camera::getInstance()) {}
	InputHandler(const InputHandler &) = delete;
	InputHandler &operator=(const InputHandler &) = delete;
};