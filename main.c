#include <stdio.h>
#include <math.h>
#include "./twod.h"

#define SCREEN_FPS 144
// Stolen https://codereview.stackexchange.com/a/250618
#define  ms_frame 1000 / SCREEN_FPS

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

    // Create a window with the specified position, dimensions, and flags
    SDL_Window *window = SDL_CreateWindow("Just a window",
                                          SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                          640, 480,
                                          SDL_WINDOW_SHOWN);

    if (window == NULL) {
        // In the case that the window could not be made...
        printf("Could not create window: %s\n", SDL_GetError());
        return 1;
    }

    // Create a 2D rendering context for a window
    SDL_Renderer *renderer =  SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // Set the color used for drawing operations (Rect, Line and Clear).
    // Paint it purple for debugging
    int success = SDL_SetRenderDrawColor(renderer,
                                           255, 0, 255,
                                           255);

    // Create a texture for a rendering context.
    SDL_Texture *texture =  SDL_CreateTexture(renderer,
                                              SDL_PIXELFORMAT_RGBA32,
                                              SDL_TEXTUREACCESS_STATIC,
                                              640, 480);
    // A rectangle, with the origin at the upper left (integer)
    SDL_Rect rectangle = {
        0, 0, // x, y
        32, 32 // w, h
    };

    // A simple dummy event
    SDL_Event event;

    // Fix framerate vars
    Uint32 initial_ticks, elapsed_ms;

    // Game loop
    while (1) {

        initial_ticks = SDL_GetTicks();

        // Pumps the event loop, gathering events from the input devices.
        SDL_PumpEvents();

        // Handle keyboard and other events
        while(SDL_PollEvent(&event)) {
            // Handle your events here
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
                exit(0);
            }
        }
        // Do some other stuff here like render

        // Clear the current rendering target with the drawing color
        int success = SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255);
        SDL_RenderClear(renderer);

        // Fill a rectangle on the current rendering target with the drawing color
        success = SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        success = SDL_RenderFillRect(renderer, &rectangle);

        { /* End Rendering */

            // Update the screen with rendering performed.
            SDL_RenderPresent(renderer);

            // Fixed framerate
            elapsed_ms = SDL_GetTicks() - initial_ticks;
            // fprintf(stdout, "%u %u\n", ms_frame, elapsed_ms);
            if(elapsed_ms < ms_frame) SDL_Delay(ms_frame - elapsed_ms);
        }
    }

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


// gcc main.c -lm `pkg-config --cflags --libs sdl2`
