#pragma once
#include "raylib.h"
#include "interface.h"

class InputHandler
{
public:
    static InputHandler &getInstance()
    {
        static InputHandler instance;
        return instance;
    }

    void handleInput();

private:
    void handleMouse();
    void handleKeys();

    void handleCamera();
    void handleEdit();

    InputHandler() {}
    InputHandler(const InputHandler &) = delete;
    InputHandler &operator=(const InputHandler &) = delete;
};