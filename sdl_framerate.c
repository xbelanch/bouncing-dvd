#include "sdl_framerate.h"

Timer *SDL_InitTimer() {
    Timer *timer = (Timer*)malloc(sizeof(Timer));
    if (timer != NULL) {
        // Get the count per second of the high resolution counter.
        Uint64 countsPerSecond = SDL_GetPerformanceFrequency();
        timer->secondsPerCount = 1.0f / (double)countsPerSecond;
        timer->isStopped = false;
        timer->previousTime = 0.0;
        timer->currentTime = 0.0;
        timer->elapsedTime = 0.0;
        timer->lagTime = 0.0;
        timer->totalFrames = 0;
        timer->totalFrameTicks = 0;
        return (timer);
    } else {
        return (NULL);
    }
}

int SDL_StartTimer(Timer *timer) {
    if (timer == NULL)
        return (1);

    Uint64 startTime = SDL_GetPerformanceCounter();

    if (timer->isStopped) {
        timer->pausedTime += (startTime - timer->stopTime);
        timer->previousTime = startTime;
        timer->stopTime = 0;
        timer->isStopped = false;
    } else {
        timer->baseTime = startTime;
        timer->previousTime = startTime;
        timer->stopTime = 0;
        timer->FPSTimer = 0.0f;
        timer->FPSCount = 0;
        timer->isStopped = false;
    }

    return (0);
}

int SDL_StopTimer(Timer *timer) {
    if (timer == NULL)
        return (1);

    if (timer->isStopped) {
        Uint64 currentTime = SDL_GetPerformanceCounter();
        timer->stopTime = currentTime;
        timer->isStopped = true;
    }
    return (0);
}

int SDL_UpdateTimer(Timer *timer) {
    if (timer == NULL) {
        return (1);
    }

	if (timer->isStopped)	{
		timer->FPS = 0;
		timer->elapsedTime = 0.0f;
        // TODO: Explain this!
        timer->totalTime = (float)(((timer->stopTime - timer->pausedTime) - timer->baseTime) * timer->baseTime);
		return (0);
	}

    double currentTime = SDL_GetPerformanceCounter();
    timer->currentTime = currentTime;

    timer->elapsedTime = (float)((timer->currentTime - timer->previousTime) * timer->secondsPerCount);
    timer->previousTime = timer->currentTime;

    if (timer->elapsedTime < 0.0f)
        timer->elapsedTime = 0.0f;

    // TODO: Explain this!
    timer->totalTime = (float)(((timer->currentTime - timer->pausedTime) * timer->secondsPerCount));


	//FPS LOGIC
    timer->FPSTimer += timer->elapsedTime;
    timer->FPSCount++;
    if (timer->FPSTimer >= 1.0f) {
        timer->FPS = timer->FPSCount;
        timer->FPSCount = 0;
        timer->FPSTimer -= 1.0f;
    }

    return (0);
}


float SDL_GetElapsedTime(Timer *timer) {
    if (timer == NULL) {
        return (1);
    }
    return timer->elapsedTime;
}

Uint32 SDL_GetFPS(Timer *timer) {
    if (timer == NULL) {
        return (1);
    }
    return timer->FPS;
}

float SDL_GetTotalTime(Timer *timer) {
    if (timer == NULL) {
        return (1);
    }
    return timer->totalTime;
}
