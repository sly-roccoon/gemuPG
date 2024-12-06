#include "raylib.h"

#include "interface.h"
#include "input.h"

constexpr unsigned int WIDTH = 1280;
constexpr unsigned int HEIGHT = 720;

int main(void)
{
    InitWindow(WIDTH, HEIGHT, "GemuPG");
    InitAudioDevice();
    //--------------------------------------------------------------------------------------

    Interface &interface = Interface::getInstance();
    InputHandler &input = InputHandler::getInstance();

    // Main game loop
    while (!WindowShouldClose())
    {
        input.handleInput();

        BeginDrawing();
        ClearBackground(RAYWHITE);
        interface.draw();
        EndDrawing();
    }

    //--------------------------------------------------------------------------------------
    CloseAudioDevice();
    CloseWindow();
    //--------------------------------------------------------------------------------------
    return 0;
}