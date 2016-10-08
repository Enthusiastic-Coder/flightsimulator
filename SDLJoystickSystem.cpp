#include "stdafx.h"
#include "SDLJoystickSystem.h"
#include <SDL_joystick.h>
#include <SDL_log.h>

void SDLJoystickSystem::joyInit(char* id)
{
    SDL_Log("There are %d joysticks available\n", SDL_NumJoysticks());

    for (int i = 0; i < SDL_NumJoysticks(); ++i)
        SDL_Log( "[%d -> %s", i, SDL_JoystickNameForIndex(i));
}

void SDLJoystickSystem::joyUpdate()
{
//    if( isAvailable() )
//        _input.update();
}

bool SDLJoystickSystem::isAvailable()
{
    return false;//_input.lastAvailable();
}

float SDLJoystickSystem::joyGetX()
{
    return 0;//_input.xPosNorm();
}

float SDLJoystickSystem::joyGetY()
{
    return 0;//_input.yPosNorm();
}

float SDLJoystickSystem::joyGetZ()
{
    return 0;//_input.zPosNorm();
}

float SDLJoystickSystem::joyGetR()
{
    return 0;//_input.rPosNorm();
}

float SDLJoystickSystem::joyGetU()
{
    return 0;//_input.uPosNorm();
}

float SDLJoystickSystem::joyGetV()
{
    return 0;//_input.vPosNorm();
}

int SDLJoystickSystem::getPOV()
{
    return 0;//_input.getPOV();
}

int SDLJoystickSystem::numberofButtonsPressed()
{
    return 0;//_input.numberofButtonsPressed();
}

int SDLJoystickSystem::buttonFlagPressed()
{
    return 0;// _input.buttonFlagPressed();
}
