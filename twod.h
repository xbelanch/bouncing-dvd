/*
  TwoD Library
  SDL2 Template Project?
  More comments are coming
 */

// Standard includes
#include <stdlib.h>
#include <stdio.h>
#include <SDL2/SDL.h>

typedef struct screen {
    size_t width, height;
    Uint32 *pixels;
    SDL_Texture *texture;
} Screen;

int twod_init() {
    int log = SDL_Init(SDL_INIT_EVERYTHING);
    return log;
}

void twod_close() {
    SDL_Quit();
}