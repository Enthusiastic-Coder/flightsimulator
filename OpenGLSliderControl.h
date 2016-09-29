#ifndef __OPENGL_SLIDER_CONTROL__
#define __OPENGL_SLIDER_CONTROL__

#include <MathSupport.h>
#include <vector4.h>

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

    void setPosition( MathSupport<int>::point pos);


private:
    MathSupport<int>::point _position;
    MathSupport<int>::size _size;
    Vector4F _color = Vector4F(1,1,1,1);
    Alignment _hAlignment = {};
    Alignment _vAlignment = {};
    int _currentValue = 0;
    int _value = 0;
    int _min = 0;
    int _max = 10;
};

#endif
