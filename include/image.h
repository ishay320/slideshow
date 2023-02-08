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

    /**
     * @brief Construct NULL Image object
     *
     */
    Image();

    /**
     * @brief Copy constructor
     *
     * @param other
     */
    Image(const Image& other);

    /**
     * @brief Open file constructor
     *
     * @param filename
     * @param flip
     */
    Image(const char* filename, bool flip = true);

    /**
     * @brief Construct a new Image object from data (will not malloc)
     *
     * @param image_data
     * @param width
     * @param height
     * @param channels
     */
    Image(unsigned char* image_data, int width, int height, int channels);

    /**
     * @brief New empty image constructor
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

    inline Pixel* getPixel(size_t height, size_t width) const;

    bool isEmpty() const;

    /**
     * @brief Move operator
     *
     * @param other
     * @return Image&
     */
    Image& operator=(Image&& other);

    /**
     * @brief Copy operator
     *
     * @param other
     * @return Image&
     */
    Image& operator=(const Image& other);

private:
    unsigned char* _image_data;
    int _width, _height, _channels;
};

/**
 * @brief Resize the image
 *
 * @param image
 * @param width
 * @param height
 * @return Image
 */
Image resize(const Image& image, int width, int height);

/**
 * @brief Fill the image with selected color
 *
 * @param image
 * @param color
 */
void fillColor(Image& image, const Pixel& color);

/**
 * @brief Set selected color to 0
 *
 * @param image
 * @param remove_r
 * @param remove_g
 * @param remove_b
 */
void removeChannel(Image& image, bool remove_r, bool remove_g, bool remove_b);

/**
 * @brief Fast gaussian blur
 * code from https://gist.github.com/bfraboni/946d9456b15cac3170514307cf032a27 under MIT
 *
 * @param image
 * @param radius
 */
void fastGaussianBlur(Image& image, unsigned int radius);

/**
 * @brief Original box blur (slow)
 *
 * @param image IO replace with filtered
 * @param radius
 */
void boxBlur(Image& image, unsigned int radius);

#endif // IMAGE_H