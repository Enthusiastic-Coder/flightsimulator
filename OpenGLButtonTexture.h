#ifndef OPENGLBUTTONTEXTURE_H
#define OPENGLBUTTONTEXTURE_H

#include <OpenglTexture2d.h>

class OpenGLTextureRenderer2D;

class OpenGLButtonTexture
{
public:
    enum ButtonStatus
    {
        Button_Disabled,
        Button_Down,
        Button_Up,
        Button_Hover
    };

    bool load(std::string filename);
    bool isButtonUp() const;
    bool isButtonDown() const;
    bool isButtonDisabled() const;
    bool isButtonHover() const;
    void setButtonUp();
    void setButtonDown();
    void setButtonDisabled();
    void setButtonHover();
    void setPosition( MathSupport<int>::point position);
    void setSize( MathSupport<int>::size size);
    void render(OpenGLTextureRenderer2D* r);
    bool isInside(MathSupport<int>::point pt);

    OpenGLTexture2D* texture();

    MathSupport<int>::point position() const;
    MathSupport<int>::size size() const;

private:
    OpenGLTexture2D _texture;
    MathSupport<int>::point _position;
    MathSupport<int>::size _size;
    ButtonStatus _buttonStatus;
};

#endif // OPENGLBUTTONTEXTURE_H
