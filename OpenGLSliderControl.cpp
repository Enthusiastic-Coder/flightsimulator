#include "OpenGLSliderControl.h"
#include <OpenGLRenderer.h>

void OpenGLSliderControl::setScreenSize(MathSupport<int>::size sz)
{
    _screensize = sz;
}

void OpenGLSliderControl::setPosition(float U, float V)
{
    _U = U;
    _V = V;
    updateLayout();
}

void OpenGLSliderControl::setSize(float dU, float dV)
{
    _dU = dU;
    _dV = dV;
    updateLayout();
}

void OpenGLSliderControl::setOrientation(OpenGLSliderControl::Orientation o)
{
    _orientation = o;
    updateLayout();
}

void OpenGLSliderControl::setHAlignment(OpenGLSliderControl::Alignment alignment)
{
    _hAlignment = alignment;
    updateLayout();
}

void OpenGLSliderControl::setVAlignment(OpenGLSliderControl::Alignment alignment)
{
    _vAlignment = alignment;
    updateLayout();
}

void OpenGLSliderControl::setColor(Vector4F color)
{
    _color = color;
}

void OpenGLSliderControl::setMinValue(float value)
{
    _min = value;
}

void OpenGLSliderControl::setMaxValue(float value)
{
    _max = value;
}

void OpenGLSliderControl::setValue(float value)
{
    _value = value;
}

void OpenGLSliderControl::update(float dt)
{
    // update current value to match value based on dt

}

void OpenGLSliderControl::render(Renderer *r)
{
    // draw outline rectangle of slider
    // draw solid transparent _color rectangle showing current value
    // draw outline rectangle showing final value

}

void OpenGLSliderControl::updateLayout()
{
    // use all state to decide how to position control
}
