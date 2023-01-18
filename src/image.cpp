#include "image.h"

#include <iostream>

Image::Image(Image&& other) : _image_data(other._image_data), _width(other._width), _height(other._height), _channels(other._channels)
{
    other._width      = 0;
    other._height     = 0;
    other._channels   = 0;
    other._image_data = nullptr;
}

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

Image::Image(int width, int height, int channels) : _width(width), _height(height), _channels(channels)
{
    _file       = false;
    _image_data = (unsigned char*)malloc(_width * _height * _channels * sizeof(unsigned char));
    if (_image_data == nullptr)
    {
        std::cerr << __FILE__ << ':' << __LINE__ << " "
                  << "[ERROR] image could not created" << '\n';
    }
}

Image::~Image()
{
    if (_image_data != nullptr)
    {

        stbi_image_free(_image_data);
        _image_data = nullptr;
    }
}

int Image::getWidth() const { return _width; }
int Image::getHeight() const { return _height; }
int Image::getChannels() const { return _channels; }

bool Image::isEmpty() const { return getData() == nullptr; }

inline Pixel* Image::getPixel(int x, int y) const { return (Pixel*)&_image_data[_channels * (x * _width + y)]; }

unsigned char* Image::getData() const { return _image_data; }

Image& Image::operator=(Image&& other)
{
    this->~Image();
    _width      = other._width;
    _height     = other._height;
    _channels   = other._channels;
    _image_data = other._image_data;

    other._width      = 0;
    other._height     = 0;
    other._channels   = 0;
    other._image_data = nullptr;

    return *this;
}

void fillColor(Image& image, Pixel color)
{
    for (int i = 0; i < image.getHeight(); i++)
    {
        for (int j = 0; j < image.getWidth(); j++)
        {
            Pixel* pixel = image.getPixel(i, j);
            *pixel       = color;
        }
    }
}
void removeChannel(Image& image, bool remove_r, bool remove_g, bool remove_b)
{
    for (int i = 0; i < image.getHeight(); i++)
    {
        for (int j = 0; j < image.getWidth(); j++)
        {
            Pixel* pixel = image.getPixel(i, j);

            if (remove_r)
                pixel->r = 255;
            if (remove_g)
                pixel->g = 0;
            if (remove_b)
                pixel->b = 0;
        }
    }
}

void boxBlurImage(Image& image, unsigned int radius)
{

    Image image_out{image.getWidth(), image.getHeight(), image.getChannels()};

    if (image.isEmpty())
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

    for (int i = 0; i < image.getHeight(); i++)
    {
        for (int j = 0; j < image.getWidth(); j++)
        {
            int sum_r = 0;
            int sum_g = 0;
            int sum_b = 0;
            int count = 0;
            for (int k = -radius; k < (int)radius; k++)
            {
                if (image.getHeight() <= i + k || 0 > i + k)
                {
                    continue;
                }
                for (int l = -radius; l <= (int)radius; l++)
                {
                    if (image.getWidth() <= j + l || 0 > j + l)
                    {
                        continue;
                    }

                    ++count;
                    sum_r += image.getPixel((i + k), (j + l))->r;
                    sum_g += image.getPixel((i + k), (j + l))->g;
                    sum_b += image.getPixel((i + k), (j + l))->b;
                }
            }
            image_out.getPixel(i, j)->r = sum_r / count;
            image_out.getPixel(i, j)->g = sum_g / count;
            image_out.getPixel(i, j)->b = sum_b / count;
        }
    }

    image = std::move(image_out);
}
