#pragma once
#include "raylib.h"
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

    void handleInput();

private:
    CommandManager cmd_mgr;
    Camera2D &camera;

    void handleMouse();
    void handleKeys();

    void handleCamera();
    void handleEdit();

    void handleUndoRedo();

    InputHandler() : camera(Interface::getInstance().getCamera()) {}
    InputHandler(const InputHandler &) = delete;
    InputHandler &operator=(const InputHandler &) = delete;
};