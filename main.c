#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <SDL2/SDL.h>

#define STB_IMAGE_IMPLEMENTATION
#include "./stb_image.h"

#define WINDOWS_WIDTH 640
#define WINDOWS_HEIGHT 480
#define UNPACK_RGBA_COLOR(color) (color >> 24), (color >> 16 & 0xff), (color >> 8 & 0xff), (color & 0xff)
#define UNPACK_RGB_COLOR(color)  (color >> 24), (color >> 16 & 0xff), (color >> 8 & 0xff)

#define COLOR_PALETTE (Uint32[]){               \
        0x0f380fff,              \
        0x306230ff,              \
        0x8bac0fff,              \
        0x9bbc0fff,              \
        0xaaaaaaff               \
}

#define BACKGROUND COLOR_PALETTE[0]

const int SIMULATION_FPS = 60;
const double MS_PER_UPDATE  = 1.0f / SIMULATION_FPS;
typedef struct {
    float x, y;
    float width, height;
    float xSpeed, ySpeed;
    Uint32 born, lastUpdate;
    Uint32 color;
    SDL_Texture *texture;
} BouncingLogo;

BouncingLogo DVD;
int W0, H0;

SDL_Renderer *renderer;
SDL_Texture *texture;
SDL_Surface* surface;
Uint32 initial_ticks, elapsed_ms;
size_t frames = 0;

void draw_rect(SDL_Renderer *renderer, float x, float y, float w, float h, Uint32 color) {

    // A rectangle, with the origin at the upper left (integer)
    SDL_Rect rect = {
        (int) ceilf(x), (int) ceilf(y), // x, y
        (int) ceilf(w), (int) ceilf(h) // w, h
    };

    // Paint a rectangle
    // SDL_SetRenderDrawColor(renderer, UNPACK_RGBA_COLOR(color));
    // SDL_RenderFillRect(renderer, &rect);

    // Paint the DVD Logo
    SDL_SetTextureColorMod(DVD.texture, UNPACK_RGB_COLOR(color));
    if (SDL_RenderCopyEx(renderer, DVD.texture, NULL, &rect, 0, NULL, SDL_FLIP_NONE) != 0) {
        fprintf(stdout, "Cannot blit to screen: %s\n", SDL_GetError());
        exit(1);
    }

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
            .width = 90,
            .height = 60,
            .xSpeed = 0.1f,
            .ySpeed = 0.1f,
            .born = SDL_GetTicks(),
            .lastUpdate = SDL_GetTicks(),
            .color = 0xffffffff
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

    // The effective displacement
    W0 = WINDOWS_WIDTH - DVD.width;
    H0 = WINDOWS_HEIGHT - DVD.height;

    // Load logo DVD image
    int req_format = STBI_rgb_alpha;
    int width, height, orig_format;
    unsigned char* data = stbi_load("./assets/sprites/DVD_logo.png", &width, &height, &orig_format, req_format);

    if (data == NULL) {
        SDL_Log("Loading image failed: %s", stbi_failure_reason());
        exit(1);
    }

    // Set up the pixel format color masks for RGB(A) byte arrays.
    // Only STBI_rgb (3) and STBI_rgb_alpha (4) are supported here!
    Uint32 rmask, gmask, bmask, amask;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    int shift = (req_format == STBI_rgb) ? 8 : 0;
    rmask = 0xff000000 >> shift;
    gmask = 0x00ff0000 >> shift;
    bmask = 0x0000ff00 >> shift;
    amask = 0x000000ff >> shift;
#else // little endian, like x86
    rmask = 0x000000ff;
    gmask = 0x0000ff00;
    bmask = 0x00ff0000;
    amask = (req_format == STBI_rgb) ? 0 : 0xff000000;
#endif

    int depth, pitch;
    if (req_format == STBI_rgb) {
        depth = 24;
        pitch = 3*width; // 3 bytes per pixel * pixels per row
    } else { // STBI_rgb_alpha (RGBA)
        depth = 32;
        pitch = 4*width;
    }

    // ...and copy data to a SDL Surface
    fprintf(stdout, "width %d height %d orig_format %d depth %d pitch %d\n", width, height, orig_format, depth, pitch);

    surface = SDL_CreateRGBSurfaceFrom((void*)data, width, height, depth, pitch, rmask, gmask, bmask, amask);

    if (surface == NULL) {
        SDL_Log("Creating surface failed: %s", SDL_GetError());
        stbi_image_free(data);
        exit(1);
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
        SDL_SetRenderDrawColor(renderer, UNPACK_RGBA_COLOR(BACKGROUND));

        // Create a texture for a rendering context.
        texture =  SDL_CreateTexture(renderer,
                                              SDL_PIXELFORMAT_RGBA32,
                                              SDL_TEXTUREACCESS_STATIC,
                                                  WINDOWS_WIDTH, WINDOWS_HEIGHT);

        // Create a texture from a surface
        DVD.texture = SDL_CreateTextureFromSurface(renderer, surface);
        if (DVD.texture == NULL) {
            fprintf(stdout, "Cannot create texture from surface: %s\n", SDL_GetError());
            exit(1);
        }
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
    Uint64 previous = SDL_GetPerformanceCounter();
    double deltaTime = 0;
    double lag = 0;

    while (1) {
        Uint64 current = SDL_GetPerformanceCounter();
        deltaTime = ((current - previous) * 1000.0 / (double) SDL_GetPerformanceFrequency());
        previous = current;
        lag += deltaTime;

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

        { /* update(elapsed) */
            DVD.x += (DVD.xSpeed * deltaTime);
            DVD.y += (DVD.ySpeed * deltaTime);

            // Collision
            if ((int)DVD.x == W0) {
                DVD.xSpeed = -(DVD.xSpeed);
                DVD.color = 0xff00ffff;
            }
            if ((int)DVD.y == H0) {
                DVD.ySpeed = -(DVD.ySpeed);
                DVD.color = 0xffff00ff;
            }
            if ((int)DVD.x == 0) {
                DVD.xSpeed = -(DVD.xSpeed);
                DVD.color = 0x00ffffff;
            }
            if ((int)DVD.y == 0) {
                DVD.ySpeed = -(DVD.ySpeed);
                DVD.color = 0x00ff00ff;
            }
            while (lag >= MS_PER_UPDATE) {
                lag -= MS_PER_UPDATE;
            }

        }

        { /* render() */

            // Clear the current rendering target with the drawing color
            SDL_SetRenderDrawColor(renderer, UNPACK_RGBA_COLOR(BACKGROUND));
            SDL_RenderClear(renderer);

            // Draw a rectangle on the current rendering target with the drawing color
            draw_rect(renderer,
                      DVD.x,
                      DVD.y,
                      DVD.width, DVD.height,
                      DVD.color);

            // Update the screen with rendering performed.
            SDL_RenderPresent(renderer);
        }

        // { /* Cap to an "exact" framerate */
        //     // Cap to 60 FPS (FPS Delay)
        //     lastTime = current;
        //     frames++;
        // }
    } // End of Game Loop

    { /* Clean Up */
        // Free image data
        stbi_image_free(data);
        // Free surface memory
        SDL_FreeSurface(surface);
        // Close and destroy texture
        SDL_DestroyTexture(texture);
        // Close and destroy texture
        SDL_DestroyRenderer(renderer);
        // Close and destroy the window
        SDL_DestroyWindow(window);
    }

    return 0;
}
