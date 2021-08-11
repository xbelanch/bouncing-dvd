#include <stdio.h>
#include "./twod.h"

int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv[0];

    int log = twod_init();
    if (log == 0) {
        fprintf(stdout, "SDL2 has been successed initialized");
    } else {
        fprintf(stdout, "Cannot initialize SDL2\n");
    }

    // Some graphics info
    const char *vdriver = SDL_GetCurrentVideoDriver();
    fprintf(stdout, "Current Video Driver: %s\n", vdriver);

    int numVideoDisplays = SDL_GetNumVideoDisplays();
    fprintf(stdout, "Number of Video Displays: %d\n", numVideoDisplays);

    SDL_Rect *rect = malloc(sizeof(SDL_Rect));
    int displayBounds = SDL_GetDisplayBounds(0, rect);
    fprintf(stdout, "Display bounds: %d\n", displayBounds);

    SDL_DisplayOrientation displayOrientation = SDL_GetDisplayOrientation(0);
    fprintf(stdout, "Display orientation: %d\n", displayOrientation);


    SDL_Window *window = SDL_CreateWindow("Just a window",
                                          0, 0,
                                          640, 480,
                                          SDL_WINDOW_SHOWN);


    twod_close();

    return 0;
}


// gcc main.c `pkg-config --cflags --libs sdl2`
