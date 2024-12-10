#include "raylib.h"

#include "interface.h"
#include "audio.h"
#include "input.h"

constexpr unsigned int WIDTH = 1280;
constexpr unsigned int HEIGHT = 720;

int main(void)
{
    InitWindow(WIDTH, HEIGHT, "GemuPG");
    //--------------------------------------------------------------------------------------

    Interface &interface = Interface::getInstance();
    InputHandler &input = InputHandler::getInstance();
    AudioEngine &audio = AudioEngine::getInstance();

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
    CloseWindow();
    //--------------------------------------------------------------------------------------
    return 0;
}