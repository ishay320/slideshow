#include <iostream>

#include "raylib.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

int main(int argc, char *argv[])
{
    InitWindow(800, 450, "raylib [core] example - basic window");

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("Congrats! You created your first window!", 190, 200, 20,
                 LIGHTGRAY);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
