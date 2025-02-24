#include "input.h"
#include "gui.h"
#include <cstdio>

Vector2f getRendererSize()
{
	int w;
	int h;
	SDL_GetRenderOutputSize(Interface::getInstance().getRenderer(), &w, &h);
	return {static_cast<float>(w), static_cast<float>(h)};
}

void Input::handleEvent(SDL_Event *event)
{
	updateKeys(event);
	ImGui_ImplSDL3_ProcessEvent(event);

	if (!ImGui::GetIO().WantCaptureMouse)
		switch (event->type)
		{
		case SDL_EVENT_MOUSE_WHEEL:
			handleMouseWheel(event);
			break;
		case SDL_EVENT_MOUSE_BUTTON_DOWN:
		case SDL_EVENT_MOUSE_BUTTON_UP:
		case SDL_EVENT_MOUSE_MOTION:
			handleMouse(event);
			break;
		}

	if (!ImGui::GetIO().WantTextInput)
		switch (event->type)
		{
		case SDL_EVENT_KEY_DOWN:
		case SDL_EVENT_KEY_UP:
			handleKeys(event);
			break;
		case SDL_EVENT_WINDOW_FOCUS_LOST:
			handleLoseFocus();
			break;
		}
}

void Input::updateKeys(SDL_Event *event)
{
	if (event->type != SDL_EVENT_KEY_DOWN && event->type != SDL_EVENT_KEY_UP)
		return;

	if (event->type == SDL_EVENT_KEY_DOWN && !isKeyDown(event->key.key))
	{
		keys_down_.push_back(event->key.key);
	}
	else if (event->type == SDL_EVENT_KEY_UP)
	{
		auto it = std::find(keys_down_.begin(), keys_down_.end(), event->key.key);
		if (it != keys_down_.end())
			keys_down_.erase(it);
	}
}

bool Input::isKeyDown(SDL_Keycode key)
{
	return std::find(keys_down_.begin(), keys_down_.end(), key) != keys_down_.end();
}

void Input::handleMouseWheel(SDL_Event *event)
{
	SDL_MouseWheelEvent wheel = event->wheel;
	if (!wheel.y)
		return;

	Vector2f mouse_pos = {wheel.mouse_x, wheel.mouse_y};

	float scale_factor = 1.0f + (0.25f * fabsf(wheel.y));
	if (wheel.y < 0)
		scale_factor = 1.0f / scale_factor;

	Camera::zoomAtPos(mouse_pos, scale_factor, getRendererSize());
}

void Input::handleCamPan(SDL_Event *event)
{
	static bool isDragging;
	static Vector2f delta;

	if (event->type == SDL_EVENT_MOUSE_MOTION)
	{
		delta = {event->motion.xrel, event->motion.yrel};
	}

	SDL_MouseButtonEvent button = event->button;
	if (button.button == SDL_BUTTON_MIDDLE || (button.button == SDL_BUTTON_LEFT && (isKeyDown(SDLK_LCTRL) || isKeyDown(SDLK_RCTRL))))
	{
		if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN)
			isDragging = true;
		if (event->type == SDL_EVENT_MOUSE_BUTTON_UP)
			isDragging = false;
	}
	if (isDragging)
	{

		Camera::setPos(Camera::getPos() + delta * -1.0f / Camera::getZoom(), getRendererSize());
	}
}

void Input::handlePlacement(SDL_Event *event)
{
	SDL_MouseButtonEvent button = event->button;
	static Vector2f mouse_pos;
	static Vector2f world_pos;
	mouse_pos = {button.x, button.y};
	world_pos = Camera::screenToWorld(mouse_pos);
	Grid &grid = Interface::getInstance().getGrid();
	std::unique_ptr<Command> cmd;
	block_type_t cur_selection = Interface::getInstance().getSelection();

	if (button.button == SDL_BUTTON_LEFT && !isKeyDown(SDLK_LCTRL))
	{
		if (cur_selection == AREA) // draw area
		{
			Area *area_at_mouse = grid.getArea(world_pos);
			if (area_at_mouse && button.down)
			{
				area_at_mouse->toggleGUI();
				return;
			}
			cmd = std::make_unique<AddAreaCommand>(world_pos);
		}
		else // place block
		{
			Block *block_at_mouse = grid.getBlock(world_pos);
			if (block_at_mouse && button.down)
			{
				block_at_mouse->toggleGUI();
				return;
			}
			cmd = std::make_unique<AddBlockCommand>(world_pos);
		}
	}

	if (button.button == SDL_BUTTON_RIGHT)
	{
		if (cur_selection == AREA) // remove area
		{
			cmd = std::make_unique<RemoveAreaCommand>(world_pos);
		}
		else // remove block
		{
			cmd = std::make_unique<RemoveBlockCommand>(world_pos);
		}
	}

	if (cmd)
		cmd_mgr.executeCommand(std::move(cmd));
}

