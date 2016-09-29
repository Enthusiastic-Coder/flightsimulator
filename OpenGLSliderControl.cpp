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

void OpenGLSliderControl::update(float dt)
{
    // update current value to match value based on dt

}

void OpenGLSliderControl::render(Renderer *r)
{

}

void OpenGLSliderControl::updateLayout()
{
    // use all state to decide how to position control
}
