#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#include <SDL2/SDL.h>



#define HEXCOLOR(code) \
  ((code) >> (3 * 8)) & 0xFF, \
  ((code) >> (2 * 8)) & 0xFF, \
  ((code) >> (1 * 8)) & 0xFF, \
  ((code) >> (0 * 8)) & 0xFF

#define BACKGROUND_COLOR 0x181818FF


int main(void)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "ERROR: could not initialize SDL: %s\n", SDL_GetError());
        exit(1);
    }

    SDL_Window *window = SDL_CreateWindow("slider", 0, 0, 800, 600, SDL_WINDOW_RESIZABLE);
    if (window == NULL) {
        fprintf(stderr, "ERROR: could not create a window: %s\n", SDL_GetError());
        exit(1);
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        fprintf(stderr, "ERROR: could not create a renderer: %s\n", SDL_GetError());
        exit(1);
    }

    bool quit = false;
    while (!quit) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch(event.type) {
            case SDL_QUIT: {
                quit = true;
            }
            break;
            }
        }

        SDL_SetRenderDrawColor(renderer, HEXCOLOR(BACKGROUND_COLOR));
        SDL_RenderClear(renderer);

#define STEP_TIME_SLIDER_X 100.0f
#define STEP_TIME_SLIDER_Y 100.0f
#define STEP_TIME_SLIDER_LEN 500.0f
#define STEP_TIME_SLIDER_THICC 5.0f
#define STEP_TIME_SLIDER_COLOR 0x00FF00FF
#define STEP_TIME_GRIP_SIZE 10.0f
#define STEP_TIME_GRIP_COLOR 0xFF0000FF

        // Slider Body
        {
            SDL_SetRenderDrawColor(renderer, HEXCOLOR(STEP_TIME_SLIDER_COLOR));
            SDL_Rect rect = {
                .x = STEP_TIME_SLIDER_X,
                .y = STEP_TIME_SLIDER_Y - STEP_TIME_SLIDER_THICC * 0.5f,
                .w = STEP_TIME_SLIDER_LEN,
                .h = STEP_TIME_SLIDER_THICC,
            };
            SDL_RenderFillRect(renderer, &rect);
        }

        

        SDL_RenderPresent(renderer);
    }

    SDL_Quit();

    return 0;
}
