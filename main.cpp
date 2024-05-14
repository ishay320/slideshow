#include <libheif/heif.h>
#include <stdlib.h>

#include <algorithm>
#include <array>
#include <atomic>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <memory>
#include <random>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

#include "commandline.h"
#include "logger.h"
#include "raylib.h"

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

    search_prompt.push_back(path);
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

struct vec2 {
    int x;
    int y;
};

class FileGetter {
   public:
    FileGetter(const std::string& path, const std::vector<std::string>& types)
    {
        _paths = getFilesByTypes(path, types);
        srand(time(NULL));
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
    ImageWrapper() { _type = TYPE::EMPTY; }
    ImageWrapper(ImageWrapper&& other)
    {
        _image       = other._image;
        _type        = other._type;
        other._image = {};
        other._type  = TYPE::EMPTY;
    }
    ImageWrapper(const ImageWrapper&) = delete;
    ImageWrapper& operator=(ImageWrapper&& other)
    {
        if (this == &other) {
            return *this;
        }
        _image       = other._image;
        _type        = other._type;
        other._image = {};
        other._type  = TYPE::EMPTY;
        return *this;
    }

    ImageWrapper(std::filesystem::path path)
    {
        if (path.extension() == ".heic") {
            // TODO: make sure that there is no memory leak
            heif_context* ctx = heif_context_alloc();
            heif_context_read_from_file(ctx, path.c_str(), nullptr);

            // get a handle to the primary image
            heif_image_handle* handle;
            heif_context_get_primary_image_handle(ctx, &handle);

            // decode the image and convert colorspace to RGB, saved as 24bit
            // interleaved
            heif_image* img;
            heif_decode_image(handle, &img, heif_colorspace_RGB,
                              heif_chroma_interleaved_RGB, nullptr);

            int stride;
            auto data = (uint8_t*)heif_image_get_plane(
                img, heif_channel_interleaved, &stride);
            _image.width   = heif_image_handle_get_width(handle);
            _image.height  = heif_image_handle_get_height(handle);
            _image.mipmaps = 1;
            _image.format  = PIXELFORMAT_UNCOMPRESSED_R8G8B8;
            _type          = TYPE::HEIF;
            _image.data    = malloc(3 * _image.width * _image.height);
            memcpy(_image.data, data, 3 * _image.width * _image.height);

            heif_image_release(img);
            heif_image_handle_release(handle);
            heif_context_free(ctx);
        }
        else {
            auto image = LoadImage(path.c_str());
            loadFromImage(image);
            _type = TYPE::NORMAL;
        }
    }
    ~ImageWrapper()
    {
        if (_type != TYPE::EMPTY) {
            UnloadImage(_image);
        }
    }

    /**
     * warning: do not use value after the class destructed
     */
    const Image get() const { return _image; }

    ImageWrapper& blur(int size)
    {
        ImageBlurGaussian(&_image, size);
        return *this;
    }

    ImageWrapper copy() const
    {
        auto image = ImageCopy(_image);
        ImageWrapper ret;
        ret.loadFromImage(image);
        return ret;
    }

    void resize(size_t width, size_t height)
    {
        ImageResize(&_image, width, height);
    }
    vec2 size() { return {_image.width, _image.height}; }

   private:
    void loadFromImage(Image image)
    {
        _type  = TYPE::RAW;
        _image = image;
    }

    enum class TYPE { EMPTY, NORMAL, HEIF, RAW };
    TYPE _type;

    Image _image{};
};

class ImageGetter {
   public:
    ImageGetter(FileGetter file_getter) : _file_getter(file_getter)
    {
        _buffer = _file_getter.getNext();
    }
    ImageGetter(ImageGetter&&)            = default;
    ImageGetter& operator=(ImageGetter&&) = default;
    ~ImageGetter() { join(); }

