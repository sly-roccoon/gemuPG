#include "input.h"
#include <cstdio>

void InputHandler::handleMouseWheel(SDL_Event *event)
{
	SDL_MouseWheelEvent wheel = event->wheel;
	if (!wheel.y)
		return;

	Vector2f mouse_pos = {wheel.mouse_x, wheel.mouse_y};
	Camera::setPos((Camera::screenToWorld(mouse_pos) - mouse_pos) / Camera::getZoom()); // TODO: compare with previous raylib code and camera offset/target

	float scale_factor = 1.0f + (0.25f * fabsf(wheel.y));
	if (wheel.y < 0)
		scale_factor = 1.0f / scale_factor;
	camera_.setZoom(std::clamp(camera_.getZoom() * scale_factor, MIN_CAMERA_ZOOM, MAX_CAMERA_ZOOM));
}

void InputHandler::handleMouse(SDL_Event *event)
{
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

	//--------------------------------------------------
	if (button.button == SDL_BUTTON_LEFT && button.down)
	{
		printf("LButton\n");
		Vector2f mouse_pos = {button.x, button.y};
		auto cmd = std::make_unique<AddBlockCommand>(Camera::screenToWorld(mouse_pos));
		cmd_mgr.executeCommand(std::move(cmd));
	}
	if (button.button == SDL_BUTTON_RIGHT && button.down)
	{
		printf("RButton\n");

		Vector2 mouse_pos = {button.x, button.y};
		auto cmd = std::make_unique<RemoveBlockCommand>(Camera::screenToWorld(mouse_pos));
		cmd_mgr.executeCommand(std::move(cmd));
	}
}

void InputHandler::handleEvent(SDL_Event *event)
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
	}
	// handleKeys();
}

// void InputHandler::handleEdit()
// {
// }

// void InputHandler::handleKeys()
// {
// 	InputHandler::handleUndoRedo();
// }

// void InputHandler::handleUndoRedo()
// {
// 	if (IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL))
// 		if (IsKeyPressed(KEY_Z))
// 		{
// 			cmd_mgr.undo();
// 		}
// 		else if (IsKeyPressed(KEY_Y))
// 		{
// 			cmd_mgr.redo();
// 		}
// }