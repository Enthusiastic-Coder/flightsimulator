#include "stdafx.h"
#include "JoystickSystem.h"


void JoystickSystem::joyInit()
{
    _input.init();
}

void JoystickSystem::joyUpdate()
{
    if( isAvailable() )
        _input.update();
}

bool JoystickSystem::isAvailable()
{
    return _input.lastAvailable();
}

float JoystickSystem::joyGetX()
{
    return _input.xPosNorm();
}

float JoystickSystem::joyGetY()
{
    return _input.yPosNorm();
}

float JoystickSystem::joyGetZ()
{
    return _input.zPosNorm();
}

float JoystickSystem::joyGetR()
{
    return _input.rPosNorm();
}

float JoystickSystem::joyGetU()
{
    return _input.uPosNorm();
}

float JoystickSystem::joyGetV()
{
    return _input.vPosNorm();
}

int JoystickSystem::getPOV()
{
    return _input.getPOV();
}

int JoystickSystem::numberofButtonsPressed()
{
    return _input.numberofButtonsPressed();
}

int JoystickSystem::buttonFlagPressed()
{
    return _input.buttonFlagPressed();
}
