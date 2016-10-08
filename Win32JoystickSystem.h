#pragma once

#include "interfaces.h"
#include "Win32JoystickController.h"

class Win32JoystickSystem : public IJoystick
{
public:

    void joyInit();
    void joyUpdate();
    bool isAvailable();
    float joyGetX();
    float joyGetY();
    float joyGetZ();
    float joyGetR();
    float joyGetU();
    float joyGetV();
    int getPOV();
    int numberofButtonsPressed();
    int buttonFlagPressed();

private:
    Win32JoystickController _input;
};