    ImageWrapper getNext()
    {
        join();
        ImageWrapper image = std::move(_buffer);
        _buffer_thread =
            std::thread{[&]() { _buffer = _file_getter.getNext(); }};
        return image;
    }

   private:
    void join()
    {
        if (_buffer_thread.joinable()) {
            _buffer_thread.join();
        }
    }
    FileGetter _file_getter;
    ImageWrapper _buffer;
    std::thread _buffer_thread;
};

class TextureWrapper {
   public:
    TextureWrapper()
    {
        // TODO: check for memory leak
        Image img = {
            .data    = calloc(1920 * 1080, 1),
            .width   = 1920,
            .height  = 1080,
            .mipmaps = 1,
            .format  = PIXELFORMAT_UNCOMPRESSED_GRAYSCALE,
        };
        Texture texture = LoadTextureFromImage(img);
        if (texture.id <= 0) {
            throw std::runtime_error{"could not load image to gpu"};
        }
        _texture = std::shared_ptr<Texture>(
            new Texture(texture), [](Texture* t) { UnloadTexture(*t); });
    }
    TextureWrapper(const ImageWrapper& image)
    {
        Texture texture = LoadTextureFromImage(image.get());
        if (texture.id <= 0) {
            throw std::runtime_error{"could not load image to gpu"};
        }
        _texture = std::shared_ptr<Texture>(
            new Texture(texture), [](Texture* t) { UnloadTexture(*t); });
    }

    TextureWrapper(Image image)
    {
        Texture texture = LoadTextureFromImage(image);
        if (texture.id <= 0) {
            throw std::runtime_error{"could not load image to gpu"};
        }
        _texture = std::shared_ptr<Texture>(
            new Texture(texture), [](Texture* t) { UnloadTexture(*t); });
    }
    ~TextureWrapper() {}

    Texture texture() const { return *_texture.get(); }

   private:
    std::shared_ptr<Texture> _texture;
};

void getRandomPointAndVelocity(float radius, float speed, Vector2& point,
                               Vector2& vel)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0, 2 * M_PI);

    // Generate a random angle
    float angle = dis(gen);

    // Calculate the point coordinates
    float x = radius * std::cos(angle);
    float y = radius * std::sin(angle);

    // Calculate the vector from the origin to the point
    float dx = -x;
    float dy = -y;

    // Calculate the length of the vector
    float length = std::sqrt(dx * dx + dy * dy);

    // Normalize the vector
    dx /= length;
    dy /= length;

    // Scale the normalized vector by the speed
    float vx = dx * speed;
    float vy = dy * speed;

    point = {x, y};
    vel   = {vx, vy};
}

class Movement {
   public:
    Movement() : _speed(0) {}
    Movement(ImageWrapper image, ImageWrapper background, float speed)
        : _speed(speed)
    {
        this->replaceImage(std::move(image), std::move(background));
    }

    Movement(Movement&&)                 = default;
    Movement(const Movement&)            = default;
    Movement& operator=(Movement&&)      = default;
    Movement& operator=(const Movement&) = default;
    ~Movement()                          = default;

    void setSpeed(float speed) { _speed = speed; }

    void update(double delta_time, uint8_t transparency)
    {
        DrawTextureEx(_texture_back.texture(), {0, 0}, 0, _scale,
                      {255, 255, 255, transparency});
        DrawTextureV(_texture.texture(), _point, {255, 255, 255, transparency});
        _point.x += _point_vel.x * delta_time;
        _point.y += _point_vel.y * delta_time;
    }

    void replaceImage(ImageWrapper image, ImageWrapper background)
    {
        _texture = TextureWrapper{image};

        _texture_back = TextureWrapper{background};
        SetTextureFilter(_texture_back.texture(), TEXTURE_FILTER_BILINEAR);

        const int display  = GetCurrentMonitor();
        const int width_s  = GetMonitorWidth(display);
        const int height_s = GetMonitorHeight(display);

        const int middle_x = width_s / 2;
        const int middle_y = height_s / 2;

        const int width_i  = _texture_back.texture().width;
        const int height_i = _texture_back.texture().height;
        _scale = std::max((float)width_s / width_i, (float)height_s / height_i);

        getRandomPointAndVelocity(200, _speed, _point, _point_vel);
        _point.x += middle_x - (_texture.texture().width / 2.f);
        _point.y += middle_y - (_texture.texture().height / 2.f);
    }

