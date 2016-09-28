#ifndef OPENGLBUTTONTEXTURE_H
#define OPENGLBUTTONTEXTURE_H

#include <OpenglTexture2d.h>

class OpenGLTextureRenderer2D;

class OpenGLButtonTexture
{
public:
    enum ButtonStatus
    {
        Button_Disabled = 0x1,
        Button_Hover = 0x2
    };

    enum HorizAlignment
    {
        HAlign_Left,
        HAlign_Middle,
        HAlign_Right
    };

    enum VertAlignment
    {
        VAlign_Top,
        VAlign_Middle,
        VAlign_Bottom
    };

    bool load(std::string filename);
    bool isButtonUp() const;
    bool isButtonDown() const;
    bool isButtonDisabled() const;
    bool isButtonHover() const;

    void setHAlignment(HorizAlignment value);
    void setVAlignment(VertAlignment value);
    void setColor(Vector4F color);
    void setButtonUp();
    void setButtonDown();
    void setButtonDisabled(bool bDisable);
    void setButtonHover(bool bHover);

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
    ButtonStatus _buttonStatus = {};
    bool _bButtonDown = false;
    Vector4F _color = Vector4F(1,1,1,1);
    HorizAlignment _hAlignment = {};
    VertAlignment _vAlignment = {};
};

#endif // OPENGLBUTTONTEXTURE_H
