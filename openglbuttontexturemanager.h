#ifndef OPENGLBUTTONTEXTUREMANAGER_H
#define OPENGLBUTTONTEXTUREMANAGER_H

#include <map>
#include "opengltexturerenderer2d.h"
#include <MathSupport.h>

class Renderer;
class OpenGLButtonTexture;

class OpenGLButtonTextureManager
{
public:
    enum AnchorState
    {
        Anchor_Top,
        Anchor_Right,
        Anchor_Bottom,
        Anchor_Left
    };

    OpenGLButtonTextureManager(OpenGLTextureRenderer2D* r);

    void setButtonAnchor(OpenGLButtonTexture* button, AnchorState anchorState);
    void setButtonPos(OpenGLButtonTexture* button, float offset, float cx, float cy);
    void setButtonVisibility( OpenGLButtonTexture* button, bool bShow);

    void handleMouseMove( MathSupport<int>::point pt);
    void handleMouseDown( MathSupport<int>::point pt );
    void handleMouseUp( MathSupport<int>::point pt );

    bool buttonClicked(OpenGLButtonTexture *button);
    bool buttonPressedDown(OpenGLButtonTexture *button);

    void update(float dt);
    void render();
    void prepareLayout(OpenGLButtonTexture* filteredButton);

private:
    struct buttonState
    {
        MathSupport<int>::point position;
        MathSupport<int>::size size;
        float pixelRate = 20.0f;
        AnchorState anchorState = Anchor_Top;
        float offset = 0.0f;
        MathSupport<float>::size dims = {0.1f, 0.1f};
        bool bVisible = true;
    };
    OpenGLTextureRenderer2D* _textureRenderer;
    std::map<OpenGLButtonTexture*, buttonState> _buttonStates;
    OpenGLButtonTexture* _buttonMouseDown = 0;
    OpenGLButtonTexture* _buttonClicked = 0;
    OpenGLButtonTexture* _buttonHovered = 0;
};

#endif // OPENGLBUTTONTEXTUREMANAGER_H
