#ifndef __OPENGL_SLIDER_CONTROL__
#define __OPENGL_SLIDER_CONTROL__

#include <MathSupport.h>
#include <vector4.h>

class Renderer;

class OpenGLSliderControl
{
public:
    enum Alignment
    {
        Align_Low,
        Align_Middle,
        Align_High
    };

    enum Orientation
    {
        Orient_Horizontal,
        Orient_Vertical
    };

    void setScreenSize( MathSupport<int>::size sz);
    void setPosition( float U, float V);
    void setSize(float dU, float dV);
    void setOrientation(Orientation o);
    void setHAlignment(Alignment alignment);
    void setVAlignment(Alignment alignment);
    void setColor(Vector4F color);

    void setMinValue(float value);
    void setMaxValue(float value);
    void setValue(float value);

    void update(float dt);
    void render(Renderer* r);

protected:
    void updateLayout();

private:
    MathSupport<int>::size _screensize;
    float _U, _V;
    float _dU, _dV;
    Vector4F _color = Vector4F(1,1,1,1);
    Orientation _orientation = {};
    Alignment _hAlignment = {};
    Alignment _vAlignment = {};
    float _currentValue = 0;
    float _value = 0;
    float _min = 0;
    float _max = 10;
    float _rateOfValue = 1;
};

#endif
