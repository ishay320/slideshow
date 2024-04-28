#include <stdlib.h>

#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "commandline.h"
#include "logger.h"
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
        out.push_back(s.substr(last, pos - last));
        last = pos + 1;
    }
    return out;
}

std::vector<std::filesystem::path> getFilesByTypes(
    const std::filesystem::path& path, const std::vector<std::string>& types)
{
    std::vector<std::string> search_prompt;
    if (types.empty()) {
        throw std::runtime_error{"no types passed"};
    }

    search_prompt.push_back(".");
    search_prompt.push_back("-iname");
    search_prompt.push_back("*\\." + types[0]);
    for (size_t i = 1; i < types.size(); i++) {
        search_prompt.push_back("-o");
        search_prompt.push_back("-iname");
        search_prompt.push_back("*\\." + types[i]);
    }

    std::vector<std::filesystem::path> out;
    std::string c_out = run_command_with_out("find", search_prompt);
    for (auto&& s : saparateByDelim(c_out, "\n")) {
        out.push_back(s);
    }
    return out;
}

class FileGetter {
   public:
    FileGetter(const std::string& path, const std::vector<std::string>& types)
    {
        _paths = getFilesByTypes(path, types);
    }
    ~FileGetter() = default;

    std::filesystem::path getNext()
    {
        int pos = rand() % _paths.size();
        return _paths[pos];
    }

   private:
    std::vector<std::filesystem::path> _paths;
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

    // FIXME: I`m broken
    ~TextureWrapper() { UnloadTexture(_texture); }

    Texture texture() { return _texture; }

   private:
    Texture _texture;
};

class GetTextureWrapper {
   public:
    GetTextureWrapper(FileGetter files_getter) : _files_getter(files_getter) {}
    ~GetTextureWrapper() = default;

    TextureWrapper getNext() { return {_files_getter.getNext()}; }

   private:
    FileGetter _files_getter;
};

// steps:
// [ ] 1. check updates online
// [ ] 2. check config for folders of images
// [x] 3. create class of files
// [x] 4. create class of texture getter
// [ ] 5. create class of effect
// [ ] 6. loop til the world end
int main(int argc, char* argv[])
{
    struct {
        int swap_time = 5;
        std::vector<std::string> file_types{"png", "jpg"};
        int fps = 30;
    } config;

    InitWindow(800, 450, "slideshow");
    const int display = GetCurrentMonitor();
    SetWindowSize(GetMonitorWidth(display), GetMonitorHeight(display));
    SetTargetFPS(config.fps);
    ToggleFullscreen();

    FileGetter fg{".", config.file_types};
    GetTextureWrapper gtw{fg};
    auto image = gtw.getNext();

    int pos_x = 0, pos_y = 0;
    int next_swap = config.swap_time;
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawTexture(image.texture(), pos_x++, pos_y++, WHITE);

        DrawFPS(50, 50);
        EndDrawing();
        if (GetTime() > next_swap) {
            next_swap += config.swap_time;
            image = gtw.getNext();
        }
    }

    CloseWindow();
    return 0;
}
// TODO: add logger
