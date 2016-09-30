#include "OpenGLSliderControl.h"
#include <OpenGLRenderer.h>

void OpenGLSliderControl::onSize(MathSupport<int>::size sz)
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

void OpenGLSliderControl::handleMouseDown(MathSupport<int>::point pt)
{
    if( !isInside(pt))
        return;

    if( _orientation == Orient_Horizontal)
        _value = float(pt.x - _position.x) / _size.width * (_max - _min) + _min;
    else
        _value = float(pt.y - _position.y) / _size.height * (_max - _min) + _min;
}

bool OpenGLSliderControl::isInside(MathSupport<int>::point pt)
{
    if( pt.x < _position.x )
        return false;
    if( pt.x > _position.x + _size.width)
        return false;
    if( pt.y < _position.y )
        return false;
    if( pt.y > _position.y + _size.height)
        return false;

    return true;
}

void OpenGLSliderControl::update(float dt)
{
    // update current value to match value based on dt
    _currentValue = _value;

}

float OpenGLSliderControl::getValue()
{
    return _currentValue;
}

void OpenGLSliderControl::updateLayout()
{
    // use all state to decide how to position control
    _position = toScrn(_U, _V);
    MathSupport<int>::point pos = toScrn(_U +_dU, _V + _dV);
    _size.width = pos.x - _position.x;
    _size.height = pos.y - _position.y;

    if(_hAlignment == Align_Middle)
    {

    }
    else if( _hAlignment == Align_High)
    {

    }

    if(_vAlignment == Align_Middle)
    {

    }
    else if( _vAlignment == Align_High)
    {

    }

}

MathSupport<int>::point OpenGLSliderControl::toScrn(float U, float V)
{
    MathSupport<float>::point fPt = {_screensize.width*U, _screensize.height * V};
    MathSupport<int>::point pt;
    pt.x = fPt.x;
    pt.y = fPt.y;
    return pt;
}

void OpenGLSliderControl::render(Renderer *r)
{
    // draw outline rectangle of slider
    // draw solid transparent _color rectangle showing current value
    // draw outline rectangle showing final value

}


