#include "stdafx.h"
#include "SDLJoystickSystem.h"


void SDLJoystickSystem::joyInit()
{
    //_input.init();
}

void SDLJoystickSystem::joyUpdate()
{
//    if( isAvailable() )
//        _input.update();
}

bool SDLJoystickSystem::isAvailable()
{
    return 0;//_input.lastAvailable();
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
