#include <iostream>

#include "raylib.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

int main(int argc, char *argv[])
{
    InitWindow(800, 450, "slideshow");
    SetTargetFPS(30);

    Texture2D image = LoadTexture("./data/test.jpg");
    if (image.id <= 0) {
        std::cout << "could not load image\n";
        return 1;
    }

    int pos_x = 0, pos_y = 0;
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawTexture(image, pos_x++, pos_y++, WHITE);

        DrawFPS(50, 50);
        EndDrawing();
    }

    UnloadTexture(image);
    CloseWindow();
    return 0;
}
