#include "input.h"
#include "gui.h"
#include <cstdio>

void Input::handleEvent(SDL_Event *event)
{
	ImGui_ImplSDL3_ProcessEvent(event);

	if (ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantCaptureKeyboard)
		return;

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

	Camera::zoomAtPos(mouse_pos, scale_factor);
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
		Camera::setPos(Camera::getPos() + delta * -1.0f / Camera::getZoom());
}

void Input::handlePlacement(SDL_Event *event)
{
	SDL_MouseButtonEvent button = event->button;
	Vector2f mouse_pos = {button.x, button.y};
	Grid &grid = Interface::getInstance().getGrid();
	Vector2f world_pos = Camera::screenToWorld(mouse_pos);
	std::unique_ptr<Command> cmd;
	block_type_t cur_selection = Interface::getInstance().getSelection();

	if (button.button == SDL_BUTTON_LEFT && !isKeyDown(SDLK_LCTRL))
	{
		if (cur_selection == AREA) // draw area
		{
			if (grid.getArea(world_pos))
				return;
			cmd = std::make_unique<AddAreaCommand>(world_pos);
		}
		else if (button.down) // place block
		{
			Block *block_at_mouse = grid.getBlock(world_pos);
			if (block_at_mouse)
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
		else if (button.down) // remove block
		{
			cmd = std::make_unique<RemoveBlockCommand>(world_pos);
		}
	}

	if (cmd)
		cmd_mgr.executeCommand(std::move(cmd));
}

void Input::handleMouse(SDL_Event *event)
{
	handleCamPan(event);
	handlePlacement(event);
}

void Input::handleKeys(SDL_Event *event)
{
	updateKeys(event);
	// handleUndoRedo(event); //buggy, no time to fix :)
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