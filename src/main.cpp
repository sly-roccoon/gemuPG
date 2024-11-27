#include "raylib.h"
#include "interface.h"

constexpr unsigned int WIDTH = 1280;
constexpr unsigned int HEIGHT = 720;

int main(void)
{
    InitWindow(WIDTH, HEIGHT, "GemuPG");
    InitAudioDevice();
    //--------------------------------------------------------------------------------------

    Interface& interface = Interface::getInstance();

    // Main game loop
    while (!WindowShouldClose())
    {
        

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