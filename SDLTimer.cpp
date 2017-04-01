#include "stdafx.h"
#include "SDLTimer.h"


SDLTimer::SDLTimer(void) : _start(0), _stop(0)
{
    _frequency = GetFrequency();
}

double SDLTimer::GetFrequency(void)
{
    return (double)SDL_GetPerformanceFrequency();
}

void SDLTimer::Restart(void)
{
    _start = _stop;
}

void SDLTimer::Start(void)
{
    _start = SDL_GetPerformanceCounter();
}

float SDLTimer::Elapsed(void)
{
    _stop = SDL_GetPerformanceCounter();
    return (_stop - _start) / _frequency;
}

float SDLTimer::ElapsedMilliSeconds(void)
{
    return Elapsed()*1000;
}
