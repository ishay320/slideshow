#ifndef IMAGE_RENDERER_H
#define IMAGE_RENDERER_H
#include <glad/glad.h>
// put GLFW after glad
#include <GLFW/glfw3.h>

#include "utils.h"
#include <glm/glm.hpp>
#include <stdlib.h>

typedef struct
{
    glm::vec2 position;
    glm::vec4 color;
    glm::vec2 uv;
} Vertex;

// TODO: dynamicly allocate
#define VERTICES_CAP (3 * 640)

typedef enum
{
    UNIFORM_SLOT_TIME = 0,
    UNIFORM_SLOT_RESOLUTION,
    UNIFORM_SLOT_CAMERA_POS,
    UNIFORM_SLOT_CAMERA_SCALE,
    COUNT_UNIFORM_SLOTS,
} Uniform_Slot;

typedef enum
{
    SHADER_FOR_COLOR = 0,
    SHADER_FOR_IMAGE,
    SHADER_FOR_TEXT,
    SHADER_FOR_EPICNESS, // This is the one that does that cool rainbowish animation
    COUNT_SIMPLE_SHADERS,
} Shader;

class ImageRenderer
{
public:
    ImageRenderer(const char* vert_shader, const char* frag_shader);
    ~ImageRenderer();
    void addTriangle(glm::vec2 p0, glm::vec2 p1, glm::vec2 p2, glm::vec4 c0, glm::vec4 c1, glm::vec4 c2, glm::vec2 uv0, glm::vec2 uv1, glm::vec2 uv2);
    void addQuad(glm::vec2 p0, glm::vec2 p1, glm::vec2 p2, glm::vec2 p3, glm::vec4 c0, glm::vec4 c1, glm::vec4 c2, glm::vec4 c3, glm::vec2 uv0,
                 glm::vec2 uv1, glm::vec2 uv2, glm::vec2 uv3);
    void setShader(uint shader_number);
    void addVertex(glm::vec2 p, glm::vec4 c, glm::vec2 uv);
    void image_rect(glm::vec2 p, glm::vec2 s, glm::vec2 uvp, glm::vec2 uvs, glm::vec4 c);
    void flush();
    void sync();
    void draw();
    void setResolution(glm::vec2 resolution);
    void setTime(float time);

private:
    GLuint _vao;
    GLuint _vbo;
    GLuint _program;
    Shader _current_shader;

    GLint _uniforms[COUNT_UNIFORM_SLOTS];
    Vertex _vertices[VERTICES_CAP];
    size_t _vertices_count;

    glm::vec2 _resolution = {600, 800};
    float _time;

    glm::vec2 _camera_pos;
    float _camera_scale;
};

#endif // IMAGE_RENDERER_H