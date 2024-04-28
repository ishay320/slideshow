#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

#include "commandline.h"
#include "raylib.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

std::vector<std::string> saparateByDelim(const std::string& s,
                                         const std::string& delim)
{
    std::vector<std::string> out;
    size_t pos  = 0;
    size_t last = 0;
    while ((pos = s.find(delim, last)) != std::string::npos) {
        out.push_back(s.substr(last, pos));
        last = pos + 1;
    }
    return out;
}

std::vector<std::filesystem::path> getFilesByRegex(
    const std::filesystem::path& path, const std::string& regex)
{
    std::vector<std::filesystem::path> out;
    std::string c_out = run_command_with_out("find", {path, "-name", regex});
    for (auto&& s : saparateByDelim(c_out, "\n")) {
        out.push_back(s);
    }
    return out;
}

// get random file
// get image (char**)
class FileGetter {
   public:
    FileGetter();
    ~FileGetter();

    std::filesystem::path getNext();
};

class TextureWrapper {
   public:
    TextureWrapper(const std::filesystem::path& path)
    {
        _texture = LoadTexture(path.string().c_str());
        if (_texture.id <= 0) {
            throw std::filesystem::filesystem_error("could not load image",
                                                    path, std::error_code());
        }
    }
    ~TextureWrapper() { UnloadTexture(_texture); }

    Texture texture() { return _texture; }

   private:
    Texture _texture;
};

class GetTextureWrapper {
   public:
    GetTextureWrapper(FileGetter files_getter) : _files_getter(files_getter) {}
    ~GetTextureWrapper() = default;

    TextureWrapper getNext();

   private:
    FileGetter _files_getter;
};

// steps:
// [ ] 1. check updates online
// [ ] 2. check config for folders of images
// [ ] 3. create class of files
// [ ] 4. create class of texture getter
// [ ] 5. create class of effect
// [ ] 6. loop til the world end
int main(int argc, char* argv[])
{
    InitWindow(800, 450, "slideshow");
    const int display = GetCurrentMonitor();
    SetWindowSize(GetMonitorWidth(display), GetMonitorHeight(display));
    ToggleFullscreen();
    SetTargetFPS(30);

    std::vector<std::filesystem::path> files = getFilesByRegex(".", "*\\.jpg");
    TextureWrapper image{files[0]};

    int pos_x = 0, pos_y = 0;
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawTexture(image.texture(), pos_x++, pos_y++, WHITE);

        DrawFPS(50, 50);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
// TODO: add logger
