#include "raylib.h"

constexpr unsigned int WIDTH = 1280;
constexpr unsigned int HEIGHT = 720;

void draw();

int main(void)
{
    InitWindow(WIDTH, HEIGHT, "GemuPG");
    InitAudioDevice();
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())
    {
        

        BeginDrawing();
            ClearBackground(RAYWHITE);
            draw();
        EndDrawing();
    }



    //--------------------------------------------------------------------------------------
    CloseAudioDevice();
    CloseWindow();
    //--------------------------------------------------------------------------------------
    return 0;
}

void draw()
{
    return;
}