void Input::handleMouse(SDL_Event *event)
{
	updateMousePos(event);
	handleCamPan(event);
	handlePlacement(event);
}

void Input::updateMousePos(SDL_Event *event)
{
	if (event->type != SDL_EVENT_MOUSE_MOTION)
		return;

	SDL_MouseMotionEvent motion = event->motion;
	mouse_pos_ = {motion.x, motion.y};
}

void Input::handleKeys(SDL_Event *event)
{
	handleShortcuts(event);
	handleCopyPaste(event);
	handleSaveLoad(event);
	// handleUndoRedo(event); //doesn't work for block parameter changes, no time to add :)
}

void Input::handleLoseFocus()
{
	keys_down_.clear();
}

void Input::handleUndoRedo(SDL_Event *event)
{
	SDL_KeyboardEvent key = event->key;

	if (isKeyDown(SDLK_LCTRL) || isKeyDown(SDLK_RCTRL))
		if (key.key == SDLK_Z && key.type == SDL_EVENT_KEY_DOWN)
			cmd_mgr.undo();
		else if (key.key == SDLK_Y && key.type == SDL_EVENT_KEY_DOWN)
			cmd_mgr.redo();
}

void Input::handleSaveLoad(SDL_Event *event)
{
	SDL_KeyboardEvent key = event->key;

	if (isKeyDown(SDLK_LCTRL) || isKeyDown(SDLK_RCTRL))
	{
		bool save_as = false;
		if (isKeyDown(SDLK_LSHIFT) || isKeyDown(SDLK_RSHIFT))
			save_as = true;
		if (key.key == SDLK_S && key.type == SDL_EVENT_KEY_DOWN)
		{
			if (save_as)
				Interface::getInstance().stop();
			SaveLoad::save(save_as);
		}
		else if (key.key == SDLK_O && key.type == SDL_EVENT_KEY_DOWN)
		{
			Interface::getInstance().stop();
			SaveLoad::load();
		}
		else if (key.key == SDLK_N && key.type == SDL_EVENT_KEY_DOWN)
		{
			Interface::getInstance().getGrid().clear();
			SaveLoad::clearPath();
		}
	}
}

void Input::handleCopyPaste(SDL_Event *event)
{
	SDL_KeyboardEvent key = event->key;

	if (!isKeyDown(SDLK_LCTRL) && !isKeyDown(SDLK_RCTRL))
		return;

	if (key.type != SDL_EVENT_KEY_DOWN)
		return;

	Vector2f world_pos = floorVec(Camera::screenToWorld(mouse_pos_));

	if (key.key == SDLK_C)
		Interface::getInstance().getGrid().copyBlock(world_pos);
	if (key.key == SDLK_V)
		Interface::getInstance().getGrid().pasteBlock(world_pos);
}

void Input::handleShortcuts(SDL_Event *event)
{
	SDL_KeyboardEvent key = event->key;

	if (key.key == SDLK_F11 && key.type == SDL_EVENT_KEY_DOWN)
		Interface::getInstance().toggleFullscreen();

	if (key.key == SDLK_1 && key.type == SDL_EVENT_KEY_DOWN)
		Interface::getInstance().setSelection(AREA);
	if (key.key == SDLK_2 && key.type == SDL_EVENT_KEY_DOWN)
		Interface::getInstance().setSelection(BLOCK_GENERATOR);
	if (key.key == SDLK_3 && key.type == SDL_EVENT_KEY_DOWN)
		Interface::getInstance().setSelection(BLOCK_SEQUENCER);

	if (key.key == SDLK_SPACE && key.type == SDL_EVENT_KEY_DOWN)
		Interface::getInstance().togglePlayPause();

	if ((key.key == SDLK_Q || key.key == SDLK_ESCAPE) && key.type == SDL_EVENT_KEY_DOWN)
		Interface::getInstance().closeAllWindows();

	if (key.key == SDLK_D && key.type == SDL_EVENT_KEY_DOWN)
		GUI::toggleShowOutput();
}