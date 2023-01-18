#ifndef TEXTURE_H
#define TEXTURE_H
#include "image.h"

class Texture
{
public:
    Texture(const char* texture_path);
    Texture(const Image& image);
    Texture(unsigned char* image_data, int width, int height, int channels);

    void setTexture(const Image& image);

    ~Texture();

    void bind();

private:
    unsigned int _texture;
};

#endif // TEXTURE_H