   private:
    float _speed;
    float _scale;
    Vector2 _point;
    Vector2 _point_vel;
    TextureWrapper _texture;
    TextureWrapper _texture_back;
};

class Effect {
   public:
    Effect(ImageGetter image_getter, float speed)
        : _speed(speed), _image_getter(std::move(image_getter))
    {
        _image_movement[0].setSpeed(_speed);
        _image_movement[1].setSpeed(_speed);

        auto [image, background] = loadImages();
        _image_movement[0].replaceImage(std::move(image),
                                        std::move(background));

        std::tie(image, background) = loadImages();
        _image_movement[1].replaceImage(std::move(image),
                                        std::move(background));

        std::tie(_image_buffer, _background_buffer) = loadImages();
    }

    std::tuple<ImageWrapper, ImageWrapper> loadImages()
    {
        auto image           = _image_getter.getNext();
        vec2 size            = image.size();
        int width            = size.x;
        int height           = size.y;
        const int display    = GetCurrentMonitor();
        const float width_m  = GetMonitorWidth(display);
        const float height_m = GetMonitorHeight(display);
        float normal = height < width ? height / height_m : width / width_m;
        LOG_INFO("resize image normal: " << normal);
        image.resize(width / normal, height / normal);
        auto background = image.copy();
        background.resize((width / normal) / 2.f, (height / normal) / 2.f);
        background.blur(3);
        return {std::move(image), std::move(background)};
    }

    ~Effect()
    {
        if (_load_buffer.joinable()) {
            _load_buffer.join();
        }
    }

    void update(double delta_time)
    {
        uint8_t transparency = 255;
        if (_swap_time < 1) {
            _swap_time   = _swap_time + (1 * delta_time);
            transparency = _swap_time * 255;
            _image_movement[!_main_movment].update(delta_time, 255);
        }
        else if (_swap_time < 2) {
            _swap_time = 5;  // replace once
            _image_movement[!_main_movment].replaceImage(
                std::move(_image_buffer), std::move(_background_buffer));
            if (_load_buffer.joinable()) {
                _load_buffer.join();
            }
            _load_buffer = std::thread{[&]() {
                std::tie(_image_buffer, _background_buffer) = loadImages();
            }};
        }
        _image_movement[_main_movment].update(delta_time, transparency);
    }

    void next()
    {
        _swap_time    = 0;
        _main_movment = !_main_movment;
    }

   private:
    std::atomic<float> _swap_time     = 5;
    std::atomic<size_t> _main_movment = 0;
    float _speed;
    ImageGetter _image_getter;
    std::array<Movement, 2> _image_movement;

    std::thread _load_buffer;
    ImageWrapper _image_buffer;
    ImageWrapper _background_buffer;
};

// steps:
// [ ] 1. check updates online
// [ ] 2. check config for folders of images
// [x] 3. create class of files
// [x] 4. create class of texture getter
// [x] 5. create class of effect
// [x] 6. using effect class fill it with the image and transition
// [x] 7. loop til the world end
int main(int argc, char* argv[])
{
    struct {
        int swap_time = 8;
        std::vector<std::string> file_types{"png", "jpg", "heic"};
        float effect_speed      = 50;
        int fps                 = 60;
        std::string images_path = "./data";
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
    Effect effect{std::move(ig), config.effect_speed};

    LOG_INFO("starting the loop");
    int next_swap_time     = config.swap_time;
    double prev_frame_time = GetTime();
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);

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
