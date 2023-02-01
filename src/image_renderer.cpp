#include "image_renderer.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// default resolution
glm::vec2 ImageRenderer::_resolution = {600, 800};

/**
 * @brief convert shader to name
 *
 * @param shader
 * @return const char*
 */
static const char* shaderTypeAsCstr(GLuint shader)
{
    switch (shader)
    {
        case GL_VERTEX_SHADER:
            return "GL_VERTEX_SHADER";
        case GL_FRAGMENT_SHADER:
            return "GL_FRAGMENT_SHADER";
        default:
            return "(Unknown)";
    }
}

static bool compileShaderSource(const GLchar* source, GLenum shader_type, GLuint* shader)
{
    *shader = glCreateShader(shader_type);
    glShaderSource(*shader, 1, &source, NULL);
    glCompileShader(*shader);

    GLint is_compiled = 0;
    glGetShaderiv(*shader, GL_COMPILE_STATUS, &is_compiled);

    if (!is_compiled)
    {
        GLchar message[1024];
        GLsizei message_size = 0;
        glGetShaderInfoLog(*shader, sizeof(message), &message_size, message);
        fprintf(stderr, "ERROR: could not compile %s\n", shaderTypeAsCstr(shader_type));
        fprintf(stderr, "%.*s\n", message_size, message);
        return false;
    }

    return true;
}

static bool compileShaderFile(const char* file_path, GLenum shader_type, GLuint* shader_out)
{
    const std::string code = utils::readFile(file_path);

    if (code == "")
    {
        fprintf(stderr, "ERROR: failed to load `%s` shader file: %s\n", file_path, strerror(errno));
        return false;
    }

    const char* c_str = code.c_str();
    if (!compileShaderSource(c_str, shader_type, shader_out))
    {
        fprintf(stderr, "ERROR: failed to compile `%s` shader file\n", file_path);
        return false;
    }
    return true;
}

static void attachShadersToProgram(GLuint* shaders, size_t shaders_count, GLuint program)
{
    for (size_t i = 0; i < shaders_count; ++i)
    {
        glAttachShader(program, shaders[i]);
    }
}

static bool linkProgram(GLuint _program, const char* file_path, size_t line)
{
    glLinkProgram(_program);

    GLint linked = 0;
    glGetProgramiv(_program, GL_LINK_STATUS, &linked);
    if (!linked)
    {
        GLsizei message_size = 0;
        GLchar message[1024];

        glGetProgramInfoLog(_program, sizeof(message), &message_size, message);
        fprintf(stderr, "%s:%zu: Program Linking: %.*s\n", file_path, line, message_size, message);
    }

    return linked;
}

typedef enum
{
    VERTEX_ATTR_POSITION = 0,
    VERTEX_ATTR_COLOR,
    VERTEX_ATTR_UV,
} Vertex_Attr;

ImageRenderer::ImageRenderer(const char* vert_shader_path, const char* frag_shader_path)
{
    _camera_scale = 3.0f;
    _camera_pos   = {0.f, 0.f};

    {
        glGenVertexArrays(1, &_vao);
        glBindVertexArray(_vao);

        glGenBuffers(1, &_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(_vertices), _vertices, GL_DYNAMIC_DRAW);

        // position
        glEnableVertexAttribArray(VERTEX_ATTR_POSITION);
        glVertexAttribPointer(VERTEX_ATTR_POSITION, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, position));

        // color
        glEnableVertexAttribArray(VERTEX_ATTR_COLOR);
        glVertexAttribPointer(VERTEX_ATTR_COLOR, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, color));

        // uv
        glEnableVertexAttribArray(VERTEX_ATTR_UV);
        glVertexAttribPointer(VERTEX_ATTR_UV, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, uv));

        GLuint shaders[2] = {0};

        if (!compileShaderFile(vert_shader_path, GL_VERTEX_SHADER, &shaders[0]))
        {
            exit(1);
        }

        if (!compileShaderFile(frag_shader_path, GL_FRAGMENT_SHADER, &shaders[1]))
        {
            exit(1);
        }
        _program = glCreateProgram();
        attachShadersToProgram(shaders, sizeof(shaders) / sizeof(shaders[0]), _program);
        if (!linkProgram(_program, __FILE__, __LINE__))
        {
            exit(1);
        }
        setShader();
    }

    for (size_t i = 0; i < RENDERER_MAX_IMAGES; i++)
    {
        resetTransform(i);
    }
}

ImageRenderer::~ImageRenderer() {}

typedef struct
{
    Uniform_Slot slot;
    const char* name;
} UniformDef;

static_assert(COUNT_UNIFORM_SLOTS == 5, "The amount of the shader uniforms have change. Please update the definition table accordingly");
static const UniformDef uniform_defs[COUNT_UNIFORM_SLOTS] = {
    [UNIFORM_SLOT_TIME] =
        {
            .slot = UNIFORM_SLOT_TIME,
            .name = "time",
        },
    [UNIFORM_SLOT_RESOLUTION] =
        {
            .slot = UNIFORM_SLOT_RESOLUTION,
            .name = "resolution",
        },
    [UNIFORM_SLOT_CAMERA_POS] =
        {
            .slot = UNIFORM_SLOT_CAMERA_POS,
            .name = "camera_pos",
        },
    [UNIFORM_SLOT_CAMERA_SCALE] =
        {
            .slot = UNIFORM_SLOT_CAMERA_SCALE,
            .name = "camera_scale",
        },
    [UNIFORM_SLOT_TRANSFORM] =
        {
            .slot = UNIFORM_SLOT_TRANSFORM,
            .name = "transform",
        },
};

