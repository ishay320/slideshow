#include <glad/glad.h>
// put GLFW after glad
#include <GLFW/glfw3.h>
#include <math.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#include "camera.h"
#include "mesh.h"
#include "object.h"
#include "scene.h"
#include "shader.h"
#include "texture.h"
#include "utils.h"

const char* g_vertex_shader_path   = "./shaders/shader.vs";
const char* g_fragment_shader_path = "./shaders/shader.fs";

const char* g_image_path = "pics/cat2.jpg";

void framebuffersSizeCallback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void clearBackground();

// settings
unsigned int g_screen_width  = 800;
unsigned int g_screen_height = 600;

void initOpenGL()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

GLFWwindow* initWindow(const char* window_name, int width, int height, GLFWframebuffersizefun window_resize_callback)
{
    GLFWwindow* window = glfwCreateWindow(width, height, window_name, NULL, NULL);
    if (window == NULL)
    {
        std::cerr << "ERROR: Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return NULL;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, window_resize_callback);
    return window;
}

int main()
{
    initOpenGL();
    GLFWwindow* window = initWindow("test name", g_screen_width, g_screen_height, framebuffersSizeCallback);

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "ERROR: Failed to initialize GLAD" << std::endl;
        return -1;
    }

    Shader shader{g_vertex_shader_path, g_fragment_shader_path};

    float vertices[] = {
        // positions        // colors         // texture coords
        0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top right
        -0.5f, 0.5f,  0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top left
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
        0.5f,  -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom right
    };
    const uint8_t vertex_block_size = 8;
    const uint8_t vertices_number   = (sizeof(vertices) / sizeof(*vertices)) / vertex_block_size;
    unsigned int indices[]          = {
        0, 1, 2, // first triangle
        0, 2, 3, // second triangle
    };

    Mesh mesh{vertices, vertices_number, vertex_block_size, indices, sizeof(indices)};

    Texture texture{g_image_path};

    Object object{shader, mesh, texture};

    Object object2{shader, mesh, texture};

    Camera camera{glm::vec3(0.0f, 0.0f, 1.0f)};

    Scene scene{camera};
    scene.addObject(object);
    scene.addObject(object2);

    // render loop
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        // background
        clearBackground();

        float sin_time = sin(glfwGetTime());
        float cos_time = cos(glfwGetTime());

        // create transformations
        object.resetTransform();
        object.translate(glm::vec3(sin_time, cos_time, 0.0f));
        object.rotate((float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
        object.scale(glm::vec3(sin_time, cos_time, 1.0f));

        object2.resetTransform();
        object2.scale(glm::vec3(sin_time, cos_time, 1.0f));

        // render scene
        scene.render(g_screen_width, g_screen_height);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    object.deallocate();

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
}

void clearBackground()
{
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void framebuffersSizeCallback(GLFWwindow* window, int width, int height)
{
    (void)window;
    glViewport(0, 0, width, height);
    g_screen_width  = width;
    g_screen_height = height;
}