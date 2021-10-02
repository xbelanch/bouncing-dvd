/*
 * sdl_framerate.h: framerate manager
 *
 * Based on SDL_framerate.h from Andreas Schiffler Copyright (C) 2001-2012.
 * source: https://www.ferzkopp.net/Software/SDL_gfx-2.0/Docs/html/_s_d_l__framerate_8h_source.html
 *
 * Instead of allowing a CPU little nap we try something a bit different related to this
 * source: https://gameprogrammingpatterns.com/game-loop.html
 * from Robert Nystrom
 *
 * deWitters on GameLoop
 * source: https://dewitters.com/dewitters-gameloop/
 *
 * Another more "new" article about this topic is from :
 * source
 */

#ifndef SDL_FRAMERATE_H
#define SDL_FRAMERATE_H

#include <SDL2/SDL.h>
#include <stdbool.h>

typedef struct {
    double totalTime;
    double secondsPerCount;
    double elapsedTime;

    Uint64 baseTime;
    Uint64 pausedTime;
    Uint64 stopTime;
    Uint64 previousTime;
    Uint64 currentTime;

    bool isStopped;
    Uint32 FPS;
    Uint32 FPSCount;
    float FPSTimer;
    double lagTime;
    Uint32 totalFrameTicks;
    Uint32 totalFrames;
} Timer;

Timer *SDL_InitTimer(void);
int SDL_StartTimer(Timer *timer);
int SDL_StopTimer(Timer *timer);
int SDL_PauseTimer(Timer *timer);
int SDL_UpdateTimer(Timer *timer);
float SDL_GetElapsedTime(Timer *timer);
Uint32 SDL_GetFPS(Timer *timer);
float SDL_GetTotalTime(Timer *timer);

#endif
