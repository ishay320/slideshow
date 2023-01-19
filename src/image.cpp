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

/*
***********************************
************ functions ************
***********************************
*/

Image resize(const Image& image, int width, int height)
{
    Image image_out{width, height, image.getChannels()};

    float height_multiplier = image.getHeight() / height;
    float width_multiplier  = image.getWidth() / width;
    for (int row = 0; row < height; row++)
    {
        for (int column = 0; column < width; column++)
        {
            *image_out.getPixel(row, column) = *image.getPixel(row * height_multiplier, column * width_multiplier);
        }
    }
    return image_out;
}

void fillColor(Image& image, const Pixel& color)
{
    for (int row = 0; row < image.getHeight(); row++)
    {
        for (int column = 0; column < image.getWidth(); column++)
        {
            Pixel* pixel = image.getPixel(row, column);
            *pixel       = color;
        }
    }
}
void removeChannel(Image& image, bool remove_r, bool remove_g, bool remove_b)
{
    for (int row = 0; row < image.getHeight(); row++)
    {
        for (int column = 0; column < image.getWidth(); column++)
        {
            Pixel* pixel = image.getPixel(row, column);

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

    for (int row = 0; row < image.getHeight(); row++)
    {
        for (int column = 0; column < image.getWidth(); column++)
        {
            int sum_r = 0;
            int sum_g = 0;
            int sum_b = 0;
            int count = 0;
            for (int k = -radius; k < (int)radius; k++)
            {
                if (image.getHeight() <= row + k || 0 > row + k)
                {
                    continue;
                }
                for (int l = -radius; l <= (int)radius; l++)
                {
                    if (image.getWidth() <= column + l || 0 > column + l)
                    {
                        continue;
                    }

                    ++count;
                    sum_r += image.getPixel((row + k), (column + l))->r;
                    sum_g += image.getPixel((row + k), (column + l))->g;
                    sum_b += image.getPixel((row + k), (column + l))->b;
                }
            }
            image_out.getPixel(row, column)->r = sum_r / count;
            image_out.getPixel(row, column)->g = sum_g / count;
            image_out.getPixel(row, column)->b = sum_b / count;
        }
    }

    image = std::move(image_out);
}
