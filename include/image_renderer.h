#ifndef IMAGE_RENDERER_H
#define IMAGE_RENDERER_H
#include <glad/glad.h>
// put GLFW after glad
#include <GLFW/glfw3.h>

#include "image.h"
#include "utils.h"
#include <glm/glm.hpp>
#include <stdlib.h>

/**
 * @file ImageRenderer.h
 * @brief Class for rendering images using opengl
 *
 */

// TODO: dynamicly allocate
#define RENDERER_QUAD_SIZE 6
#define RENDERER_MAX_IMAGES 16

typedef struct
{
    glm::vec2 position;
    glm::vec4 color; // not used
    glm::vec2 uv;
} Vertex;

typedef enum
{
    UNIFORM_SLOT_TIME = 0,
    UNIFORM_SLOT_RESOLUTION,
    UNIFORM_SLOT_CAMERA_POS,
    UNIFORM_SLOT_CAMERA_SCALE,
    UNIFORM_SLOT_TRANSFORM,
    UNIFORM_SLOT_OPACITY,
    COUNT_UNIFORM_SLOTS,
} Uniform_Slot;

/**
 * @class ImageRenderer
 * @brief Class for rendering images using opengl
 *
 */
class ImageRenderer
{
public:
    /**
     * @brief Construct a new ImageRenderer object
     *
     * @param vert_shader The vertex shader source code
     * @param frag_shader The fragment shader source code
     */
    ImageRenderer(const char* vert_shader, const char* frag_shader);
    ~ImageRenderer();

    /**
     * @brief Set the Resolution, use this in screen resize callback
     *
     * @param resolution The new resolution
     */
    static void setResolution(glm::vec2 resolution);

    /**
     * @brief Set the time uniform value in the shader
     *
     * @param time The new time value
     */
    void setTime(float time);

    /**
     * @brief Set the opacity uniform value in the shader
     *
     * @param opacity The new opacity value
     */
    void setOpacity(float opacity);

    /**
     * @brief Draw all images on the stack
     *
     */
    void drawImages();

    /**
     * @brief add image to the stack
     *
     * @param image
     * @param x pos
     * @param y pos
     * @return size_t the "pointer" to the image location on the stack
     */
    size_t pushImage(const Image& image, int x, int y);

    /**
     * @brief remove the last image
     *
     * @return size_t the "pointer" to the current biggest image location on the stack
     */
    size_t popImage();

    /**
     * @brief Change the opacity of the image at `pos` to `level`
     *
     * @param pos The position of the image in the stack
     * @param level The level of opacity, range [0, 255]
     */
    void opacityImage(size_t pos, size_t level);

    /**
     * @brief Draw the image at `pos`
     *
     * @param pos The position of the image in the stack
     */
    void drawImage(size_t pos);

    /**
     * @brief Set the transform for the image at `pos`
     *
     * @param pos The position of the image in the stack
     */
    void setTransform(size_t pos);

    /**
     * @brief Reset the transform for the image at `pos`
     *
     * @param pos The position of the image in the stack
     */
    void resetTransform(size_t pos);

    /**
     * @brief Rotate the image at `pos` by `angle` around `rotation_vector`
     *
     * @param pos The position of the image in the stack
     * @param angle The angle to rotate in degree
     */
    void rotate(size_t pos, float angle);

    /**
     * @brief Rotate the image at `pos` by `angle` around `rotation_vector`
     *
     * @param pos The position of the image in the stack
     * @param angle The angle to rotate in radians
     */
    void rotateRad(size_t pos, float angle);

    /**
     * @brief Translate the image at `pos` by `translation_vector`
     *
     * @param pos The position of the image in the stack
     * @param translation_vector The vector to translate by
     */
    void translate(size_t pos, const glm::vec2& translation_vector);

    /**
     * @brief Scale the image at `pos` by `scale_vector`
     *
     * @param pos The position of the image in the stack
     * @param scale_vector The vector to scale by
     */
    void scale(size_t pos, const glm::vec2& scale_vector);

private:
    /**
     * @brief Binds the texture at the specified position.
     *
     * @param pos The position of the texture.
     */
    void bindTexture(size_t pos);

    /**
     * @brief Synchronizes the vertices.
     *        Synchronizes the vertices with the GPU.
     */
    void syncVertices();

    /**
     * @brief Draw a single image at the specified position on the image stack
     *
     * @param image_pos The position of the image on the stack
     */
    void draw(size_t image_pos);

    /**
     * @brief Sets the shader.
     *
     * Sets the shader program for the ImageRenderer.
     */
    void setShader();

    /**
     * @brief Sets a 4x4 matrix as a uniform.
     *
     * @param uniform The uniform slot to set.
     * @param value The value of the 4x4 matrix to set.
     */
    void setMat4(Uniform_Slot uniform, const glm::mat4& value);

    GLuint _vao;
    GLuint _vbo;
    GLuint _program;

    Vertex _vertices[RENDERER_QUAD_SIZE * RENDERER_MAX_IMAGES];
    GLint _uniforms[COUNT_UNIFORM_SLOTS];

    static glm::vec2 _resolution;
    float _time;

    glm::vec2 _camera_pos;
    float _camera_scale;

    // TODO: dynamic array
    unsigned int _texture[RENDERER_MAX_IMAGES];
    glm::mat4 _transforms[RENDERER_MAX_IMAGES];
    size_t _current_image = 0;
};

#endif // IMAGE_RENDERER_H