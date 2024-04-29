#include <stdlib.h>

#include <filesystem>
#include <iostream>
#include <memory>
#include <random>
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
        Texture texture = LoadTexture(path.string().c_str());
        if (texture.id <= 0) {
            throw std::filesystem::filesystem_error("could not load image",
                                                    path, std::error_code());
        }
        _texture = std::shared_ptr<Texture>(
            new Texture(texture), [](Texture* t) { UnloadTexture(*t); });
    }

    ~TextureWrapper() {}

    Texture texture() const { return *_texture.get(); }

   private:
    std::shared_ptr<Texture> _texture;
};

class GetTextureWrapper {
   public:
    GetTextureWrapper(FileGetter files_getter) : _files_getter(files_getter) {}
    ~GetTextureWrapper() = default;

    TextureWrapper getNext() { return TextureWrapper{_files_getter.getNext()}; }

   private:
    FileGetter _files_getter;
};

struct Point {
    double x;
    double y;
    double vx;
    double vy;
};

Point getRandomPointAndVelocity(double radius, double speed)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0, 2 * M_PI);

    // Generate a random angle
    double angle = dis(gen);

    // Calculate the point coordinates
    double x = radius * cos(angle);
    double y = radius * sin(angle);

    // Calculate the vector from the origin to the point
    double dx = -x;
    double dy = -y;

    // Calculate the length of the vector
    double length = sqrt(dx * dx + dy * dy);

    // Normalize the vector
    dx /= length;
    dy /= length;

    // Scale the normalized vector by the speed
    double vx = dx * speed;
    double vy = dy * speed;

    return {x, y, vx, vy};
}

class Effect {
   public:
    Effect(TextureWrapper texture) : _texture(texture)
    {
        _point = getRandomPointAndVelocity(200, 100);
    }
    Effect(Effect const& e) : _point(e._point), _texture(e._texture) {}
    Effect& operator=(const Effect& e)
    {
        _point   = e._point;
        _texture = e._texture;
        return *this;
    }
    ~Effect() = default;

    void update(double delta_time)
    {
        DrawTexture(_texture.texture(), static_cast<int>(_point.x),
                    static_cast<int>(_point.y), WHITE);
        _point.x += _point.vx * delta_time;
        _point.y += _point.vy * delta_time;
    }

   private:
    Point _point;
    TextureWrapper _texture;
};

// steps:
// [ ] 1. check updates online
// [ ] 2. check config for folders of images
// [x] 3. create class of files
// [x] 4. create class of texture getter
// [x] 5. create class of effect
// [ ] 6. create class of transition- it fill them with the image and transition
// [ ] 7. loop til the world end
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
    TextureWrapper image = gtw.getNext();
    Effect effect{image};

    int next_swap          = config.swap_time;
    double prev_frame_time = GetTime();
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        double current_time = GetTime();
        double delta_time   = current_time - prev_frame_time;
        prev_frame_time     = current_time;
        effect.update(delta_time);

        DrawFPS(50, 50);
        EndDrawing();
        if (GetTime() > next_swap) {
            next_swap += config.swap_time;
            effect = Effect{gtw.getNext()};
        }
    }

    CloseWindow();
    return 0;
}
// TODO: add logger
