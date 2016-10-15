#ifndef __OPENGL_PAINTER__H__
#define __OPENGL_PAINTER__H__

class OpenGLFontRenderer2D;
class OpenGLShaderProgram;
class OpenGLFontTexture;
class Renderer;

#include "vector4.h"
#include <string>

class OpenGLPainter
{
public:
    void selectFontRenderer(OpenGLFontRenderer2D *f);
    void selectPrimitiveShader(OpenGLShaderProgram* shader);

    void beginFont(OpenGLFontTexture* font, Vector4F color);
    void renderText(int x, int y, std::string text);
    void endFont();

    void beginPrimitive();
    void endPrimitive();

    Renderer *renderer();

private:
    OpenGLFontRenderer2D* _fontRenderer = 0 ;
    OpenGLShaderProgram* _primitiveShader = 0;
};


#endif //__OPENGL_PAINTER__H__
