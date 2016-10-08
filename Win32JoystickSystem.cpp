#include "stdafx.h"
#include "Win32JoystickSystem.h"


void Win32JoystickSystem::joyInit()
{
    _input.init();
}

void Win32JoystickSystem::joyUpdate()
{
    if( isAvailable() )
        _input.update();
}

bool Win32JoystickSystem::isAvailable()
{
    return _input.lastAvailable();
}

float Win32JoystickSystem::joyGetX()
{
    return _input.xPosNorm();
}

float Win32JoystickSystem::joyGetY()
{
    return _input.yPosNorm();
}

float Win32JoystickSystem::joyGetZ()
{
    return _input.zPosNorm();
}

float Win32JoystickSystem::joyGetR()
{
    return _input.rPosNorm();
}

float Win32JoystickSystem::joyGetU()
{
    return _input.uPosNorm();
}

float Win32JoystickSystem::joyGetV()
{
    return _input.vPosNorm();
}

int Win32JoystickSystem::getPOV()
{
    return _input.getPOV();
}

int Win32JoystickSystem::numberofButtonsPressed()
{
    return _input.numberofButtonsPressed();
}

int Win32JoystickSystem::buttonFlagPressed()
{
    return _input.buttonFlagPressed();
}
