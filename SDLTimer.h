#pragma once

#include <SDL_timer.h>

class SDLTimer
{
public:
    SDLTimer();

    double GetFrequency(void);
    void Restart(void);
    void Start(void) ;
    float Elapsed(void);
    float ElapsedMilliSeconds(void);

private:
    Uint64 _start;
    Uint64 _stop;
    double _frequency = 0.0;
};
