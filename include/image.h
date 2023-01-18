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
    Image(const char* filename, bool flip = true);

    Image(unsigned char* image_data, int width, int height, int channels);

    int getWidth() const;
    int getHeight() const;
    int getChannels() const;

    inline Pixel* getPixel(int x, int y) const;

    ~Image();
    unsigned char* getData() const;

private:
    unsigned char* _image_data;
    int _width, _height, _channels;
};

void removeChannel(Image& image, bool remove_r, bool remove_g, bool remove_b);

/* WIP */
void blurImage(unsigned char* image, unsigned int radius, unsigned int width, unsigned int height, unsigned int channels);

#endif // IMAGE_H