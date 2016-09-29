#include "OpenGLSliderControl.h"
#include <OpenGLRenderer.h>

void OpenGLSliderControl::setPosition(MathSupport<int>::point pos)
{
    _position = pos;
    updateLayout();
}

void OpenGLSliderControl::setLength(int length)
{
    _length = length;
    updateLayout();
}

void OpenGLSliderControl::setWidth(int width)
{
    _width = width;
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
