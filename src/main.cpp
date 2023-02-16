#include <glad/glad.h>
// put GLFW after glad
#include <GLFW/glfw3.h>

#include "effect.h"
#include "file_getter.h"
#include "image.h"
#include "image_renderer.h"
#include "inline_logger.h"

#include <array>
#include <future>
#include <iostream>

static const char* vert_shader_file_path = "/home/ishay320/Desktop/slideshow/shaders/shader.vs";
static const char* frag_shader_file_path = "/home/ishay320/Desktop/slideshow/shaders/shader.fs";

void framebuffersSizeCallback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void clearBackground();

void initOpenGL();
GLFWwindow* initWindow(const char* window_name, int width, int height, GLFWframebuffersizefun window_resize_callback);

// settings
const unsigned int c_screen_width  = 800;
const unsigned int c_screen_height = 600;

const double c_image_swap_time = 5.0;

int main(void)
{
    // Init GLFW openGL window
    initOpenGL();
    GLFWwindow* window = initWindow("slideshow", c_screen_width, c_screen_height, framebuffersSizeCallback);

    ImageRenderer::setResolution(glm::vec2{c_screen_width, c_screen_height});

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "ERROR: Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Start of the renderer
    const char* types[] = {".jpg", ".png"};
    FileGetter::LocalFileGetter image_getter{".", types, sizeof(types) / sizeof(*types)};
    FileGetter::ImageBuffer image_buffer{image_getter};

    image_getter.refreshDatabase();

    ImageRenderer image_renderer{vert_shader_file_path, frag_shader_file_path};

    Effect effect(image_renderer, image_buffer, c_image_swap_time);

    std::future<std::array<Image, 2>> images_async;
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        effect.update();

        // render scene
        effect.render();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

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

void framebuffersSizeCallback(GLFWwindow* window, int width, int height)
{
    (void)window;
    glViewport(0, 0, width, height);
    ImageRenderer::setResolution(glm::vec2{width, height});
}

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