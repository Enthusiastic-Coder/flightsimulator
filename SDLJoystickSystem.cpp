#include "stdafx.h"
#include "SDLJoystickSystem.h"
#include <SDL_log.h>

#define SINT16_MAX ((float)(0x7FFF))

SDLJoystickSystem::~SDLJoystickSystem()
{
    SDL_JoystickClose(_accelerometer);
}

void SDLJoystickSystem::joyInit(char* id)
{
    SDL_Log("There are %d joysticks available\n", SDL_NumJoysticks());

    for (int i = 0; i < SDL_NumJoysticks(); ++i)
        SDL_Log( "[%d -> %s", i, SDL_JoystickNameForIndex(i));

    _accelerometer = SDL_JoystickOpen(0);
}

void SDLJoystickSystem::joyUpdate()
{
    SDL_JoystickUpdate();
    _numberOfButtonsPressed = 0;
    _buttonFlag = 0;
    int count = SDL_JoystickNumButtons(_accelerometer);
    for( int i=0; i < count; ++i)
    {
        int pressed = SDL_JoystickGetButton(_accelerometer, i);
        _numberOfButtonsPressed += pressed;
        if(pressed)  _buttonFlag |= i;
    }
}

bool SDLJoystickSystem::isAvailable()
{
    return _accelerometer != 0;
}

float SDLJoystickSystem::joyGetX()
{
    return float(SDL_JoystickGetAxis(_accelerometer, 0))/SINT16_MAX;
}

float SDLJoystickSystem::joyGetY()
{
    return float(SDL_JoystickGetAxis(_accelerometer, 1))/SINT16_MAX;
}

float SDLJoystickSystem::joyGetZ()
{
    return float(SDL_JoystickGetAxis(_accelerometer, 2))/SINT16_MAX;
}

float SDLJoystickSystem::joyGetR()
{
    return float(SDL_JoystickGetAxis(_accelerometer, 3))/SINT16_MAX;
}

float SDLJoystickSystem::joyGetU()
{
    return float(SDL_JoystickGetAxis(_accelerometer, 4))/SINT16_MAX;
}

float SDLJoystickSystem::joyGetV()
{
    return float(SDL_JoystickGetAxis(_accelerometer, 5))/SINT16_MAX;
}

int SDLJoystickSystem::getPOV()
{
    return 0;//_input.getPOV();
}

int SDLJoystickSystem::numberofButtonsPressed()
{
    return _numberOfButtonsPressed;
}

int SDLJoystickSystem::buttonFlagPressed()
{
    return _buttonFlag;
}
