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

    void onSize( int cx, int cy);
    void setDimensions(OpenGLSliderControl::Orientation o, float U, float V, float dU, float dV);
    void setSize(float dU, float dV);
    void setThumbThickness(float v);
    void setHAlignment(Alignment alignment);
    void setVAlignment(Alignment alignment);
    void setColor(Vector4F color);

    void setMinValue(float value);
    void setMaxValue(float value);
    void setValue(float value);

    void handleMouseUp( MathSupport<int>::point pt );
    void handleMouseMove( MathSupport<int>::point pt);
    void handleMouseDown( MathSupport<int>::point pt );
    bool isInside(MathSupport<int>::point pt);

    void update(float dt);
    void render(Renderer* r);

    float getValue();

protected:
    void updateLayout();
    MathSupport<int>::point toScrn(float U, float V);

private:
    int _cx, _cy;
    float _U = 0;
    float _V = 0;
    float _dU = 0;
    float _dV = 0;
    Orientation _orientation = {};
    MathSupport<int>::point _position;
    MathSupport<int>::size _size;
    Vector4F _color = Vector4F(1,1,1,1);
    Alignment _hAlignment = {};
    Alignment _vAlignment = {};
    float _thumbThickness = 0.05f;
    float _currentValue = 0;
    float _value = 0;
    float _min = 0;
    float _max = 10;
    float _rateOfValue = 1;
    bool _bMouseDown = false;
};

#endif
