#pragma once

#include "interfaces.h"

#include <SDL_joystick.h>

class SDLJoystickSystem : public IJoystick
{
public:
    ~SDLJoystickSystem();
    void joyInit(char *id) override;
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
    SDL_Joystick* _accelerometer = 0;
    int _numberOfButtonsPressed = 0;
    int _buttonFlag = 0;
};


