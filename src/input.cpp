#include "input.h"
#include <cstdio>

void Input::handleEvent(SDL_Event *event)
{
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

	if (event->type == SDL_EVENT_KEY_DOWN)
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

void Input::handleMouse(SDL_Event *event)
{
	//---------------------PANNING-----------------------------
	static bool isDragging;
	static Vector2f delta;

	if (event->type == SDL_EVENT_MOUSE_MOTION)
	{
		delta = {event->motion.xrel, event->motion.yrel};
	}

	SDL_MouseButtonEvent button = event->button;
	if (button.button == SDL_BUTTON_MIDDLE)
	{
		if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN)
			isDragging = true;
		if (event->type == SDL_EVENT_MOUSE_BUTTON_UP)
			isDragging = false;
	}
	if (isDragging)
		Camera::setPos(Camera::getPos() + delta * -1.0f / Camera::getZoom());

	//---------------------PLACING-----------------------------
	if (button.button == SDL_BUTTON_LEFT && button.down)
	{
		Vector2f mouse_pos = {button.x, button.y};
		auto cmd = std::make_unique<AddBlockCommand>(Camera::screenToWorld(mouse_pos));
		cmd_mgr.executeCommand(std::move(cmd));
	}
	if (button.button == SDL_BUTTON_RIGHT && button.down)
	{
		Vector2 mouse_pos = {button.x, button.y};
		auto cmd = std::make_unique<RemoveBlockCommand>(Camera::screenToWorld(mouse_pos));
		cmd_mgr.executeCommand(std::move(cmd));
	}
}

void Input::handleKeys(SDL_Event *event)
{
	updateKeys(event);
	handleUndoRedo(event);
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