static void get_uniform_location(GLuint program, GLint locations[COUNT_UNIFORM_SLOTS])
{
    for (int slot = 0; slot < COUNT_UNIFORM_SLOTS; slot++)
    {
        locations[slot] = glGetUniformLocation(program, uniform_defs[slot].name);
    }
}

void ImageRenderer::setShader()
{
    glUseProgram(_program);
    get_uniform_location(_program, _uniforms);

    // Set default uniform
    glUniform2f(_uniforms[UNIFORM_SLOT_RESOLUTION], _resolution.x, _resolution.y);
    glUniform1f(_uniforms[UNIFORM_SLOT_TIME], _time);
    glUniform2f(_uniforms[UNIFORM_SLOT_CAMERA_POS], _camera_pos.x, _camera_pos.y);
    glUniform1f(_uniforms[UNIFORM_SLOT_CAMERA_SCALE], _camera_scale);
}

static void addVertex(glm::vec2 p, glm::vec4 c, glm::vec2 uv, Vertex* vertex)
{
    vertex->position = p;
    vertex->color    = c;
    vertex->uv       = uv;
}

static void addTriangle(glm::vec2 p0, glm::vec2 p1, glm::vec2 p2, glm::vec4 c0, glm::vec4 c1, glm::vec4 c2, glm::vec2 uv0, glm::vec2 uv1,
                        glm::vec2 uv2, Vertex* vertex)
{
    addVertex(p0, c0, uv0, vertex + 0);
    addVertex(p1, c1, uv1, vertex + 1);
    addVertex(p2, c2, uv2, vertex + 2);
}

static void addQuad(glm::vec2 p0, glm::vec2 p1, glm::vec2 p2, glm::vec2 p3, glm::vec4 c0, glm::vec4 c1, glm::vec4 c2, glm::vec4 c3, glm::vec2 uv0,
                    glm::vec2 uv1, glm::vec2 uv2, glm::vec2 uv3, Vertex* vertex)
{
    addTriangle(p0, p1, p2, c0, c1, c2, uv0, uv1, uv2, vertex + 0);
    addTriangle(p1, p2, p3, c1, c2, c3, uv1, uv2, uv3, vertex + 3);
}

void ImageRenderer::drawImages()
{
    glUniform2f(_uniforms[UNIFORM_SLOT_RESOLUTION], _resolution.x, _resolution.y);
    syncVertices();
    for (int i = _current_image - 1; i >= 0; i--)
    {
        bindTexture(i);
        draw(i);
    }
}

void ImageRenderer::syncVertices()
{
    const size_t vertexes_to_sync = _current_image * RENDERER_QUAD_SIZE * sizeof(Vertex);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertexes_to_sync, _vertices);
}

void ImageRenderer::draw(size_t image_pos)
{
    setTransform(image_pos);
    glDrawArrays(GL_TRIANGLES, image_pos * RENDERER_QUAD_SIZE, RENDERER_QUAD_SIZE);
}

void ImageRenderer::setResolution(glm::vec2 resolution)
{
    ImageRenderer::_resolution = resolution;
}

static void setTexture(const Image& image, unsigned int* texture)
{
    glGenTextures(1, texture);

    glBindTexture(GL_TEXTURE_2D, *texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.getWidth(), image.getHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, image.getData());
    glGenerateMipmap(GL_TEXTURE_2D);
}

void ImageRenderer::bindTexture(size_t pos)
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _texture[pos]);
}

size_t ImageRenderer::pushImage(const Image& image, int x, int y)
{
    setTexture(image, &_texture[_current_image]);
    // TODO: add image ratio
    addQuad({x, x}, {x, y}, {y, x}, {y, y}, glm::vec4{0}, glm::vec4{0}, glm::vec4{0}, glm::vec4{0}, {0, 0}, {0, 1}, {1, 0}, {1, 1},
            &_vertices[RENDERER_QUAD_SIZE * _current_image]);
    return _current_image++;
}

void ImageRenderer::setMat4(Uniform_Slot uniform, const glm::mat4& value)
{
    glUniformMatrix4fv(_uniforms[uniform], 1, GL_FALSE, &value[0][0]);
}

void ImageRenderer::setTransform(size_t pos)
{
    setMat4(UNIFORM_SLOT_TRANSFORM, _transforms[pos]);
}

void ImageRenderer::resetTransform(size_t pos)
{
    _transforms[pos] = glm::mat4(1.0f); // initialize matrix to identity matrix
}

void ImageRenderer::rotate(size_t pos, float angle, const glm::vec3& rotation_vector)
{
    _transforms[pos] = glm::rotate(_transforms[pos], angle, rotation_vector);
}

void ImageRenderer::translate(size_t pos, const glm::vec3& translation_vector)
{
    _transforms[pos] = glm::translate(_transforms[pos], translation_vector);
}

void ImageRenderer::scale(size_t pos, const glm::vec3& scale_vector)
{
    _transforms[pos] = glm::scale(_transforms[pos], scale_vector);
}