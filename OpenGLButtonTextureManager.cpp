#include "OpenGLButtonTextureManager.h"
#include "OpenGLRenderer.h"
#include "OpenGLButtonTexture.h"

OpenGLButtonTextureManager::OpenGLButtonTextureManager(OpenGLTextureRenderer2D *r) :
    _textureRenderer(r)
{

}

void OpenGLButtonTextureManager::setButtonPos(OpenGLButtonTexture *button, float u, float v, float cx, float cy)
{
    _buttonStates[button].offset = {u, v};
    _buttonStates[button].dims.width = cx;
    _buttonStates[button].dims.height = cy;
    onSizeLayout(button);
}

void OpenGLButtonTextureManager::setButtonToggle(OpenGLButtonTexture *button, bool bToggle)
{
    _buttonStates[button].bToggle = bToggle;
}

void OpenGLButtonTextureManager::setButtonVisibility(OpenGLButtonTexture *button, bool bShow)
{
    _buttonStates[button].bVisible = bShow;
    onSizeLayout(button);
}

void OpenGLButtonTextureManager::setVisibleOnHover(OpenGLButtonTexture *button, bool bVisibleOnHover)
{
    _buttonStates[button].bVisibleOnHover = bVisibleOnHover;
}

void OpenGLButtonTextureManager::handleMouseMove(MathSupport<int>::point pt)
{
    _cursorPos = pt;
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
    _cursorPos = pt;
    std::map<OpenGLButtonTexture*, buttonState>::iterator it;
    for( it = _buttonStates.begin(); it != _buttonStates.end(); ++it)
        if( it->first->isInside(pt))
        {
            _buttonMouseDown = it->first;
            _bButtonMouseWasDown = _buttonMouseDown->isButtonDown();
            _buttonMouseDown->setButtonDown();
            return;
        }
}

void OpenGLButtonTextureManager::handleMouseUp(MathSupport<int>::point pt)
{
    _cursorPos = pt;
    std::map<OpenGLButtonTexture*, buttonState>::iterator it;
    for( it = _buttonStates.begin(); it != _buttonStates.end(); ++it)
        if( it->first == _buttonMouseDown && it->first->isInside(pt) )
        {
            _buttonClicked = it->first;
            break;
        }

    if( _buttonMouseDown != 0)
    {
        buttonState& bs = _buttonStates[_buttonMouseDown];

        if(bs.bToggle && !_buttonMouseDown->isInside(pt))
        {
            if( !_bButtonMouseWasDown)
            {
                _buttonMouseDown->setButtonUp();
                _buttonMouseDown = 0;
            }
        }
        else if(!bs.bToggle || bs.bToggle && _bButtonMouseWasDown)
        {
            _buttonMouseDown->setButtonUp();
            _buttonMouseDown = 0;
        }
        else
            _buttonMouseDown->setButtonDown();
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
        {
            if( !state.bVisibleOnHover || texture->isInside(_cursorPos))
                texture->render(_textureRenderer);
        }
    }

    _textureRenderer->endRender();
}

void OpenGLButtonTextureManager::onSizeLayout(OpenGLButtonTexture *filteredButton)
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

        state.position.x = state.offset.x * screenWidth;
        state.position.y = state.offset.y * screenHeight;
        state.size.width = state.dims.width * screenWidth;
        state.size.height = state.dims.height * screenHeight;
    }
}
