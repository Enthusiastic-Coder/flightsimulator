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
    Renderer *renderer();

    void selectFontRenderer(OpenGLFontRenderer2D *f);
    void selectPrimitiveShader(OpenGLShaderProgram* shader);

    void beginFont(OpenGLFontTexture* font);
    void setFontColor(Vector4F color);
    void renderText(int x, int y, std::string text);
    void renderText(int x, int y, char ch);
    void endFont();

    void beginPrimitive();
    void setPrimitiveColor(Vector4F color);
    void drawPoint(float x, float y);
    void drawPoints(float* pts, int count);
    void drawLine(float x1, float y1, float x2, float y2);
    void drawLines(const float *pts, int count);
    void drawLineLoop(const float *pts, int count);
    void drawLineStrip(const float *pts, int count);
    void drawTriangle(float x1, float y1, float x2, float y2, float x3, float y3);
    void drawTriangles(float* pts, int count);
    void drawTriangleFan(const float *pts, int count);
    void drawRect(float x, float y, float w, float h);
    void drawQuad(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4);
    void drawQuads(const float* pts, int count);
    void fillTriangle(float x1, float y1, float x2, float y2, float x3, float y3);
    void fillTriangles(float* pts, int count);
    void fillTriangleFan(const float* pts, int count);
    void fillRect(float x, float y, float w, float h);
    void fillQuad(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4);
    void fillQuads(const float *pts, int count);
    void endPrimitive();

private:
    void drawPrimitive(const float* pts, int count, int primType);

private:
    OpenGLFontRenderer2D* _fontRenderer = 0 ;
    OpenGLShaderProgram* _primitiveShader = 0;
    Vector4F _primitiveColor = Vector4F(1,1,1,1);
};


#endif //__OPENGL_PAINTER__H__
