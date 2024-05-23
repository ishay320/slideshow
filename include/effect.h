#pragma once
#include <array>
#include <atomic>
#include <cstdint>
#include <thread>

#include "getters.h"
#include "raylib.h"
#include "wrappers.h"

void getRandomPointAndVelocity(float radius, float speed, Vector2& point,
                               Vector2& vel);

class Movement {
   public:
    Movement();
    Movement(ImageWrapper image, ImageWrapper background, float speed);

    Movement(Movement&&)                 = default;
    Movement(const Movement&)            = default;
    Movement& operator=(Movement&&)      = default;
    Movement& operator=(const Movement&) = default;
    ~Movement()                          = default;

    void setSpeed(float speed);

    void update(double delta_time, uint8_t transparency);

    void replaceImage(ImageWrapper image, ImageWrapper background);

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
    Effect(ImageGetter image_getter, float speed);

    std::tuple<ImageWrapper, ImageWrapper> loadImages();

    ~Effect();

    void update(double delta_time);

    void next();

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
