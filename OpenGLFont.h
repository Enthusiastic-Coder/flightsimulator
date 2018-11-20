#pragma once

class OpenGLFontTexture;
class Renderer;
class OpenGLShaderProgram;

#include "MathSupport.h"
#include <string>
#include "OpenGLFontMeshBuilder.h"

class OpenGLFont
{
public:
    void selectShader(OpenGLShaderProgram* p);
    void selectFont(OpenGLFontTexture* texture);
    OpenGLFontTexture* getFont();
    void setColor( const Vector4F& color);
    void beginRender(Renderer* r);
    void renderText(Renderer* r, int x, int y, std::string str);
    void renderText(Renderer* r, int x, int y, char ch);
    void endRender(Renderer* r);
private:
    OpenGLFontMeshBuilder _fontMesh;
    Vector4F _color = {1,1,1,1};
    OpenGLShaderProgram* _shader = 0;
};
