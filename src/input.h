#pragma once
#include "interface.h"
#include "util.h"
#include "command.h"
#include "save.h"

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
	void updateMousePos(SDL_Event *);
	void handleCamPan(SDL_Event *);
	void handlePlacement(SDL_Event *);
	void handleKeys(SDL_Event *);
	void handleLoseFocus();

	void handleUndoRedo(SDL_Event *);
	void handleSaveLoad(SDL_Event *);
	void handleShortcuts(SDL_Event *);
	void handleCopyPaste(SDL_Event *);

	bool isKeyDown(SDL_Keycode);
	void updateKeys(SDL_Event *);
	std::vector<SDL_Keycode> keys_down_;

	Input() = default;
	Input(const Input &) = delete;
	Input &operator=(const Input &) = delete;

	Vector2f mouse_pos_ = {0.0f, 0.0f};
};