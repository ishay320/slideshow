#include "effect.h"
#include "inline_logger.h"

#define SIZE_OF_FOREGROUND_IMAGE -50, 50
#define SIZE_OF_BACKGROUND_IMAGE -100, 100

static std::array<Image, 2> swapImages(FileGetter::ImageBuffer& image_buffer);

Effect::Effect(ImageRenderer& image_renderer, FileGetter::ImageBuffer& image_buffer, float slide_time)
    : _image_renderer(image_renderer), _image_buffer(image_buffer), _slide_time(slide_time)
{
    _start_time = glfwGetTime() - _slide_time;
    _image_renderer.setOpacity(_opacity);
}

Effect::~Effect() {}

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
            break;

        default:
            break;
    }
    updatePos();
}

void Effect::render()
{
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
        _images_async = std::async(swapImages, std::ref(_image_buffer));
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

void Effect::updatePos()
{
    const double time    = glfwGetTime();
    const float sin_time = sin(time);

    // create transformations
    _image_renderer.resetTransform(0);
    _image_renderer.translate(0, {sin_time * -100.0, 0});
    _image_renderer.resetTransform(1);
    _image_renderer.translate(1, {sin_time * 100.0, 0});
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

/* ******** */

/**
 * @brief Gets images from buffer and process them
 *
 * @param image_buffer
 * @return std::array<Image, 2> empty images if buffer returned empty image
 */
static std::array<Image, 2> swapImages(FileGetter::ImageBuffer& image_buffer)
{
    std::array<Image, 2> out;
    const size_t width  = 1920;
    const size_t height = 1080;
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