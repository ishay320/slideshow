#include <glad/glad.h>
// put GLFW after glad
#include <GLFW/glfw3.h>

#include "image.h"
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
const unsigned int c_screen_width  = 800;
const unsigned int c_screen_height = 600;

int main(int argc, char const* argv[])
{
    initOpenGL();
    GLFWwindow* window = initWindow("slideshow", c_screen_width, c_screen_height, framebuffersSizeCallback);

    ImageRenderer::setResolution(glm::vec2{c_screen_width, c_screen_height});

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "ERROR: Failed to initialize GLAD" << std::endl;
        return -1;
    }

    ImageRenderer image_renderer{vert_shader_file_path, frag_shader_file_path};
    size_t pos1 = image_renderer.pushImage(Image{g_image_path[0]}, 0, 50);
    size_t pos2 = image_renderer.pushImage(Image{g_image_path[1]}, -100, 100);

    double last_time = glfwGetTime();

    while (!glfwWindowShouldClose(window))
    {

        processInput(window);

        // background
        clearBackground();

        float sin_time = sin(glfwGetTime());
        float cos_time = cos(glfwGetTime());

        // create transformations
        image_renderer.resetTransform(0);
        image_renderer.scale(0, {cos_time, 1, 1});
        image_renderer.resetTransform(1);
        image_renderer.translate(1, {sin_time, 0, 0});

        // render scene
        image_renderer.drawImages();

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