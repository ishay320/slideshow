#include "effect.h"
#include "inline_logger.h"

#define SIZE_OF_FOREGROUND_IMAGE -100, 100
#define SIZE_OF_BACKGROUND_IMAGE -300, 300

static std::array<Image, 2> getAndProcessImages(FileGetter::ImageBuffer& image_buffer, glm::vec2 resolution);
static void clearBackground();

Effect::Effect(ImageRenderer& image_renderer, FileGetter::ImageBuffer& image_buffer, float slide_time)
    : _image_renderer(image_renderer), _image_buffer(image_buffer), _slide_time(slide_time)
{
    _start_time = glfwGetTime() - _slide_time;
    _image_renderer.setOpacity(_opacity);
}

Effect::~Effect() {}

void Effect::setNewMovement()
{
#if 1
    // set for specific effect
    const float vel_x = (rand() % 20 - 10) / 40.f;
    const float vel_y = (rand() % 20 - 10) / 60.f;

    _movements[0].pos_x = 0;
    _movements[0].pos_y = 0;
    _movements[0].vel_x = vel_x;
    _movements[0].vel_y = vel_y;

    _movements[1].pos_x = 0;
    _movements[1].pos_y = 0;
    _movements[1].vel_x = -vel_x / 2.0f;
    _movements[1].vel_y = -vel_y / 2.0f;
#else
    for (size_t i = 0; i < EFFECT_IMAGES_NUM; i++)
    {
        _movements[i].pos_x = 0;
        _movements[i].pos_y = 0;
        _movements[i].vel_x = (rand() % 20 - 10) / 40.f;
        _movements[i].vel_y = (rand() % 20 - 10) / 60.f;
    }
#endif
}

void Effect::update()
{
    switch (checkEffectTime())
    {
        case RendererMode::RENDER_MODE_TRANSITION_IN:
            _opacity = (glfwGetTime() - _start_time) / _transition_time;
            _image_renderer.setOpacity(_opacity);
            break;
        case RendererMode::RENDER_MODE_TRANSITION_OUT:
            _opacity = (_transition_time - (glfwGetTime() - _start_time - (_slide_time - _transition_time))) / _transition_time;
            _image_renderer.setOpacity(_opacity);
            break;
        case RendererMode::RENDER_MODE_SHOW:
            // Do nothing
            break;
        case RendererMode::RENDER_MODE_DONE:
            updateImages();
            setNewMovement();
            break;

        default:
            break;
    }
    updatePos();
}

void Effect::render()
{
    clearBackground();

    _image_renderer.drawImages();
}

void Effect::updateImages()
{
    if (!_images_async.valid())
    {
        // Images did not loaded in the buffer
        if (_image_buffer.empty())
        {
            return;
        }
        _images_async = std::async(getAndProcessImages, std::ref(_image_buffer), _image_renderer.getResolution());
    }

    using namespace std::chrono_literals;
    if (_images_async.wait_for(0ms) == std::future_status::ready)
    {
        std::array<Image, 2> images = _images_async.get();

        _image_renderer.popImage();
        _image_renderer.popImage();
        _image_renderer.pushImage(images[0], SIZE_OF_FOREGROUND_IMAGE);
        _image_renderer.pushImage(images[1], SIZE_OF_BACKGROUND_IMAGE);

        _start_time = glfwGetTime();
    }
}

void Effect::updateMovement()
{
    for (size_t i = 0; i < EFFECT_IMAGES_NUM; i++)
    {
        _movements[i].pos_x += _movements[i].vel_x;
        _movements[i].pos_y += _movements[i].vel_y;
    }
}
void Effect::updatePos()
{
    const double time = glfwGetTime();
    updateMovement();

    // create transformations
    size_t image_num = 0;
    _image_renderer.resetTransform(image_num);
    _image_renderer.translate(image_num, {_movements[image_num].pos_x, _movements[image_num].pos_y});
    image_num++;
    _image_renderer.resetTransform(image_num);
    _image_renderer.translate(image_num, {_movements[image_num].pos_x, _movements[image_num].pos_y});
}

Effect::RendererMode Effect::checkEffectTime()
{
    const double runtime = glfwGetTime() - _start_time;

    if (runtime > _slide_time)
        return RendererMode::RENDER_MODE_DONE;

    if (_transition_time > runtime)
        return RendererMode::RENDER_MODE_TRANSITION_IN;

    if (runtime > _slide_time - _transition_time)
        return RendererMode::RENDER_MODE_TRANSITION_OUT;

    return RendererMode::RENDER_MODE_SHOW;
}

/* ************************** */
/* **** static functions **** */
/* ************************** */

/**
 * @brief Gets images from buffer and process them
 *
 * @param image_buffer buffer to pull images from
 * @param resolution image max resolution
 * @return std::array<Image, 2> empty images if buffer returned empty image
 */
static std::array<Image, 2> getAndProcessImages(FileGetter::ImageBuffer& image_buffer, glm::vec2 resolution)
{
    std::array<Image, 2> out;
    const size_t width  = resolution.x;
    const size_t height = resolution.y;
    Image image         = image_buffer.getNext();
    if (image.isEmpty())
    {
        LOG_WARNING("images are empty");
        return out;
    }
    Image image_resized = resizeToMax(image, width, height);

    Image background{image_resized};
    fastGaussianBlur(background, 15);

    out[0] = std::move(image_resized);
    out[1] = std::move(background);
    return out;
}

static void clearBackground()
{
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

// TODO: replace images with new images and do not go to black screen
// TODO: add zoom effect