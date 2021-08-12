#include <stdio.h>
#include "./twod.h"

#define SCREEN_FPS 60

void draw_rect(SDL_Renderer *renderer, float x, float y, float w, float h, Uint32 color) {

    // A rectangle, with the origin at the upper left (integer)
    SDL_Rect rect = {
        (int) ceilf(x), (int) ceilf(y), // x, y
        (int) ceilf(w), (int) ceilf(h) // w, h
    };

    // TODO: Unpack color macro?
    int success = SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    success = SDL_RenderFillRect(renderer, &rect);
}

float x = 0.0f;
float y = 0.0f;
float velocityX = 320.0f;
float velocityY = 320.0f;

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

    // A simple dummy event
    SDL_Event event;

    // Framerate stuff
    Uint32 initial_ticks, elapsed_ms;
    Uint32 lastUpdate = SDL_GetTicks();

    // Game loop
    size_t frames = 0;
    while (1) {

        // initial_ticks = SDL_GetTicks();
        Uint64 start = SDL_GetPerformanceCounter();

        // Pumps the event loop, gathering events from the input devices.
        SDL_PumpEvents();

        // Handle keyboard and other events
        while(SDL_PollEvent(&event)) {
            // Handle your events here
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
                exit(0);
            }
        }

        {  /* Render clear */
            // Clear the current rendering target with the drawing color
            int success = SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255);
            SDL_RenderClear(renderer);
        }

        { /* Physics loop */

            Uint32 time = SDL_GetTicks();
            float dt = (time - lastUpdate) / 1000.0f;

            x += (velocityX * dt);
            y += (velocityY * dt);

            if (x + 64 >= 640 || x < 0)
                velocityX *= -1;

            if (y + 64 >= 480 || y < 0)
                velocityY *= -1;

            lastUpdate = SDL_GetTicks();
        }


        { /* Render stuff */
            // Draw a rectangle on the current rendering target with the drawing color
            draw_rect(renderer,
                      x,
                      y,
                      64, 64, 0);
        }

        { /* End Rendering */

            // Update the screen with rendering performed.
            SDL_RenderPresent(renderer);

            // Cap to 60 FPS (FPS Delay)
            Uint64 end = SDL_GetPerformanceCounter();
            float elapsedMS = (end - start) / (float)SDL_GetPerformanceFrequency() * 1000.0f;
            SDL_Delay(floor(16.6666f - elapsedMS));

            frames++;
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
