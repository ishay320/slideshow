#pragma once
#include <libheif/heif.h>

#include <cstring>
#include <filesystem>

#include "raylib.h"
#include "utils.h"

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

    vec2i size() { return {_image.width, _image.height}; }

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
