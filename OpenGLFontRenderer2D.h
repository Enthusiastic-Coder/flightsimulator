#pragma once

class OpenGLFontTexture;
class Renderer;
class OpenGLShaderProgram;

#include "MathSupport.h"
#include <string>
#include "OpenGLFontMeshBuilder.h"

class OpenGLFontRenderer2D
{
public:
    void selectShader(OpenGLShaderProgram* p);
    void selectRenderer(Renderer* r);
    void selectFont(OpenGLFontTexture* texture);
    OpenGLFontTexture* getFont();
    void onSize( int cx, int cy);
    void setFontColor(const Vector4F& color);
    void beginRender(float offSetX=0.0f, float offSetY=0.0f);
    void renderText(int x, int y, std::string str);
    void endRender();

    Renderer* renderer();
    OpenGLShaderProgram* shader();

private:
    OpenGLFontMeshBuilder _fontMesh;
    MathSupport<int>::size _screenSize;
    Vector4F _fontColor = {1,1,1,1};
    Renderer* _renderer = 0;
    OpenGLShaderProgram* _shader = 0;
};
