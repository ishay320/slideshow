#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define HEXCOLOR(code) ((code) >> (3 * 8)) & 0xFF, ((code) >> (2 * 8)) & 0xFF, ((code) >> (1 * 8)) & 0xFF, ((code) >> (0 * 8)) & 0xFF

#define MEASURE(function)                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              \
    {                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  \
        clock_t start, end;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            \
        double cpu_time_used;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          \
        start = clock();                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               \
        (function);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    \
        end = clock();                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 \
        cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      \
        printf("function time %f\n", cpu_time_used);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   \
    }

#define BACKGROUND_COLOR 0x181818FF

#define WIDTH 800
#define HEIGHT 600
typedef struct {
    unsigned char r;
    unsigned char g;
    unsigned char b;
} Pixel;

void Get_Screen_Size(int *Width, int *Height) {
    SDL_DisplayMode DM;
    SDL_GetCurrentDisplayMode(0, &DM);
    *Width = DM.w;
    *Height = DM.h;
}

void Image_Blur(SDL_Surface *image, int radius) { // TODO: no alfa so 3 channel need to check how many
    SDL_LockSurface(image);
    for (size_t i = 0; i < image->h * image->w; i++) {
        Pixel *pixels = (Pixel *)image->pixels;
        Pixel pixel_return = {.r = 0, .g = 0, .b = 255};
        unsigned char *color;
        color = (unsigned char *)(pixels + i);
        int sum_r = 0;
        int sum_g = 0;
        int sum_b = 0;

        for (size_t j = 0; j < radius; j++) {
            for (size_t k = 0; k < radius; k++) {
                int row_pos = (int)((float)radius / 2);
                sum_r += pixels[i + j - row_pos + (image->w * (k - row_pos))].r;
                sum_g += pixels[i + j - row_pos + (image->w * (k - row_pos))].g;
                sum_b += pixels[i + j - row_pos + (image->w * (k - row_pos))].b;
            }
        }
        pixel_return = *(Pixel *)color;
        pixel_return.r = sum_r / (radius * radius);
        pixel_return.g = sum_g / (radius * radius);
        pixel_return.b = sum_b / (radius * radius);
        pixels[i] = pixel_return;
    }
    SDL_UnlockSurface(image);
}

int main(void) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "ERROR: could not initialize SDL: %s\n", SDL_GetError());
        exit(1);
    }
    SDL_DisableScreenSaver();

    int Width = 0;
    int Height = 0;
    Get_Screen_Size(&Width, &Height);
    SDL_Window *window = SDL_CreateWindow("slider", 0, 0, Width, Height, SDL_WINDOW_RESIZABLE); // SDL_WINDOW_FULLSCREEN
    if (window == NULL) {
        fprintf(stderr, "ERROR: could not create a window: %s\n", SDL_GetError());
        exit(1);
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        fprintf(stderr, "ERROR: could not create a renderer: %s\n", SDL_GetError());
        exit(1);
    }

    // load our image
    SDL_Texture *background_texture = NULL;
    SDL_Texture *forground_texture = NULL;
    int w, h; // texture width & height

    // background_texture = IMG_LoadTexture(renderer, "pics/closup-of-cat-on-floor-julie-austin-pet-photography.jpg"); // SDL_TEXTUREACCESS_STREAMING need to be

    SDL_Surface *image = IMG_Load("pics/closup-of-cat-on-floor-julie-austin-pet-photography.jpg");
    if (image == NULL) {
        printf("err- %s", SDL_GetError());
        exit(1);
    }

    forground_texture = SDL_CreateTextureFromSurface(renderer, image);
    if (forground_texture == NULL) {
        printf("err- %s", SDL_GetError());
        exit(1);
    }

    MEASURE(Image_Blur(image, 9))

    background_texture = SDL_CreateTextureFromSurface(renderer, image);
    if (background_texture == NULL) {
        printf("err- %s", SDL_GetError());
        exit(1);
    }

    SDL_QueryTexture(background_texture, NULL, NULL, &w, &h); // get the width and height of the texture

    // put the location where we want the texture to be drawn into a rectangle
    SDL_Rect backcground_rect;
    backcground_rect.x = 0;
    backcground_rect.y = 0;
    SDL_Rect forground_rect;
    forground_rect.x = 50;
    forground_rect.y = 50;

    // set the size of the pic TODO: dynamicly change because image have different sizes
    float width_relation, hight_relation;
    int window_hight, window_width = 0;
    SDL_GetWindowSize(window, &window_width, &window_hight);
    width_relation = (float)window_width / (float)w;
    hight_relation = (float)window_hight / (float)h;

    if (hight_relation < width_relation) {
        backcground_rect.h = h * hight_relation;
        backcground_rect.w = w * hight_relation;
    } else {
        backcground_rect.h = h * width_relation;
        backcground_rect.w = w * width_relation;
    }

    SDL_SetRenderDrawColor(renderer, HEXCOLOR(BACKGROUND_COLOR));

    unsigned char *pixels = NULL;
    int pitch = 0;

    // int err = SDL_LockTexture(background_texture, NULL, (void **)&pixels, &pitch);
    // (void)err;
    // set pixels to solid white
    for (int i = 0; i < pitch; i++) {
        pixels[i] = 255;
    }
    // SDL_UnlockTexture(background_texture);

    bool quit = false;
    while (!quit) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_QUIT: {
                quit = true;
            } break;
            }
        }

        // clear the screen
        SDL_RenderClear(renderer);
        // copy the texture to the rendering context
        SDL_RenderCopy(renderer, background_texture, NULL, &backcground_rect);
        SDL_RenderCopy(renderer, forground_texture, NULL, &forground_rect);
        // flip the backbuffer
        // this means that everything that we prepared behind the screens is actually shown
        SDL_RenderPresent(renderer);
    }

    SDL_Quit();

    return 0;
}
