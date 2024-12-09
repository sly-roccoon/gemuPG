#include "input.h"
#include "raymath.h"
#include "stdio.h"

void InputHandler::handleInput()
{
    handleMouse();
    handleKeys();
}

void InputHandler::handleMouse()
{
    handleCamera();
    handleEdit();
}

void InputHandler::handleCamera()
{
    float wheel = GetMouseWheelMove();

    if (wheel)
    {
        Vector2 mouse_grid_pos = GetScreenToWorld2D(GetMousePosition(), camera);
        camera.offset = GetMousePosition();
        camera.target = mouse_grid_pos;

        float scale_factor = 1.0f + (0.25f * fabsf(wheel));
        if (wheel < 0)
            scale_factor = 1.0f / scale_factor;
        camera.zoom = Clamp(camera.zoom * scale_factor, MIN_CAMERA_ZOOM, MAX_CAMERA_ZOOM);
    }
    else if (IsMouseButtonDown(MOUSE_MIDDLE_BUTTON))
    {
        Vector2 delta = GetMouseDelta();
        delta = delta * -1.0f / camera.zoom;
        camera.target = camera.target + delta;
    }
}

void InputHandler::handleEdit()
{
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        Vector2 mouse_grid_pos = GetScreenToWorld2D(GetMousePosition(), camera);
        auto cmd = std::make_unique<AddBlockCommand>(mouse_grid_pos);
        cmd_mgr.executeCommand(std::move(cmd));
    }
    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
    {
        Vector2 mouse_grid_pos = GetScreenToWorld2D(GetMousePosition(), camera);
        auto cmd = std::make_unique<RemoveBlockCommand>(mouse_grid_pos);
        cmd_mgr.executeCommand(std::move(cmd));
    }
}

void InputHandler::handleKeys()
{
    InputHandler::handleUndoRedo();
}

void InputHandler::handleUndoRedo()
{
    if (IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL))
        if (IsKeyPressed(KEY_Z))
        {
            cmd_mgr.undo();
        }
        else if (IsKeyPressed(KEY_Y))
        {
            cmd_mgr.redo();
        }
}