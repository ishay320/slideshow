#include "effect.h"

#include <algorithm>
#include <random>

#include "logger.h"

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
Movement::Movement() : _speed(0) {}
Movement::Movement(ImageWrapper image, ImageWrapper background, float speed)
    : _speed(speed)
{
    this->replaceImage(std::move(image), std::move(background));
}

void Movement::setSpeed(float speed) { _speed = speed; }

void Movement::update(double delta_time, uint8_t transparency)
{
    DrawTextureEx(_texture_back.texture(), {0, 0}, 0, _scale,
                  {255, 255, 255, transparency});
    DrawTextureV(_texture.texture(), _point, {255, 255, 255, transparency});
    _point.x += _point_vel.x * delta_time;
    _point.y += _point_vel.y * delta_time;
}

void Movement::replaceImage(ImageWrapper image, ImageWrapper background)
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

Effect::Effect(ImageGetter image_getter, float speed)
    : _speed(speed), _image_getter(std::move(image_getter))
{
    _image_movement[0].setSpeed(_speed);
    _image_movement[1].setSpeed(_speed);

    auto [image, background] = loadImages();
    _image_movement[0].replaceImage(std::move(image), std::move(background));

    std::tie(image, background) = loadImages();
    _image_movement[1].replaceImage(std::move(image), std::move(background));

    std::tie(_image_buffer, _background_buffer) = loadImages();
}

std::tuple<ImageWrapper, ImageWrapper> Effect::loadImages()
{
    auto image           = _image_getter.getNext();
    vec2i size           = image.size();
    int width            = size.x;
    int height           = size.y;
    const int display    = GetCurrentMonitor();
    const float width_m  = GetMonitorWidth(display);
    const float height_m = GetMonitorHeight(display);
    float normal         = height < width ? height / height_m : width / width_m;
    LOG_INFO("resize image normal: " << normal);
    image.resize(width / normal, height / normal);
    auto background = image.copy();
    background.resize((width / normal) / 2.f, (height / normal) / 2.f);
    background.blur(3);
    return {std::move(image), std::move(background)};
}

Effect::~Effect()
{
    if (_load_buffer.joinable()) {
        _load_buffer.join();
    }
}

void Effect::update(double delta_time)
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

void Effect::next()
{
    _swap_time    = 0;
    _main_movment = !_main_movment;
}
