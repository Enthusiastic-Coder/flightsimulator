#include "stdafx.h"
#include "OpenGLSliderControl.h"
#include <OpenGLRenderer.h>
#include <OpenGLPipeline.h>


void OpenGLSliderControl::onSize(int cx, int cy)
{
    _cx = cx;
    _cy = cy;
    updateLayout();
}

void OpenGLSliderControl::setDimensions(OpenGLSliderControl::Orientation o, float U, float V, float dU, float dV)
{
    _orientation = o;
    _U = U;
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

void OpenGLSliderControl::handleMouseMove(MathSupport<int>::point pt)
{
    handleMouseDown(pt);
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
    MathSupport<float>::point fPt = {_cx * U, _cy * V};
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
    OpenGLPipeline& pipeline = OpenGLPipeline::Get(r->camID);
    pipeline.GetModel().Push();
    pipeline.GetProjection().LoadIdentity();
    pipeline.GetProjection().SetOrthographic(0, _cx, _cy, 0, -1, 1);
    pipeline.GetModel().LoadIdentity();
    pipeline.GetView().LoadIdentity();

    pipeline.bindMatrices(r->progId());
    r->progId().sendUniform("colorModulator", _color);

    r->setUseIndex(false);

    {
        float vertices[] = {
            _position.x, _position.y, 0.0f,
            _position.x + _size.width, _position.y, 0.0f,
            _position.x + _size.width, _position.y + _size.height, 0.0f,
            _position.x, _position.y + _size.height, 0.0f
        };

        float colors[] = {
            1,1,1, 1,
            1,1,1, 1,
            1,1,1, 1,
            1,1,1, 1,
        };

        r->bindVertex(Renderer::Vertex, 3, vertices);
        r->bindVertex(Renderer::Color, 4, colors);

        r->setVertexCountOffset(indicesCount(vertices,3));
        r->setPrimitiveType(GL_LINE_LOOP);
        r->Render();
    }

    {
        int iThumbHeight;

        r->setPrimitiveType(GL_TRIANGLE_FAN);

        if(_orientation == Orient_Horizontal)
        {
            iThumbHeight = _size.width * _thumbThickness;
            int thumbPos = float(_currentValue) / (_max - _min) * _size.width;

            float vertices[] = {
                _position.x + thumbPos - iThumbHeight, _position.y, 0.0f,
                _position.x + thumbPos + iThumbHeight, _position.y, 0.0f,
                _position.x + thumbPos + iThumbHeight, _position.y + _size.height, 0.0f,
                _position.x + thumbPos - iThumbHeight, _position.y + _size.height, 0.0f
            };

            float colors[] = {
                1,1,1,0.5,
                1,1,1,0.5,
                1,1,1,0.5,
                1,1,1,0.5,
            };

            r->bindVertex(Renderer::Vertex, 3, vertices);
            r->bindVertex(Renderer::Color, 4, colors);
            r->setVertexCountOffset(indicesCount(vertices,3));
            r->Render();
        }
        else
        {
            iThumbHeight = _size.height * _thumbThickness;
            int thumbPos = float(_currentValue) / (_max - _min) * _size.height;

            float vertices[] = {
                _position.x , _position.y + thumbPos - iThumbHeight, 0.0f,
                _position.x + _size.width, _position.y + thumbPos - iThumbHeight, 0.0f,
                _position.x + _size.width, _position.y + thumbPos + iThumbHeight, 0.0f,
                _position.x, _position.y + thumbPos + iThumbHeight, 0.0f
            };

            float colors[] = {
                1,1,1,0.5,
                1,1,1,0.5,
                1,1,1,0.5,
                1,1,1,0.5,
            };

            r->bindVertex(Renderer::Vertex, 3, vertices);
            r->bindVertex(Renderer::Color, 4, colors);
            r->setVertexCountOffset(indicesCount(vertices,3));
            r->Render();
        }
    }

    r->unBindBuffers();
    pipeline.GetModel().Pop();
}


