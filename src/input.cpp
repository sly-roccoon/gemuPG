#include "input.h"
#include "raymath.h"

void InputHandler::handleInput()
{
    handleMouse();
    handleKeys();
}

void InputHandler::handleMouse()
{
    handleCamera();
}

void InputHandler::handleCamera()
{
    float wheel = GetMouseWheelMove();
    Camera2D &camera = Interface::getInstance().getCamera();

    if (wheel)
    {
        Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), camera);
        camera.offset = GetMousePosition();
        camera.target = mouseWorldPos;

        float scaleFactor = 1.0f + (0.25f * fabsf(wheel));
        if (wheel < 0)
            scaleFactor = 1.0f / scaleFactor;
        camera.zoom = Clamp(camera.zoom * scaleFactor, 0.125f, 64.0f);
    }
    else if (IsMouseButtonDown(MOUSE_MIDDLE_BUTTON))
    {
        Vector2 delta = GetMouseDelta();
        delta = delta * -1.0f / camera.zoom;
        camera.target = camera.target + delta;
    }
}

void InputHandler::handleKeys()
{
}