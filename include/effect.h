#ifndef EFFECT_H
#define EFFECT_H

#include "file_getter.h"
#include "image_renderer.h"

#include <array>
#include <future>

#define EFFECT_IMAGES_NUM 2

typedef struct
{
    float pos_x;
    float pos_y;
    float vel_x;
    float vel_y;
} Movement;

class Effect
{
public:
    Effect(ImageRenderer& image_renderer, FileGetter::ImageBuffer& image_buffer, float slide_time);
    ~Effect();

    void update();
    void render();

private:
    void updateImages();
    void updatePos();
    void setNewMovement();
    void updateMovement();

    typedef enum // TODO: add start transition mode that runs once for hot swap images
    {
        RENDER_MODE_SHOW = 0,
        RENDER_MODE_DONE,
        RENDER_MODE_TRANSITION_OUT,
        RENDER_MODE_TRANSITION_IN,
        COUNT_RENDER_MODE
    } RendererMode;

    RendererMode checkEffectTime();

    ImageRenderer& _image_renderer;
    FileGetter::ImageBuffer& _image_buffer;

    double _start_time;         /* The time that the current slide started */
    float _slide_time;          /* The time that the whole slide takes */
    float _transition_time = 1; /* Transition time - start and end */
    float _opacity         = 0; /* Current opacity */

    Movement _movements[EFFECT_IMAGES_NUM];

    std::future<std::array<Image, EFFECT_IMAGES_NUM>> _images_async;
};

#endif // EFFECT_H