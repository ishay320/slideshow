#include <glad/glad.h>
// put GLFW after glad
#include <GLFW/glfw3.h>

#include <iostream>

#include "stb_image.h"
#include "texture.h"

Texture::Texture(const Image& image) { setTexture(image); }

Texture::Texture(unsigned char* image_data, int width, int height, int channels)
{
    Image image{image_data, width, height, channels};

    setTexture(image);
}

Texture::Texture(const char* texture_path)
{
    // load texture
    Image image{texture_path};

    setTexture(image);
}

void Texture::setTexture(const Image& image)
{
    glGenTextures(1, &_texture);

    glBindTexture(GL_TEXTURE_2D, _texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.getWidth(), image.getHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, image.getData());
    glGenerateMipmap(GL_TEXTURE_2D);
}

void Texture::bind()
{
    glActiveTexture(GL_TEXTURE0); // if multi texture are loaded then choose
    glBindTexture(GL_TEXTURE_2D, _texture);
}

Texture::~Texture() {}
