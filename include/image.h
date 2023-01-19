#ifndef IMAGE_H
#define IMAGE_H
#include "stb_image.h"

struct Pixel
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
};

class Image
{
public:
    /**
     * @brief Move constructor
     *
     * @param other
     */
    Image(Image&& other);

    Image(const char* filename, bool flip = true);

    Image(unsigned char* image_data, int width, int height, int channels);

    /**
     * @brief Construct a new empty image
     *
     * @param width
     * @param height
     * @param channels
     */
    Image(int width, int height, int channels);

    ~Image();

    int getWidth() const;
    int getHeight() const;
    int getChannels() const;
    unsigned char* getData() const;

    inline Pixel* getPixel(int x, int y) const;

    bool isEmpty() const;

    /**
     * @brief Move operator
     *
     * @param other
     * @return Image&
     */
    Image& operator=(Image&& other);

private:
    unsigned char* _image_data;
    int _width, _height, _channels;
    bool _file = true;
};

Image resize(const Image& image, int width, int height);

void fillColor(Image& image, const Pixel& color);

void removeChannel(Image& image, bool remove_r, bool remove_g, bool remove_b);

/**
 * @brief original box blur (slow)
 *
 * @param image IO replace with filtered
 * @param radius
 */
void boxBlurImage(Image& image, unsigned int radius);

#endif // IMAGE_H