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
	OpenGLFont(Renderer* r);
    void selectShader(OpenGLShaderProgram* p);
    void selectFont(OpenGLFontTexture* texture);
    void setColor( const Vector4F& color);
    void beginRender();
    void renderText(int x, int y, std::string str);
    void renderText(int x, int y, char ch);
    void endRender();
private:
	Renderer* _r = nullptr;
    OpenGLFontMeshBuilder _fontMesh;
    Vector4F _color = {1,1,1,1};
    OpenGLShaderProgram* _shader = 0;
};
