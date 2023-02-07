#include "image.h"

#include <iostream>
#include <math.h>
#include <string.h>

Image::Image(Image&& other) : _image_data(other._image_data), _width(other._width), _height(other._height), _channels(other._channels)
{
    other._width      = 0;
    other._height     = 0;
    other._channels   = 0;
    other._image_data = nullptr;
}

Image::Image() : _image_data(nullptr), _width(0), _height(0), _channels(0) {}

Image::Image(const Image& other) : _width(other._width), _height(other._height), _channels(other._channels)
{
    const size_t image_size = _width * _height * _channels * sizeof(*_image_data);
    _image_data             = (unsigned char*)malloc(image_size);
    if (_image_data == NULL)
    {
        std::cerr << "ERROR: buy more ram, trying to malloc " << image_size << " bytes\n";
    }
    else
    {
        memcpy(_image_data, other._image_data, image_size);
    }
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

int Image::getWidth() const
{
    return _width;
}
int Image::getHeight() const
{
    return _height;
}
int Image::getChannels() const
{
    return _channels;
}

bool Image::isEmpty() const
{
    return getData() == nullptr;
}

inline Pixel* Image::getPixel(int x, int y) const
{
    return (Pixel*)&_image_data[_channels * (x * _width + y)];
}

unsigned char* Image::getData() const
{
    return _image_data;
}

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

Image& Image::operator=(const Image& other)
{
    this->~Image();
    _width      = other._width;
    _height     = other._height;
    _channels   = other._channels;
    _image_data = other._image_data;

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

static void fast_gaussian_blur_rgb(unsigned char*& in, unsigned char*& out, int w, int h, int c, float sigma);

void fastGaussianBlur(Image& image, unsigned int radius)
{
    Image image_out{image.getWidth(), image.getHeight(), image.getChannels()};
    unsigned char* image_out_data = image_out.getData();
    unsigned char* image_data     = image.getData();
    fast_gaussian_blur_rgb(image_data, image_out_data, image.getWidth(), image.getHeight(), image.getChannels(), radius);
    image = std::move(image_out);
}

void boxBlur(Image& image, unsigned int radius)
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

// ****************************
// ***** static functions *****
// ****************************

static void std_to_box(int boxes[], float sigma, int boxes_size)
{
    // ideal filter width
    int w_l = std::floor(std::sqrt((12 * sigma * sigma / boxes_size) + 1));
    if (w_l % 2 == 0)
    {
        w_l--;
    }
    int w_u = w_l + 2;

    float m_i = (12 * sigma * sigma - boxes_size * w_l * w_l - 4 * boxes_size * w_l - 3 * boxes_size) / (-4 * w_l - 4);
    int m     = std::round(m_i);

    for (int i = 0; i < boxes_size; i++)
    {
        boxes[i] = ((i < m ? w_l : w_u) - 1) / 2;
    }
}
static void horizontal_blur_rgb(unsigned char* in, unsigned char* out, int w, int h, int c, int r)
{
    float iarr = 1.f / (r + r + 1);
#pragma omp parallel for
    for (int i = 0; i < h; i++)
    {
        int ti = i * w;
        int li = ti;
        int ri = ti + r;

        int fv[3]  = {in[ti * c + 0], in[ti * c + 1], in[ti * c + 2]};
        int lv[3]  = {in[(ti + w - 1) * c + 0], in[(ti + w - 1) * c + 1], in[(ti + w - 1) * c + 2]};
        int val[3] = {(r + 1) * fv[0], (r + 1) * fv[1], (r + 1) * fv[2]};

        for (int j = 0; j < r; j++)
        {
            val[0] += in[(ti + j) * c + 0];
            val[1] += in[(ti + j) * c + 1];
            val[2] += in[(ti + j) * c + 2];
        }

        for (int j = 0; j <= r; j++, ri++, ti++)
        {
            val[0] += in[ri * c + 0] - fv[0];
            val[1] += in[ri * c + 1] - fv[1];
            val[2] += in[ri * c + 2] - fv[2];
            out[ti * c + 0] = std::round(val[0] * iarr);
            out[ti * c + 1] = std::round(val[1] * iarr);
            out[ti * c + 2] = std::round(val[2] * iarr);
        }

        for (int j = r + 1; j < w - r; j++, ri++, ti++, li++)
        {
            val[0] += in[ri * c + 0] - in[li * c + 0];
            val[1] += in[ri * c + 1] - in[li * c + 1];
            val[2] += in[ri * c + 2] - in[li * c + 2];
            out[ti * c + 0] = std::round(val[0] * iarr);
            out[ti * c + 1] = std::round(val[1] * iarr);
            out[ti * c + 2] = std::round(val[2] * iarr);
        }

        for (int j = w - r; j < w; j++, ti++, li++)
        {
            val[0] += lv[0] - in[li * c + 0];
            val[1] += lv[1] - in[li * c + 1];
            val[2] += lv[2] - in[li * c + 2];
            out[ti * c + 0] = std::round(val[0] * iarr);
            out[ti * c + 1] = std::round(val[1] * iarr);
            out[ti * c + 2] = std::round(val[2] * iarr);
        }
    }
}
static void total_blur_rgb(unsigned char* in, unsigned char* out, int w, int h, int c, int r)
{
    // radius range on either side of a pixel + the pixel itself
    float iarr = 1.f / (r + r + 1);
#pragma omp parallel for
    for (int i = 0; i < w; i++)
    {
        int ti = i;
        int li = ti;
        int ri = ti + r * w;

        int fv[3]  = {in[ti * c + 0], in[ti * c + 1], in[ti * c + 2]};
        int lv[3]  = {in[(ti + w * (h - 1)) * c + 0], in[(ti + w * (h - 1)) * c + 1], in[(ti + w * (h - 1)) * c + 2]};
        int val[3] = {(r + 1) * fv[0], (r + 1) * fv[1], (r + 1) * fv[2]};

        for (int j = 0; j < r; j++)
        {
            val[0] += in[(ti + j * w) * c + 0];
            val[1] += in[(ti + j * w) * c + 1];
            val[2] += in[(ti + j * w) * c + 2];
        }

        for (int j = 0; j <= r; j++, ri += w, ti += w)
        {
            val[0] += in[ri * c + 0] - fv[0];
            val[1] += in[ri * c + 1] - fv[1];
            val[2] += in[ri * c + 2] - fv[2];
            out[ti * c + 0] = std::round(val[0] * iarr);
            out[ti * c + 1] = std::round(val[1] * iarr);
            out[ti * c + 2] = std::round(val[2] * iarr);
        }

        for (int j = r + 1; j < h - r; j++, ri += w, ti += w, li += w)
        {
            val[0] += in[ri * c + 0] - in[li * c + 0];
            val[1] += in[ri * c + 1] - in[li * c + 1];
            val[2] += in[ri * c + 2] - in[li * c + 2];
            out[ti * c + 0] = std::round(val[0] * iarr);
            out[ti * c + 1] = std::round(val[1] * iarr);
            out[ti * c + 2] = std::round(val[2] * iarr);
        }

        for (int j = h - r; j < h; j++, ti += w, li += w)
        {
            val[0] += lv[0] - in[li * c + 0];
            val[1] += lv[1] - in[li * c + 1];
            val[2] += lv[2] - in[li * c + 2];
            out[ti * c + 0] = std::round(val[0] * iarr);
            out[ti * c + 1] = std::round(val[1] * iarr);
            out[ti * c + 2] = std::round(val[2] * iarr);
        }
    }
}

static void box_blur_rgb(unsigned char*& in, unsigned char*& out, int w, int h, int c, int r)
{
    std::swap(in, out);
    horizontal_blur_rgb(out, in, w, h, c, r);
    total_blur_rgb(in, out, w, h, c, r);
    // Note to myself :
    // here we could go anisotropic with different radiis rx,ry in HBlur and TBlur
}

static void fast_gaussian_blur_rgb(unsigned char*& in, unsigned char*& out, int w, int h, int c, float sigma)
{
    // sigma conversion to box dimensions
    int boxes[3];
    std_to_box(boxes, sigma, 3);
    box_blur_rgb(in, out, w, h, c, boxes[0]);
    box_blur_rgb(out, in, w, h, c, boxes[1]);
    box_blur_rgb(in, out, w, h, c, boxes[2]);
}
