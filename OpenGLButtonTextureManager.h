#ifndef OPENGLBUTTONTEXTUREMANAGER_H
#define OPENGLBUTTONTEXTUREMANAGER_H

#include <map>
#include "OpenGLTextureRenderer2D.h"
#include <MathSupport.h>

class Renderer;
class OpenGLButtonTexture;

class OpenGLButtonTextureManager
{
public:
    OpenGLButtonTextureManager(OpenGLTextureRenderer2D* r);

    void setButtonPos(OpenGLButtonTexture* button, float u, float v, float cx, float cy);
    void setButtonToggle(OpenGLButtonTexture *button, bool bToggle);
    void setButtonVisibility( OpenGLButtonTexture* button, bool bShow);
    void setVisibleOnHover(OpenGLButtonTexture* button, bool bVisibleOnHover);

    void handleMouseMove( MathSupport<int>::point pt);
    void handleMouseDown( MathSupport<int>::point pt );
    void handleMouseUp( MathSupport<int>::point pt );

    bool buttonClicked(OpenGLButtonTexture *button);
    bool buttonPressedDown(OpenGLButtonTexture *button);

    void update(float dt);
    void render();
    void onSizeLayout(OpenGLButtonTexture* filteredButton=0);

private:
    struct buttonState
    {
        MathSupport<int>::point position;
        MathSupport<int>::size size;
        bool bToggle = false;
        float pixelRate = 20.0f;
        MathSupport<float>::point offset = {};
        MathSupport<float>::size dims = {0.1f, 0.1f};
        bool bVisible = true;
        bool bVisibleOnHover = false;
    };
    OpenGLTextureRenderer2D* _textureRenderer;
    std::map<OpenGLButtonTexture*, buttonState> _buttonStates;
    OpenGLButtonTexture* _buttonMouseDown = 0;
    bool _bButtonMouseWasDown = false;
    OpenGLButtonTexture* _buttonClicked = 0;
    OpenGLButtonTexture* _buttonHovered = 0;
    MathSupport<int>::point _cursorPos = {};
};


#endif // OPENGLBUTTONTEXTUREMANAGER_H
