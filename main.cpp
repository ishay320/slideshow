#include <stdlib.h>

#include <cstdio>
#include <filesystem>
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

class ImageWrapper {
   public:
    ImageWrapper() {}
    ImageWrapper(Image image)
    {
        _image = std::shared_ptr<Image>(new Image(image),
                                        [](Image* t) { UnloadImage(*t); });
    }
    ImageWrapper(const std::filesystem::path& path)
    {
        Image image = LoadImage(path.c_str());
        // ImageFormat(&imOrigin, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8)
        _image = std::shared_ptr<Image>(new Image(image),
                                        [](Image* t) { UnloadImage(*t); });
    }
    ImageWrapper(ImageWrapper&&)                 = default;
    ImageWrapper(const ImageWrapper&)            = default;
    ImageWrapper& operator=(ImageWrapper&&)      = default;
    ImageWrapper& operator=(const ImageWrapper&) = default;
    ~ImageWrapper()                              = default;

    Image get() const { return *_image.get(); }
    ImageWrapper blur(int size)
    {
        ImageBlurGaussian(_image.get(), size);
        return *this;
    }

   private:
    std::shared_ptr<Image> _image;
};

class ImageGetter {
   public:
    ImageGetter(FileGetter file_getter) : _file_getter(file_getter) {}
    ImageGetter(ImageGetter&&)                 = default;
    ImageGetter(const ImageGetter&)            = default;
    ImageGetter& operator=(ImageGetter&&)      = default;
    ImageGetter& operator=(const ImageGetter&) = default;
    ~ImageGetter()                             = default;

    // TODO: make it async for better performance
    ImageWrapper getNext() { return {_file_getter.getNext()}; }

   private:
    FileGetter _file_getter;
};

class TextureWrapper {
   public:
    TextureWrapper() {}
    TextureWrapper(const ImageWrapper& image) : _image(image)
    {
        Texture texture = LoadTextureFromImage(_image.get());
        if (texture.id <= 0) {
            throw "could not load image to gpu";
        }
        _texture = std::shared_ptr<Texture>(
            new Texture(texture), [](Texture* t) { UnloadTexture(*t); });
    }

    TextureWrapper(Image image) : _image(image)
    {
        Texture texture = LoadTextureFromImage(image);
        if (texture.id <= 0) {
            throw "could not load image to gpu";
        }
        _texture = std::shared_ptr<Texture>(
            new Texture(texture), [](Texture* t) { UnloadTexture(*t); });
    }
    ~TextureWrapper() {}

    Texture texture() const { return *_texture.get(); }

    ImageWrapper copyImage() const
    {
        Image image = ImageCopy(_image.get());
        return {image};
    }

   private:
    ImageWrapper _image;
    std::shared_ptr<Texture> _texture;
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

class Movement {
   public:
    Movement(ImageWrapper image, float speed) : _speed(speed)
    {
        this->replaceImage(image);
    }

    Movement(Movement&&)                 = default;
    Movement(const Movement&)            = default;
    Movement& operator=(Movement&&)      = default;
    Movement& operator=(const Movement&) = default;
    ~Movement()                          = default;

    void update(double delta_time)
    {
        DrawTextureEx(_texture_back.texture(), {0, 0}, 0, _scale, WHITE);
        DrawTexture(_texture.texture(), static_cast<int>(_point.x),
                    static_cast<int>(_point.y), WHITE);
        _point.x += _point.vx * delta_time;
        _point.y += _point.vy * delta_time;
    }
    void replaceImage(ImageWrapper image)
    {
        _texture = {image};

        const int display  = GetCurrentMonitor();
        const int width_s  = GetMonitorWidth(display);
        const int height_s = GetMonitorHeight(display);

        const int middle_x = width_s / 2;
        const int middle_y = height_s / 2;

        const int width_i  = _texture.texture().width;
        const int height_i = _texture.texture().height;

        _point = getRandomPointAndVelocity(200, _speed);
        _point.x += middle_x - (width_i / 2.f);
        _point.y += middle_y - (height_i / 2.f);
        _scale        = width_i < height_i ? (float)width_s / width_i
                                           : (float)height_s / height_i;
        _texture_back = TextureWrapper{_texture.copyImage().blur(10)};
    }

   private:
    float _speed;
    float _scale;
    Point _point;
    TextureWrapper _texture;
    TextureWrapper _texture_back;
};

class Effect {
   public:
    Effect(ImageGetter image_getter, float speed)
        : _speed(speed),
          _image_getter(image_getter),
          _image_movement(_image_getter.getNext(), _speed)
    {
    }
    ~Effect() = default;

    void update(double delta_time) { _image_movement.update(delta_time); }

    void next() { _image_movement.replaceImage(_image_getter.getNext()); }

   private:
    float _speed;
    ImageGetter _image_getter;
    Movement _image_movement;
};

// steps:
// [ ] 1. check updates online
// [ ] 2. check config for folders of images
// [x] 3. create class of files
// [x] 4. create class of texture getter
// [x] 5. create class of effect
// [ ] 6. create class of transition- it fill them with the image and transition
// [x] 7. loop til the world end
int main(int argc, char* argv[])
{
    struct {
        int swap_time = 5;
        std::vector<std::string> file_types{"png", "jpg"};
        float effect_speed      = 100;
        int fps                 = 30;
        std::string images_path = ".";
    } config;

    LOG_INFO("starting window");
    InitWindow(800, 450, "slideshow");
    const int display = GetCurrentMonitor();
    SetWindowSize(GetMonitorWidth(display), GetMonitorHeight(display));
    SetTargetFPS(config.fps);
    ToggleFullscreen();

    LOG_INFO("loading files from '" << config.images_path << '\'');
    FileGetter fg{config.images_path, config.file_types};
    ImageGetter ig{fg};
    Effect effect{ig, config.effect_speed};

    LOG_INFO("starting the loop");
    int next_swap_time     = config.swap_time;
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

        if (GetTime() > next_swap_time) {
            next_swap_time += config.swap_time;
            effect.next();
            LOG_INFO("changed image");
        }
    }

    LOG_INFO("closing the window");
    CloseWindow();
    return 0;
}
