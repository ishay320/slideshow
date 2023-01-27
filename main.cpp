#include <glad/glad.h>
// put GLFW after glad
#include <GLFW/glfw3.h>

#include "image_renderer.h"

#include <iostream>

static const char* vert_shader_file_path = "/home/ishay320/Desktop/slideshow/shaders/shader.vs";
static const char* frag_shader_file_path = "/home/ishay320/Desktop/slideshow/shaders/shader.fs";

const char* g_image_path[3] = {"pics/cat2.jpg", "pics/closup-of-cat-on-floor.jpg", "pics/inside/test.jpg"};

void framebuffersSizeCallback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void clearBackground();

void initOpenGL();
GLFWwindow* initWindow(const char* window_name, int width, int height, GLFWframebuffersizefun window_resize_callback);

// settings
unsigned int g_screen_width  = 800;
unsigned int g_screen_height = 600;

int main(int argc, char const* argv[])
{
    initOpenGL();
    GLFWwindow* window = initWindow("test name", g_screen_width, g_screen_height, framebuffersSizeCallback);

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "ERROR: Failed to initialize GLAD" << std::endl;
        return -1;
    }

    ImageRenderer image_renderer{vert_shader_file_path, frag_shader_file_path};
    image_renderer.addTriangle({0, 0}, {0, 30}, {30, 1}, {155, 155, 155, 155}, {155, 155, 155, 155}, {155, 155, 155, 155}, {0, 0}, {0, 0}, {0, 0});
    image_renderer.setShader(0);
    while (!glfwWindowShouldClose(window))
    {

        processInput(window);

        // background
        clearBackground();

        float sin_time = sin(glfwGetTime());
        float cos_time = cos(glfwGetTime());

        // create transformations

        // render scene
        image_renderer.sync();
        image_renderer.draw();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // object.deallocate();

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