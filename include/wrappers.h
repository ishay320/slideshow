#pragma once
#include <libheif/heif.h>

#include <cstring>
#include <filesystem>

#include "raylib.h"
#include "utils.h"

class ImageWrapper {
   public:
    ImageWrapper();
    ImageWrapper(ImageWrapper&& other);
    ImageWrapper(const ImageWrapper&) = delete;
    ImageWrapper& operator=(ImageWrapper&& other);

    ImageWrapper(std::filesystem::path path);
    ~ImageWrapper();

    /**
     * warning: do not use value after the class destructed
     */
    const Image get() const;

    ImageWrapper& blur(int size);

    ImageWrapper copy() const;

    void resize(size_t width, size_t height);

    vec2i size();

   private:
    void loadFromImage(Image image);

    enum class TYPE { EMPTY, NORMAL, HEIF, RAW };
    TYPE _type;

    Image _image{};
};

class TextureWrapper {
   public:
    TextureWrapper();
    TextureWrapper(const ImageWrapper& image);

    TextureWrapper(Image image);
    ~TextureWrapper();

    Texture texture() const;

   private:
    std::shared_ptr<Texture> _texture;
};
