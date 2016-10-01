#include "OpenGLButtonTexture.h"
#include "OpenGLTextureRenderer2D.h"

bool OpenGLButtonTexture::load(std::string filename)
{
    _texture.setMagnification(GL_NEAREST);
    _texture.setMinification(GL_NEAREST);
    return _texture.Load(filename);
}

bool OpenGLButtonTexture::isButtonUp() const
{
    return !_bButtonDown;
}

bool OpenGLButtonTexture::isButtonDown() const
{
    return _bButtonDown;
}

bool OpenGLButtonTexture::isButtonDisabled() const
{
    return _buttonStatus & Button_Disabled;
}

bool OpenGLButtonTexture::isButtonHover() const
{
    return _buttonStatus & Button_Hover;
}

void OpenGLButtonTexture::setHAlignment(Alignment value)
{
    _hAlignment = value;
}

void OpenGLButtonTexture::setVAlignment(Alignment value)
{
    _vAlignment = value;
}

void OpenGLButtonTexture::setColor(Vector4F color)
{
    _color = color;
}

void OpenGLButtonTexture::setButtonUp()
{
    _bButtonDown = false;
}

void OpenGLButtonTexture::setButtonDown()
{
    _bButtonDown = true;
}

void OpenGLButtonTexture::setButtonDisabled(bool bDisable)
{
    bDisable ?
    (int&)_buttonStatus |= Button_Disabled:
            (int&)_buttonStatus &= ~Button_Disabled;
}

void OpenGLButtonTexture::setButtonHover(bool bHover)
{
    bHover ?
    (int&)_buttonStatus |= Button_Hover:
            (int&)_buttonStatus &= ~Button_Hover;
}

void OpenGLButtonTexture::setPosition(MathSupport<int>::point position)
{
    _position = position;
}

void OpenGLButtonTexture::setSize(MathSupport<int>::size size)
{
    _size = size;
}

void OpenGLButtonTexture::render(OpenGLTextureRenderer2D *r)
{
    if( isButtonDisabled())
        r->setColorModulator(Vector4F(1.0f,1.0f,1.0f,0.45f));
    else if( isButtonHover())
        r->setColorModulator(Vector4F(0.8f,0.8f,0.8f,1.0f));
    else
        r->setColorModulator(_color);

    MathSupport<int>::point off = {0,0};

    if( isButtonDown())
        off = {5, 5};

    _texture.bind();

    MathSupport<int>::point pt = actualPosition();
    MathSupport<int>::size sz = size();

    r->render( pt.x + off.x, pt.y + off.x, sz.width - off.x, sz.height - off.y);
}

bool OpenGLButtonTexture::isInside(MathSupport<int>::point pt)
{
    MathSupport<int>::point pos = actualPosition();
    if( pt.x < pos.x )
        return false;
    if( pt.x > pos.x + _size.width)
        return false;
    if( pt.y < pos.y )
        return false;
    if( pt.y > pos.y + _size.height)
        return false;

    return true;
}

OpenGLTexture2D *OpenGLButtonTexture::texture()
{
    return &_texture;
}

MathSupport<int>::point OpenGLButtonTexture::position() const
{
    return _position;
}

MathSupport<int>::size OpenGLButtonTexture::size() const
{
    return _size;
}

MathSupport<int>::point OpenGLButtonTexture::actualPosition() const
{
    MathSupport<int>::point pt = position();
    MathSupport<int>::size sz = size();

    switch(_hAlignment)
    {
    case Align_Low:
        break;
    case Align_Middle:
        pt.x -= sz.width/2+1;
        break;
    case Align_High:
        pt.x -= sz.width +1;
        break;
    }

    switch(_vAlignment)
    {
    case Align_Low:
        break;
    case Align_Middle:
        pt.y -= sz.height/2 +1;
        break;
    case Align_High:
        pt.y -= sz.height +1;
        break;
    }

    return pt;
}
