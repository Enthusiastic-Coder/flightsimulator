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

    void beginFont(OpenGLFontTexture* font, Vector4F color=Vector4F(1,1,1,1));
    void renderText(int x, int y, std::string text);
    void endFont();

    void beginPrimitive();
    void setPrimitiveColor(Vector4F color);
    void fillQuad(int x, int y, int w, int h);
    void drawQuad(int x, int y, int w, int h);
    void drawLine(int x1, int y1, int x2, int y2);
    void drawLines(int* pts, int count);
    void drawLineLoop( int* pts, int count);
    void drawLineStrip(int* pts, int count);
    void drawTriangles(int* pts, int count);
    void fillTriangles(int* pts, int count);
    void drawTriangleFan(int* pts, int count);
    void fillTriangleFan(int* pts, int count);
    void drawQuads(int* pts, int count);
    void fillQuads(int* pts, int count);
    void endPrimitive();

    Renderer *renderer();

private:
    OpenGLFontRenderer2D* _fontRenderer = 0 ;
    OpenGLShaderProgram* _primitiveShader = 0;
};


#endif //__OPENGL_PAINTER__H__
