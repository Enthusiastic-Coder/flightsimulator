#pragma once

#include "interfaces.h"
#include "Win32JoystickController.h"

class Win32JoystickSystem : public IJoystick
{
public:
    void joyInit() override;
    void joyUpdate() override;
    bool isAvailable() override;
    float joyGetX() override;
    float joyGetY() override;
    float joyGetZ() override;
    float joyGetR() override;
    float joyGetU() override;
    float joyGetV() override;
    int getPOV() override;
    int numberofButtonsPressed() override;
    int buttonFlagPressed() override;

private:
    Win32JoystickController _input;
};


