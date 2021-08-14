#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <SDL2/SDL.h>

#define WINDOWS_WIDTH 640
#define WINDOWS_HEIGHT 480
#define UNPACK_COLOR(color) (color >> 24), (color >> 16 & 0xff), (color >> 8 & 0xff), (color & 0xff)
#define COLOR_PALETTE (Uint32[]){\
        0x0f380fff,              \
        0x306230ff,              \
        0x8bac0fff,              \
        0x9bbc0fff,              \
        0xaaaaaaff               \
}

typedef struct {
    float x, y;
    int dirX, dirY;
    float width, height;
    float xSpeed, ySpeed;
    Uint32 born, lastUpdate;
} BouncingLogo;

BouncingLogo DVD;

SDL_Renderer *renderer;
SDL_Texture *texture;
Uint32 initial_ticks, elapsed_ms;
size_t frames = 0;


void draw_rect(SDL_Renderer *renderer, float x, float y, float w, float h, Uint32 color) {
    // A rectangle, with the origin at the upper left (integer)
    SDL_Rect rect = {
        (int) ceilf(x), (int) ceilf(y), // x, y
        (int) ceilf(w), (int) ceilf(h) // w, h
    };

    // TODO: Unpack color macro?
    SDL_SetRenderDrawColor(renderer, UNPACK_COLOR(color));
    SDL_RenderFillRect(renderer, &rect);
}

int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv[0];

    srand(time(NULL));

    {  /* DVD Logo Initialization */
        DVD = (BouncingLogo){
            .x = rand() % WINDOWS_WIDTH,
            .y = rand() % WINDOWS_HEIGHT,
            .dirX = (rand() % 2)  == 1 ? 1 : -1,
            .dirY = (rand() % 2)  == 1 ? 1 : -1,
            .width = 64,
            .height = 64,
            .xSpeed = 256.0f,
            .ySpeed = 256.0f,
            .born = SDL_GetTicks(),
            .lastUpdate = SDL_GetTicks()
        };

        if (DVD.x < 0)
            DVD.x = 0;
        if (DVD.x + DVD.width > WINDOWS_WIDTH)
            DVD.x = WINDOWS_WIDTH - DVD.width;
        if (DVD.y < 0)
            DVD.y = 0;
        if (DVD.y + DVD.height > WINDOWS_HEIGHT)
            DVD.y = WINDOWS_HEIGHT - DVD.height;
    }


    { /* Init all SDL Subsystems */
        if (SDL_Init(SDL_INIT_VIDEO != 0)) {
            fprintf(stdout, "SDL_Init() failed to init: %s\n", SDL_GetError());
            return 1;
        }
        atexit(SDL_Quit);
    }

    // Create a window with the specified position, dimensions, and flags
    SDL_Window *window = SDL_CreateWindow("SDL2 Project Template",
                                          SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                          WINDOWS_WIDTH, WINDOWS_HEIGHT,
                                          SDL_WINDOW_SHOWN);

    { /* Renderer and texture */
        if (window == NULL) {
            // In the case that the window could not be made...
            printf("SDL_CreateWindow() failed to init: %s\n", SDL_GetError());
            return 1;
        }

        // Create a 2D rendering context for a window
        renderer =  SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

        // Set the color used for drawing operations (Rect, Line and Clear).
        SDL_SetRenderDrawColor(renderer, UNPACK_COLOR(COLOR_PALETTE[0]));

        // Create a texture for a rendering context.
        texture =  SDL_CreateTexture(renderer,
                                              SDL_PIXELFORMAT_RGBA32,
                                              SDL_TEXTUREACCESS_STATIC,
                                                  WINDOWS_WIDTH, WINDOWS_HEIGHT);
    }


    { /* Get and log some graphics info */
        const char *vdriver = SDL_GetCurrentVideoDriver();
        fprintf(stdout, "Current Video Driver: %s\n", vdriver);

        int numVideoDisplays = SDL_GetNumVideoDisplays();
        fprintf(stdout, "Number of Video Displays: %d\n", numVideoDisplays);

        SDL_Rect rect;
        int displayBounds = SDL_GetDisplayBounds(0, &rect);
        if (displayBounds == 0)
            fprintf(stdout, "Display bounds: %d - %d\n", rect.w, rect.h);
    }


    /* Game loop */
    while (1) {
        Uint64 start = SDL_GetPerformanceCounter();

        { /* processInput() */
            // Pumps the event loop, gathering events from the input devices.
            SDL_PumpEvents();

            // Handle keyboard and other events
            SDL_Event event;
            while(SDL_PollEvent(&event)) {
                // Handle your events here
                if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
                    exit(0);
                }
            }
        }

        { /* update() */

            Uint32 time = SDL_GetTicks();
            float dt = (time - DVD.lastUpdate) / 1000.0f;

            DVD.x += (DVD.xSpeed * dt) * DVD.dirX;
            DVD.y += (DVD.ySpeed * dt) * DVD.dirY;

            if (DVD.x >= WINDOWS_WIDTH - DVD.width || DVD.x < 0)
                DVD.dirX = -(DVD.dirX);

            if (DVD.y >= WINDOWS_HEIGHT - DVD.height || DVD.y < 0)
                DVD.dirY = -(DVD.dirY);

            DVD.lastUpdate = SDL_GetTicks();
        }


        { /* render() */

            // Clear the current rendering target with the drawing color
            SDL_SetRenderDrawColor(renderer, UNPACK_COLOR(COLOR_PALETTE[0]));
            SDL_RenderClear(renderer);

            // Draw a rectangle on the current rendering target with the drawing color
            draw_rect(renderer,
                      DVD.x,
                      DVD.y,
                      DVD.width, DVD.height,
                      COLOR_PALETTE[2]);

            // Update the screen with rendering performed.
            SDL_RenderPresent(renderer);
        }

        { /* Cap to an "exact" framerate */

            // Cap to 60 FPS (FPS Delay)
            Uint64 end = SDL_GetPerformanceCounter();
            float elapsedMS = (end - start) / (float)SDL_GetPerformanceFrequency() * 1000.0f;
            SDL_Delay(floor(16.6666f - elapsedMS));
            frames++;
        }
    }

    { /* Clean Up */
        // Close and destroy texture
        SDL_DestroyTexture(texture);
        // Close and destroy texture
        SDL_DestroyRenderer(renderer);
        // Close and destroy the window
        SDL_DestroyWindow(window);
    }

    return 0;
}
