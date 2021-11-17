#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define DEBUG

#ifdef DEBUG
#define MEASURE(function)                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              \
    clock_t start = clock();                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           \
    (function);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        \
    printf("function time %f\n", ((double)(clock() - start)) / CLOCKS_PER_SEC);
#else
#define MEASURE(function) (function);
#endif

#define HEXCOLOR(code) ((code) >> (3 * 8)) & 0xFF, ((code) >> (2 * 8)) & 0xFF, ((code) >> (1 * 8)) & 0xFF, ((code) >> (0 * 8)) & 0xFF

#define BACKGROUND_COLOR 0x181818FF

#define WIDTH 800
#define HEIGHT 600
#define SPEED 0.5
typedef struct {
    unsigned char r;
    unsigned char g;
    unsigned char b;
} Pixel;

typedef struct {
    SDL_Rect rect;
    float x;
    float y;
} Rect;

void Get_Screen_Size(int *Width, int *Height) {
    SDL_DisplayMode DM;
    SDL_GetCurrentDisplayMode(0, &DM);
    *Width = DM.w;
    *Height = DM.h;
}

void animate_rect(Rect *rect, int seed, float speed) {
    srand(seed); // TODO: #1 save the random arg somewhere to make it faster to render
    rect->x += speed * (((float)rand() / RAND_MAX) - 0.5);
    rect->y += speed * (((float)rand() / RAND_MAX) - 0.5);
    rect->rect.x = (int)rect->x;
    rect->rect.y = (int)rect->y;
}
/*
    TODO #2:
        [ ] - no alfa so 3 channel need to check how many
        [ ] - make it faster
 */
void Image_Blur(SDL_Surface *image, int radius) {
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

void resize_rect(SDL_Texture *texture, Rect *rect, SDL_Window *window) {
    int w, h; // texture width & height

    SDL_QueryTexture(texture, NULL, NULL, &w, &h); // get the width and height of the texture

    // set the size of the pic TODO: dynamicly change because image have different sizes
    float width_relation, hight_relation;
    int window_hight, window_width = 0;
    SDL_GetWindowSize(window, &window_width, &window_hight); // can be done once
    width_relation = (float)window_width / (float)w;
    hight_relation = (float)window_hight / (float)h;

    if (hight_relation < width_relation) {
        rect->rect.h = h * hight_relation;
        rect->rect.w = w * hight_relation;
    } else {
        rect->rect.h = h * width_relation;
        rect->rect.w = w * width_relation;
    }
}

int load_IMG_To_Texure(char *path, SDL_Texture **texture, SDL_Renderer *renderer, void (*effect_Function)(SDL_Surface *, int), int effect_config) { // TODO: use va_list
    SDL_Surface *image = IMG_Load(path);
    if (image == NULL) {
        printf("err- %s", SDL_GetError());
        return (-1);
    }
    if (effect_Function != NULL) {
        effect_Function(image, effect_config); // TODO: #6 blur on the time of the effect on different core
    }
    *texture = SDL_CreateTextureFromSurface(renderer, image);
    if (texture == NULL) {
        printf("err- %s", SDL_GetError());
        return (-1);
    }
    SDL_FreeSurface(image);
    return 0;
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
    SDL_Window *window = SDL_CreateWindow("slider", 0, 0, Width, Height, SDL_WINDOW_RESIZABLE); // TODO: #3 replace with SDL_WINDOW_FULLSCREEN
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

    int err = load_IMG_To_Texure("pics/closup-of-cat-on-floor-julie-austin-pet-photography.jpg", &background_texture, renderer, Image_Blur, 9);
    if (err != 0) {
        exit(1);
    }

    err = load_IMG_To_Texure("pics/closup-of-cat-on-floor-julie-austin-pet-photography.jpg", &forground_texture, renderer, NULL, 0);
    if (err != 0) {
        exit(1);
    }

    // put the location where we want the texture to be drawn into a rectangle
    Rect forground_rect = {.rect.x = 0, .rect.y = 0, .x = 0, .y = 0};
    Rect background_rect = {.rect.x = 0, .rect.y = 0, .x = 0, .y = 0};
    resize_rect(background_texture, &background_rect, window);
    forground_rect.rect.h = background_rect.rect.h;
    forground_rect.rect.w = background_rect.rect.w;

    SDL_SetRenderDrawColor(renderer, HEXCOLOR(BACKGROUND_COLOR));

    // TODO: #8 set better timing
    clock_t start = clock();
    double sec = 0;
    char *files_name[3] = {"pics/cat2.jpg", "pics/t/test.jpg", "pics/closup-of-cat-on-floor-julie-austin-pet-photography.jpg"};
    int pos = 0;

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
        // TODO: #9 get random name from file
        if (sec > 3) {
            load_IMG_To_Texure(files_name[pos], &forground_texture, renderer, NULL, 0);
            load_IMG_To_Texure(files_name[pos++], &background_texture, renderer, Image_Blur, 9);
            // TODO:#6 load the blur next image on diff thread and only replace

            if (pos == 3) {
                pos = 0;
            }
            sec = 0;
            start = clock();
        } else {
            sec += ((double)(clock() - start)) / CLOCKS_PER_SEC;
        }

        animate_rect(&forground_rect, 1, 1 * SPEED);
        animate_rect(&background_rect, 1, -0.3 * SPEED);

        // clear the screen
        SDL_RenderClear(renderer);
        // copy the texture to the rendering context
        SDL_RenderCopy(renderer, background_texture, NULL, &background_rect.rect);
        SDL_RenderCopy(renderer, forground_texture, NULL, &forground_rect.rect);
        // this means that everything that we prepared behind the screens is actually shown
        SDL_RenderPresent(renderer);
    }

    SDL_Quit();

    return 0;
}
