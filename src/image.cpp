#include "image.h"

#include <iostream>

Image::Image(const char* filename, bool flip)
{
    stbi_set_flip_vertically_on_load(flip);
    _image_data = stbi_load(filename, &_width, &_height, &_channels, 0);
    if (_image_data == nullptr)
    {
        std::cerr << __FILE__ << ':' << __LINE__ << " "
                  << "[ERROR] image '" << filename << "' could not load: " << stbi_failure_reason() << '\n';
    }
}

Image::Image(unsigned char* image_data, int width, int height, int channels)
    : _image_data(image_data), _width(width), _height(height), _channels(channels)
{
}

Image::~Image() { stbi_image_free(_image_data); }

int Image::getWidth() const { return _width; }
int Image::getHeight() const { return _height; }
int Image::getChannels() const { return _channels; }

inline Pixel* Image::getPixel(int x, int y) const { return (Pixel*)&_image_data[_channels * (x * _width + y)]; }

unsigned char* Image::getData() const { return _image_data; }

void removeChannel(Image& image, bool remove_r, bool remove_g, bool remove_b)
{
    for (int i = 0; i < image.getHeight(); i++)
    {
        for (int j = 0; j < image.getWidth(); j++)
        {
            Pixel* pixel = image.getPixel(i, j);

            if (remove_r)
                pixel->r = 0;
            if (remove_g)
                pixel->g = 0;
            if (remove_b)
                pixel->b = 0;
        }
    }
}

void blurImage(unsigned char* image, unsigned int radius, unsigned int width, unsigned int height, unsigned int channels)
{
    if (image == nullptr)
    {
        std::cerr << __FILE__ << ':' << __LINE__ << " "
                  << "[ERROR] input image is empty\n";
        return;
    }
    if (radius == 0)
    {
        std::cerr << __FILE__ << ':' << __LINE__ << " "
                  << "[ERROR] input radius cannot be 0\n";
        return;
    }

    const int width_compensated = width * channels;
    unsigned char* image_copy   = (unsigned char*)malloc(width_compensated * height * sizeof(unsigned char));
    if (image_copy == nullptr)
    {
        std::cerr << __FILE__ << ':' << __LINE__ << " "
                  << "[ERROR] buy more ram :)\n";
        return;
    }

    for (int i = 0; i < (int)height; i++)
    {
        for (int j = 0; j < width_compensated; j += channels)
        {
            int sum_r = 0;
            int sum_g = 0;
            int sum_b = 0;
            int count = 0;
            for (int k = -radius; k < (int)radius; k++)
            {
                if ((int)height <= i + k || 0 > i + k)
                {
                    continue;
                }
                for (int l = -radius; l < (int)radius; l++)
                {
                    if (width_compensated <= j + l || 0 > j + l)
                    {
                        continue;
                    }

                    ++count;
                    sum_r += image[width_compensated * (i + k) + (j + l) + 0];
                    sum_g += image[width_compensated * (i + k) + (j + l) + 1];
                    sum_b += image[width_compensated * (i + k) + (j + l) + 2];
                }
            }
            image_copy[width_compensated * i + j + 0] = sum_r / count;
            image_copy[width_compensated * i + j + 1] = sum_g / count;
            image_copy[width_compensated * i + j + 2] = sum_b / count;
            // image_copy[width * i + j] = 255;
        }
    }

    std::copy(image, image + width_compensated * height * sizeof(unsigned char), image_copy);
    free(image_copy);
}
