#include <glad/glad.h>
// put GLFW after glad
#include <GLFW/glfw3.h>

#include "file_getter.h"
#include "image.h"
#include "image_renderer.h"

#include <iostream>

#define SIZE_OF_FOREGROUND_IMAGE -50, 50
#define SIZE_OF_BACKGROUND_IMAGE -100, 100

static const char* vert_shader_file_path = "/home/ishay320/Desktop/slideshow/shaders/shader.vs";
static const char* frag_shader_file_path = "/home/ishay320/Desktop/slideshow/shaders/shader.fs";

const char* g_image_path[] = {"pics/cat2.jpg", "pics/closup-of-cat-on-floor.jpg", "pics/inside/test.jpg"};

void framebuffersSizeCallback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void clearBackground();

void initOpenGL();
GLFWwindow* initWindow(const char* window_name, int width, int height, GLFWframebuffersizefun window_resize_callback);

// settings
const unsigned int c_screen_width  = 800;
const unsigned int c_screen_height = 600;

double g_image_swap_time = 3.0;
typedef enum
{
    RENDER_MODE_SHOW,
    RENDER_MODE_TRANSITION_OUT,
    RENDER_MODE_TRANSITION_IN,
    COUNT_RENDER_MODE
} RendererMode;

void swapImages(ImageRenderer& image_renderer, FileGetter::ImageBuffer& image_buffer)
{
    const size_t width  = 1920;
    const size_t height = 1080;
    Image image         = resizeToMax(image_buffer.getNext(), width, height);

    Image background{image};
    fastGaussianBlur(background, 15);

    image_renderer.popImage();
    image_renderer.popImage();
    image_renderer.pushImage(image, SIZE_OF_FOREGROUND_IMAGE);
    image_renderer.pushImage(background, SIZE_OF_BACKGROUND_IMAGE);
}

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

    RendererMode render_mode = RENDER_MODE_SHOW;
    double time_last         = glfwGetTime();
    while (!glfwWindowShouldClose(window))
    {
        if (glfwGetTime() - time_last > 1.0)
        {
            time_last = glfwGetTime();

            swapImages(image_renderer, image_buffer);
        }
        processInput(window);

        // background
        clearBackground();

        const double time    = glfwGetTime();
        const float sin_time = sin(time);

        // create transformations
        image_renderer.resetTransform(0);
        image_renderer.translate(0, {sin_time * -100.0, 0});
        image_renderer.resetTransform(1);
        image_renderer.translate(1, {sin_time * 100.0, 0});

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