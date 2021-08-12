#include <stdio.h>
#include "./twod.h"

int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv[0];

    int log = vcore_init();
    if (log == 0) {
        fprintf(stdout, "SDL2 was successfully initialized\n");
    } else {
        fprintf(stdout, "Cannot initialize SDL2: %s\n", SDL_GetError());
    }

    // Some graphics info
    const char *vdriver = SDL_GetCurrentVideoDriver();
    fprintf(stdout, "Current Video Driver: %s\n", vdriver);

    int numVideoDisplays = SDL_GetNumVideoDisplays();
    fprintf(stdout, "Number of Video Displays: %d\n", numVideoDisplays);

    SDL_Rect rect = {0};
    int displayBounds = SDL_GetDisplayBounds(0, &rect);
    if (displayBounds == 0) {
        fprintf(stdout, "Display bounds: %d - %d\n", rect.w, rect.h);
    }

    SDL_DisplayOrientation displayOrientation = SDL_GetDisplayOrientation(0);
    switch(displayOrientation) {
    case 0:
        fprintf(stdout, "Display orientation: %d\n", displayOrientation);
        break;
    default:
        break;
    }


    SDL_Window *window = SDL_CreateWindow("Just a window",
                                          SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                          640, 480,
                                          SDL_WINDOW_SHOWN);

    if (window == NULL) {
        // In the case that the window could not be made...
        printf("Could not create window: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Renderer *renderer =  SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    SDL_Texture *texture =  SDL_CreateTexture(renderer,
                                              SDL_PIXELFORMAT_RGBA32,
                                              SDL_TEXTUREACCESS_STATIC,
                                              640, 480);
    SDL_RenderPresent(renderer);

    // Pause execution for 3000 milliseconds, for example
    SDL_Delay(3000);



    // Close and destroy texture
    SDL_DestroyTexture(texture);
    // Close and destroy texture
    SDL_DestroyRenderer(renderer);
    // Close and destroy the window
    SDL_DestroyWindow(window);

    // Clean Up
    vcore_shutdown();

    return 0;
}


// gcc main.c `pkg-config --cflags --libs sdl2`
