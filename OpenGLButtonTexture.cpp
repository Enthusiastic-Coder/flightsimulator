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
    return _buttonStatus == Button_Up;
}

bool OpenGLButtonTexture::isButtonDown() const
{
    return _buttonStatus == Button_Down;
}

bool OpenGLButtonTexture::isButtonDisabled() const
{
    return _buttonStatus == Button_Disabled;
}

bool OpenGLButtonTexture::isButtonHover() const
{
    return _buttonStatus == Button_Hover;
}

void OpenGLButtonTexture::setButtonUp()
{
    _buttonStatus = Button_Up;
}

void OpenGLButtonTexture::setButtonDown()
{
    _buttonStatus = Button_Down;
}

void OpenGLButtonTexture::setButtonDisabled()
{
    _buttonStatus = Button_Disabled;
}

void OpenGLButtonTexture::setButtonHover()
{
    _buttonStatus = Button_Hover;
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
    if( texture()== 0)
        return;

    if( isButtonDisabled())
        r->setColorModulator(Vector4F(1.0f,1.0f,1.0f,0.45f));
    else if( isButtonHover())
        r->setColorModulator(Vector4F(0.8f,0.8f,0.8f,1.0f));
    else
        r->setColorModulator(Vector4F(1.0f,1.0f,1.0f,1.0f));

    MathSupport<int>::point off = {0,0};

    if( isButtonDown())
        off = {5, 5};

    texture()->bind();
    MathSupport<int>::point pt = position();
    MathSupport<int>::size sz = size();
    r->render( pt.x + off.x, pt.y + off.x, sz.width - off.x, sz.height - off.y);
}

bool OpenGLButtonTexture::isInside(MathSupport<int>::point pt)
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
