#include "stdafx.h"
#include "OpenGLPainter.h"
#include "OpenGLFontRenderer2D.h"
#include "OpenGLShaderProgram.h"
#include "OpenGLFontTexture.h"
#include "OpenGLRenderer.h"
#include "OpenGLPipeline.h"

Renderer *OpenGLPainter::renderer()
{
    return _fontRenderer->renderer();
}

void OpenGLPainter::selectFontRenderer(OpenGLFontRenderer2D *f)
{
    _fontRenderer = f;
}

void OpenGLPainter::selectPrimitiveShader(OpenGLShaderProgram *shader)
{
    _primitiveShader = shader;
}

void OpenGLPainter::beginFont(OpenGLFontTexture *font, Vector4F color)
{
    _fontRenderer->selectFont(font);
    _fontRenderer->beginRender();
}

void OpenGLPainter::setFontColor(Vector4F color)
{
    _fontRenderer->setFontColor(color);
}

void OpenGLPainter::renderText(int x, int y, std::string text)
{
    _fontRenderer->renderText(x, y, text);
}

void OpenGLPainter::endFont()
{
    _fontRenderer->endRender();
}

void OpenGLPainter::beginPrimitive()
{
    Renderer* r = renderer();
    r->useProgram(*_primitiveShader);
    OpenGLPipeline::Get(r->camID).bindMatrices(r->progId());
    setPrimitiveColor(_primitiveColor);
    r->setUseIndex(false);
}

void OpenGLPainter::setPrimitiveColor(Vector4F color)
{
    _primitiveColor = color;
    renderer()->progId().sendUniform("primitiveColor", _primitiveColor);
}

void OpenGLPainter::drawPoint(float x, float y)
{
    float vertices[] = {
        x,y
    };

    drawPoints(vertices, 2);
}

void OpenGLPainter::drawPoints(float *pts, int count)
{
    drawPrimitive(pts, count, GL_POINTS);
}

void OpenGLPainter::drawLine(float x1, float y1, float x2, float y2)
{
    float vertices[] = {
        x1, y1,
        x2, y2
    };

    Renderer* r = renderer();
    r->bindVertex(Renderer::Vertex, 2, vertices);
    r->setVertexCountOffset( indicesCount(vertices,2));
    r->setPrimitiveType(GL_LINES);
    r->Render();
}

void OpenGLPainter::drawLines(const float *pts, int count)
{
     drawPrimitive(pts, count, GL_LINES);
}

void OpenGLPainter::drawLineLoop(const float *pts, int count)
{
    drawPrimitive(pts, count, GL_LINE_LOOP);
}

void OpenGLPainter::drawLineStrip(float *pts, int count)
{
    drawPrimitive(pts, count, GL_LINE_STRIP);
}

void OpenGLPainter::drawTriangle(float x1, float y1, float x2, float y2, float x3, float y3)
{
    float vertices[] = {
        x1, y1,
        x2, y2,
        x3, y3
    };
    Renderer* r = renderer();
    r->bindVertex(Renderer::Vertex, 2, vertices);
    r->setVertexCountOffset( indicesCount(vertices,2));
    r->setPrimitiveType(GL_LINE_LOOP);
    r->Render();
}

void OpenGLPainter::drawTriangles(float *pts, int count)
{
    Renderer* r = renderer();
    r->setPrimitiveType(GL_LINE_LOOP);

    for( int i=0; i < count; i+= 2*3)
    {
        r->bindVertex(Renderer::Vertex, 2, pts+i);
        r->setVertexCountOffset(3, 0);
        r->Render();
    }
}

void OpenGLPainter::drawTriangleFan(const float *pts, int count)
{
    drawPrimitive(pts, count, GL_TRIANGLE_FAN);
}

void OpenGLPainter::drawQuad(float x, float y, float w, float h)
{
    float vertices[] = {
        x, y,
        x+w, y,
        x+w, y+h,
        x, y+h
    };

    Renderer* r = renderer();
    r->bindVertex(Renderer::Vertex, 2, vertices);
    r->setVertexCountOffset( indicesCount(vertices,2));
    r->setPrimitiveType(GL_LINE_LOOP);
    r->Render();
}

void OpenGLPainter::drawQuads(float *pts, int count)
{
    Renderer* r = renderer();
    r->setPrimitiveType(GL_LINE_LOOP);

    for( int i=0; i < count; i+= 2*4)
    {
        r->bindVertex(Renderer::Vertex, 2, pts);
        r->setVertexCountOffset(4, i);
        r->Render();
    }
}

void OpenGLPainter::fillTriangles(float *pts, int count)
{
    drawPrimitive(pts, count, GL_TRIANGLES);
}

void OpenGLPainter::fillTriangleFan(const float *pts, int count)
{
    drawPrimitive(pts, count, GL_TRIANGLE_FAN);
}

void OpenGLPainter::fillQuad(float x, float y, float w, float h)
{
    float vertices[] = {
        x, y,
        x+w, y,
        x+w, y+h,
        x, y+h
    };
    Renderer* r = renderer();
    r->bindVertex(Renderer::Vertex, 2, vertices);
    r->setVertexCountOffset( indicesCount(vertices,2));
    r->setPrimitiveType(GL_TRIANGLE_FAN);
    r->Render();
}

void OpenGLPainter::fillQuads(const float *pts, int count)
{
    Renderer* r = renderer();
    r->setPrimitiveType(GL_TRIANGLE_FAN);
    r->bindVertex(Renderer::Vertex, 2, pts);

    for(int i=0; i < count; i += 4)
    {
        r->setVertexCountOffset( 4, i);
        r->Render();
    }
}

void OpenGLPainter::endPrimitive()
{
    renderer()->unBindBuffers();
    OpenGLShaderProgram::useDefault();
}

void OpenGLPainter::drawPrimitive(const float *pts, int count, int primType)
{
    Renderer* r = renderer();
    r->bindVertex(Renderer::Vertex, 2, pts);
    r->setVertexCountOffset( count);
    r->setPrimitiveType(primType);
    r->Render();
}

