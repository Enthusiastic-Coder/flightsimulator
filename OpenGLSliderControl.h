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
    void setTickValue(float value);

    void handleMouseUp( MathSupport<int>::point pt );
    void handleMouseMove( MathSupport<int>::point pt);
    void handleMouseDown( MathSupport<int>::point pt );
    bool isInside(MathSupport<int>::point pt);

    void update(float dt);
    void render(Renderer* r);

    float getValue() const;
    float getCurrentValue() const;

protected:
    void updateLayout();
    MathSupport<int>::point toScrn(float U, float V);
    void renderThumb(Renderer* r, int primitiveType, float value, float alpha);

private:
    int _cx = 1;
    int _cy = 1;
    float _U = 0;
    float _V = 0;
    float _dU = 0.1f;
    float _dV = 0.1f;
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
    float _rateOfValue = 5.0f;
    float _tickValue = 0.5f;
    bool _bMouseDown = false;
};

#endif
