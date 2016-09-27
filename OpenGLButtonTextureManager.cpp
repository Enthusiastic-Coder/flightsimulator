#include "OpenGLButtonTextureManager.h"
#include "OpenGLRenderer.h"
#include "OpenGLButtonTexture.h"

OpenGLButtonTextureManager::OpenGLButtonTextureManager(OpenGLTextureRenderer2D *r) :
    _textureRenderer(r)
{

}

void OpenGLButtonTextureManager::setButtonAnchor(OpenGLButtonTexture *button, OpenGLButtonTextureManager::AnchorState anchorState)
{
    _buttonStates[button].anchorState = anchorState;
    prepareLayout(button);
}

void OpenGLButtonTextureManager::setButtonPos(OpenGLButtonTexture *button, float offset, float cx, float cy)
{
    _buttonStates[button].offset = offset;
    _buttonStates[button].dims.width = cx;
    _buttonStates[button].dims.height = cy;
    prepareLayout(button);
}

void OpenGLButtonTextureManager::setButtonVisibility(OpenGLButtonTexture *button, bool bShow)
{
    _buttonStates[button].bVisible = bShow;
    prepareLayout(button);
}

void OpenGLButtonTextureManager::handleMouseMove(MathSupport<int>::point pt)
{
    if(_buttonHovered !=0 )
    {
        _buttonHovered->setButtonHover(false);
        _buttonHovered = 0;
    }

    std::map<OpenGLButtonTexture*, buttonState>::iterator it;
    for( it = _buttonStates.begin(); it != _buttonStates.end(); ++it)
        if( it->first->isInside(pt))
        {
            _buttonHovered = it->first;
            _buttonHovered->setButtonHover(true);
            return;
        }
}

void OpenGLButtonTextureManager::handleMouseDown(MathSupport<int>::point pt)
{
    std::map<OpenGLButtonTexture*, buttonState>::iterator it;
    for( it = _buttonStates.begin(); it != _buttonStates.end(); ++it)
        if( it->first->isInside(pt))
        {
            _buttonMouseDown = it->first;
            _buttonMouseDown->setButtonDown();
            return;
        }
}

void OpenGLButtonTextureManager::handleMouseUp(MathSupport<int>::point pt)
{
    std::map<OpenGLButtonTexture*, buttonState>::iterator it;
    for( it = _buttonStates.begin(); it != _buttonStates.end(); ++it)
        if( it->first == _buttonMouseDown && it->first->isInside(pt) )
        {
            _buttonClicked = it->first;
            break;
        }

    if( _buttonMouseDown != 0)
    {
        _buttonMouseDown->setButtonUp();
        _buttonMouseDown = 0;
    }
}

bool OpenGLButtonTextureManager::buttonClicked(OpenGLButtonTexture* button)
{
    bool bClicked = _buttonClicked == button;
    if( bClicked)
        _buttonClicked = 0;
    return bClicked;
}

bool OpenGLButtonTextureManager::buttonPressedDown(OpenGLButtonTexture* button)
{
    return _buttonMouseDown == button;
}

void OpenGLButtonTextureManager::update(float dt)
{
    //use dt to update position and sizes of each of the button
    std::map<OpenGLButtonTexture*, buttonState>::iterator it;
    for( it = _buttonStates.begin(); it != _buttonStates.end(); ++it)
    {
        buttonState& state = it->second;
        if( !state.bVisible)
            continue;

        OpenGLButtonTexture* texture = it->first;
        texture->setPosition(state.position);
        texture->setSize(state.size);
    }
}

void OpenGLButtonTextureManager::render()
{
    _textureRenderer->beginRender();

    std::map<OpenGLButtonTexture*, buttonState>::iterator it;
    for( it = _buttonStates.begin(); it != _buttonStates.end(); ++it)
    {
        OpenGLButtonTexture* texture = it->first;
        buttonState& state = it->second;
        if( state.bVisible )
            texture->render(_textureRenderer);
    }

    _textureRenderer->endRender();
}

void OpenGLButtonTextureManager::prepareLayout(OpenGLButtonTexture *filteredButton)
{
    int screenWidth = _textureRenderer->width();
    int screenHeight = _textureRenderer->height();

    std::map<OpenGLButtonTexture*, buttonState>::iterator it;
    for( it = _buttonStates.begin(); it != _buttonStates.end(); ++it)
    {
        if( filteredButton != 0)
            if( filteredButton != it->first )
                continue;

        buttonState& state = it->second;

        if( !state.bVisible)
            continue;

        int xPos = 0;
        int yPos = 0;
        int width = state.dims.width * screenWidth;
        int height = state.dims.height * screenHeight;

        switch( state.anchorState)
        {
        case Anchor_Top:
            xPos = state.offset * screenWidth;
            yPos = 0;
            break;
        case Anchor_Right:
            xPos = screenWidth - width;
            yPos = state.offset * screenHeight;
            break;
        case Anchor_Bottom:
            xPos = state.offset * screenWidth;
            yPos = screenHeight - height;
            break;
        case Anchor_Left:
            xPos = 0;
            yPos = state.offset * screenHeight;
            break;

        }

        state.position.x = xPos;
        state.position.y = yPos;
        state.size.width = width;
        state.size.height = height;
    }
}
