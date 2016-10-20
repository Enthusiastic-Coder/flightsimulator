#include "stdafx.h"
#include "OpenGLSliderControl.h"
#include <OpenGLRenderer.h>
#include <OpenGLPipeline.h>
#include <algorithm>
#include <vector>

void OpenGLSliderControl::onSize(int cx, int cy)
{
    _cx = cx;
    _cy = cy;
    updateLayout();
}

void OpenGLSliderControl::setDimensions(OpenGLSliderControl::Orientation o, float U, float V, float dU, float dV)
{
    _orientation = o;
    _u = U;
    _V = V;
    _dU = dU;
    _dV = dV;
    updateLayout();
}

void OpenGLSliderControl::setThumbThickness(float v)
{
    _thumbThickness = v;
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

void OpenGLSliderControl::setRange(float low, float high)
{
    _min = low;
    _max = high;
}

void OpenGLSliderControl::setValue(float value)
{
    _value = value;
}

void OpenGLSliderControl::setTickValue(float value)
{
    _tickValue = value;
}

void OpenGLSliderControl::setRateValue(float rate)
{
    _rateOfValue = rate;
}

void OpenGLSliderControl::handleMouseUp(MathSupport<int>::point pt)
{
    _bMouseDown = false;
}

bool OpenGLSliderControl::handleMouseMove(MathSupport<int>::point pt)
{
    if(_bMouseDown)
        return handleMouseDown(pt);
    else
        return false;
}

bool OpenGLSliderControl::handleMouseDown(MathSupport<int>::point pt)
{
    if( !isInside(pt))
        return false;

    _bMouseDown = true;

    if( _orientation == Orient_Horizontal)
        _value = _max - float(pt.x - _position.x) / _size.width * (_max - _min) ;
    else
        _value = _max - float(pt.y - _position.y) / _size.height * (_max - _min) ;

    return true;
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

    float dRateValue = _rateOfValue * dt;
    float dValue = _value - _currentValue;
    float dFinalRateValue = std::min(fabs(dValue), dRateValue);
    if( dValue < 0.0f)  dFinalRateValue = -dFinalRateValue;
    _currentValue += dFinalRateValue;
}

void OpenGLSliderControl::updateLayout()
{
    // use all state to decide how to position control
    _position = toScrn(_u, _V);
    MathSupport<int>::point pos = toScrn(_u +_dU, _V + _dV);
    _size.width = pos.x - _position.x;
    _size.height = pos.y - _position.y;

    if(_hAlignment == Align_Middle)
        _position.x -= _size.width /2;
    else if( _hAlignment == Align_High)
        _position.x -= _size.width+1;

    if(_vAlignment == Align_Middle)
        _position.y -= _size.height /2;
    else if( _vAlignment == Align_High)
        _position.y -= _size.height+1;
}

MathSupport<int>::point OpenGLSliderControl::toScrn(float U, float V)
{
    MathSupport<float>::point fPt = {_cx * U, _cy * V};
    MathSupport<int>::point pt = { fPt.x, fPt.y};
    return pt;
}

void OpenGLSliderControl::renderThumb(Renderer *r, int primitiveType, float value, Vector4F color)
{
    float colors[] = {
        color.x, color.y, color.z, color.w,
        color.x, color.y, color.z, color.w,
        color.x, color.y, color.z, color.w,
        color.x, color.y, color.z, color.w,
    };

    r->setPrimitiveType(primitiveType);

    if(_orientation == Orient_Horizontal)
    {
        float iThumbHeight = _size.width * _thumbThickness;
        float thumbPos = _size.width - float(value) / (_max - _min) * _size.width;

        float vertices[] = {
            _position.x + thumbPos - iThumbHeight, _position.y, 0.0f,
            _position.x + thumbPos + iThumbHeight, _position.y, 0.0f,
            _position.x + thumbPos + iThumbHeight, _position.y + _size.height, 0.0f,
            _position.x + thumbPos - iThumbHeight, _position.y + _size.height, 0.0f
        };

        r->bindVertex(Renderer::Vertex, 3, vertices);
        r->bindVertex(Renderer::Color, 4, colors);
        r->setVertexCountOffset(indicesCount(vertices,3));
        r->Render();
        r->unBindBuffers();
    }
    else
    {
        float iThumbHeight = _size.height * _thumbThickness;
        float thumbPos = _size.height - float(value) / (_max - _min) * _size.height;

        float vertices[] = {
            _position.x , _position.y + thumbPos - iThumbHeight, 0.0f,
            _position.x + _size.width, _position.y + thumbPos - iThumbHeight, 0.0f,
            _position.x + _size.width, _position.y + thumbPos + iThumbHeight, 0.0f,
            _position.x, _position.y + thumbPos + iThumbHeight, 0.0f
        };

        r->bindVertex(Renderer::Vertex, 3, vertices);
        r->bindVertex(Renderer::Color, 4, colors);
        r->setVertexCountOffset(indicesCount(vertices,3));
        r->Render();
        r->unBindBuffers();
    }
}

void OpenGLSliderControl::render(Renderer *r)
{
    // draw outline rectangle of slider
    // draw solid transparent _color rectangle showing current value
    // draw outline rectangle showing final value
    OpenGLPipeline& pipeline = OpenGLPipeline::Get(r->camID);
    pipeline.GetProjection().LoadIdentity();
    pipeline.GetProjection().SetOrthographic(0, _cx, _cy, 0, -1, 1);
    pipeline.GetModel().LoadIdentity();
    pipeline.GetView().LoadIdentity();
    pipeline.bindMatrices(r->progId());

    r->setUseIndex(false);

    float vertices[] = {
        _position.x, _position.y, 0.0f,
        _position.x + _size.width, _position.y, 0.0f,
        _position.x + _size.width, _position.y + _size.height, 0.0f,
        _position.x, _position.y + _size.height, 0.0f
    };

    float colors[] = {
        1,1,1, 0.25,
        1,1,1, 0.25,
        1,1,1, 0.25,
        1,1,1, 0.25,
    };

    r->bindVertex(Renderer::Vertex, 3, vertices);
    r->bindVertex(Renderer::Color, 4, colors);

    r->setVertexCountOffset(indicesCount(vertices,3));
    r->setPrimitiveType(GL_TRIANGLE_FAN);

    r->Render();

    std::vector<Vector3F> v;
    std::vector<Vector4F> c;

    float fDiffTickDist = _tickValue/(_max- _min) * (_orientation == Orient_Horizontal?_size.width:_size.height);
    float fFullTickDist = (_orientation == Orient_Horizontal?_size.width:_size.height);
    int spill=5;

    for( float i=0; i < fFullTickDist; i+= fDiffTickDist)
    {
        c.push_back(Vector4F(1,1,1,1));
        c.push_back(Vector4F(1,1,1,1));

        if( _orientation == Orient_Horizontal)
        {
            v.push_back(Vector3F(_position.x + i, _position.y+spill, 0));
            v.push_back(Vector3F(_position.x + i, _position.y + _size.height-spill, 0));
        }
        else
        {
            v.push_back(Vector3F(_position.x+spill, _position.y + i, 0));
            v.push_back(Vector3F(_position.x + _size.width-spill, _position.y + i, 0));
        }
    }

    Vector4F outlineColor(0.25,0.25,0.25,1);

    if( _orientation == Orient_Horizontal)
    {
        float thumbPos = _size.width- _currentValue / (_max - _min) * _size.width;

        for(int i=-1; i <= 1; ++i)
        {
            c.push_back(outlineColor);
            c.push_back(outlineColor);

            v.push_back(Vector3F(_position.x + thumbPos+i, _position.y-spill, 0));
            v.push_back(Vector3F(_position.x + thumbPos+i, _position.y + _size.height+spill, 0));
        }
    }
    else
    {
        float thumbPos = _size.height - _currentValue / (_max - _min) * _size.height;

        for(int i=0; i < 3; ++i)
        {
            c.push_back(outlineColor);
            c.push_back(outlineColor);
            v.push_back(Vector3F(_position.x -spill, _position.y + thumbPos+i, 0));
            v.push_back(Vector3F(_position.x + _size.width+spill, _position.y + thumbPos+i, 0));
        }
    }

    r->setPrimitiveType(GL_LINES);
    r->setVertexCountOffset(v.size());

    r->bindVertex(Renderer::Vertex, 3, (float*) &v[0].x);
    r->bindVertex(Renderer::Color, 4,(float*) &c[0].x);

    r->Render();
    r->unBindBuffers();

    renderThumb(r, GL_TRIANGLE_FAN, _currentValue, Vector4F(1,1,1,0.5f));
    renderThumb(r, GL_LINE_LOOP, _value, outlineColor);

}

float OpenGLSliderControl::getCurrentValue() const
{
    return _currentValue;
}

float OpenGLSliderControl::getValue() const
{
    return _value;
}

