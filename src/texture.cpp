#include <glad/glad.h>
// put GLFW after glad
#include <GLFW/glfw3.h>

#include <iostream>

#include "texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Texture::Texture(const char* texture_path)
{
    // load texture
    int width, height, nr_channels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* image_data = stbi_load(texture_path, &width, &height, &nr_channels, 0);
    if (image_data == 0)
    {
        std::cout << "ERROR: image texture could not load: '" << texture_path << '\'';
    }

    glGenTextures(1, &_texture);

    glBindTexture(GL_TEXTURE_2D, _texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image_data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(image_data);
}
void Texture::bind()
{
    glActiveTexture(GL_TEXTURE0); // if multi texture are loaded then choose
    glBindTexture(GL_TEXTURE_2D, _texture);
}
Texture::~Texture() {}
