#pragma once

class OpenGLFontTexture;
class Renderer;

#include "MathSupport.h"
#include <string>
#include "OpenGLFontMeshBuilder.h"

class OpenGLFontRenderer2D
{
public:
    void selectFont(OpenGLFontTexture* texture);
    OpenGLFontTexture* getFont();
    void onSize( int cx, int cy);
    void setFontColor(const Vector4F& color);
    void beginRender(Renderer *r);
    void renderText( Renderer* r, int x, int y, std::string str);
    void endRender(Renderer* r);

private:
    OpenGLFontMeshBuilder _fontMesh;
    MathSupport<int>::size _screenSize;
    Vector4F _fontColor = {1,1,1,1};
};
