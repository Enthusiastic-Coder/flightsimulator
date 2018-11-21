#include "stdafx.h"
#include "OpenGLPainter.h"
#include "OpenGLFontRenderer.h"
#include "OpenGLShaderProgram.h"
#include "OpenGLFontTexture.h"
#include "OpenGLRenderer.h"
#include "OpenGLPipeline.h"
#define indicesCount(arr,n)     (sizeof(arr)/sizeof(arr[0])/n)

OpenGLPainter::OpenGLPainter(Renderer * r) :_r(r)
{
}

Renderer *OpenGLPainter::renderer()
{
	return _r;
}

void OpenGLPainter::setFont(OpenGLFontRenderer *f)
{
    _font = f;
}

void OpenGLPainter::selectPrimitiveShader(OpenGLShaderProgram *shader)
{
    _primitiveShader = shader;
}

void OpenGLPainter::beginFont(OpenGLFontTexture *font)
{
    _font->selectFont(font);
    _font->beginRender();
}

void OpenGLPainter::setColor(Vector4F color)
{
    _font->setColor(color);
}

void OpenGLPainter::renderText(int x, int y, std::string text)
{
    _font->renderText( x, y, text);
}

void OpenGLPainter::renderText(int x, int y, char ch)
{
    _font->renderText( x, y, ch);
}

void OpenGLPainter::endFont()
{
    _font->endRender();
}

void OpenGLPainter::beginPrimitive()
{
    Renderer* r = renderer();
    r->useProgram(*_primitiveShader);
    OpenGLPipeline::Get(r->camID).bindMatrices(r->progId());
    r->setUseIndex(false);
}

void OpenGLPainter::setPrimitiveColor(Vector4F color)
{
    _primitiveColor = color;
    renderer()->progId().sendUniform("primitiveColor", _primitiveColor);
}

void OpenGLPainter::drawPoint(float x, float y, float z)
{
    float vertices[] = {
        x,y, z
    };

    drawPoints(vertices, 3);
}

void OpenGLPainter::drawPoints(float *pts, int count)
{
    drawPrimitive(pts, count, GL_POINTS);
}

void OpenGLPainter::drawLine(float x1, float y1, float x2, float y2)
{
    float vertices[] = {
        x1, y1, 0,
        x2, y2, 0,
    };

    Renderer* r = renderer();
    r->bindVertex(Renderer::Vertex, 3, vertices);
    r->setVertexCountOffset( indicesCount(vertices,3));
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

void OpenGLPainter::drawLineStrip(const float *pts, int count)
{
    drawPrimitive(pts, count, GL_LINE_STRIP);
}

void OpenGLPainter::drawTriangle(float x1, float y1, float x2, float y2, float x3, float y3)
{
    float vertices[] = {
        x1, y1, 0,
        x2, y2, 0,
        x3, y3, 0,
    };
    Renderer* r = renderer();
    r->bindVertex(Renderer::Vertex, 3, vertices);
    r->setVertexCountOffset( indicesCount(vertices, 3));
    r->setPrimitiveType(GL_LINE_LOOP);
    r->Render();
}

void OpenGLPainter::drawTriangles(float *pts, int count)
{
    Renderer* r = renderer();
    r->setPrimitiveType(GL_LINE_LOOP);
    r->bindVertex(Renderer::Vertex, 3, pts);

    for( int i=0; i < count; i+= 3)
    {
        r->setVertexCountOffset(3, i);
        r->Render();
    }
}

void OpenGLPainter::drawTriangleFan(const float *pts, int count)
{
    drawPrimitive(pts, count, GL_TRIANGLE_FAN);
}

void OpenGLPainter::drawRect(float x, float y, float w, float h)
{ 
    drawQuad(x, y, x+w, y, x+w, y+h, x, y+h);
}

void OpenGLPainter::drawQuad(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4)
{
    float vertices[] = {
        x1, y1, 0,
        x2, y2, 0,
        x3, y3, 0,
        x4, y4, 0,
    };

    Renderer* r = renderer();
    r->bindVertex(Renderer::Vertex, 3, vertices);
    r->setVertexCountOffset( indicesCount(vertices,3));
    r->setPrimitiveType(GL_LINE_LOOP);
    r->Render();
}

void OpenGLPainter::drawQuads(const float *pts, int count)
{
    Renderer* r = renderer();
    r->setPrimitiveType(GL_LINE_LOOP);
    r->bindVertex(Renderer::Vertex, 3, pts);

    for( int i=0; i < count; i+= 4)
    {
        r->setVertexCountOffset(4, i);
        r->Render();
    }
}

void OpenGLPainter::fillTriangle(float x1, float y1, float x2, float y2, float x3, float y3)
{
    float vertices[] = {
        x1, y1, 0,
        x2, y2, 0,
        x3, y3, 0,
    };
    Renderer* r = renderer();
    r->bindVertex(Renderer::Vertex, 3, vertices);
    r->setVertexCountOffset( indicesCount(vertices,3));
    r->setPrimitiveType(GL_TRIANGLES);
    r->Render();
}

void OpenGLPainter::fillTriangles(float *pts, int count)
{
    drawPrimitive(pts, count, GL_TRIANGLES);
}

void OpenGLPainter::fillTriangleFan(const float *pts, int count)
{
    drawPrimitive(pts, count, GL_TRIANGLE_FAN);
}

void OpenGLPainter::fillRect(float x, float y, float w, float h)
{
    fillQuad(x, y, x+w, y, x+w, y+h, x, y+h);
}

void OpenGLPainter::fillQuad(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4)
{
    float vertices[] = {
        x1, y1, 0,
        x2, y2, 0,
        x3, y3, 0,
        x4, y4, 0
    };
    Renderer* r = renderer();
    r->bindVertex(Renderer::Vertex, 3, vertices);
    r->setVertexCountOffset( indicesCount(vertices,3));
    r->setPrimitiveType(GL_TRIANGLE_FAN);
    r->Render();
}

void OpenGLPainter::fillQuads(const float *pts, int count)
{
    Renderer* r = renderer();
    r->setPrimitiveType(GL_TRIANGLE_FAN);
    r->bindVertex(Renderer::Vertex, 3, pts);

    for(int i=0; i < count; i += 4)
    {
        r->setVertexCountOffset( 4, i);
        r->Render();
    }
}

void OpenGLPainter::drawElipse(float x, float y, float cx, float cy, float steps)
{
    std::vector<Vector3F> pts;
    pts.resize((360.0f+steps)/steps);
    for( size_t i=0; i < pts.size(); ++i)
    {
        pts[i].x = x + cx * sin(steps * i / 180.0 * M_PI);
        pts[i].y = y + cy * cos(steps * i / 180.0 * M_PI);
    }

    Renderer*r = renderer();
    r->setPrimitiveType(GL_LINE_LOOP);
    r->bindVertex(Renderer::Vertex, 3, &pts[0].x);
    r->setVertexCountOffset(pts.size());
    r->Render();
}

void OpenGLPainter::fillElipse(float x, float y, float cx, float cy, float steps)
{
    std::vector<Vector3F> pts;
    pts.resize((360.0f+steps)/steps+1);
    pts[0] = {x, y, 0};
    for( size_t i=1; i < pts.size(); ++i)
    {
        pts[i].x = x + cx * sin(steps * (i-1) / 180.0f * M_PI);
        pts[i].y = y + cy * cos(steps * (i-1) / 180.0f * M_PI);
    }

    Renderer*r = renderer();
    r->setPrimitiveType(GL_TRIANGLE_FAN);
    r->bindVertex(Renderer::Vertex, 3, &pts[0].x);
    r->setVertexCountOffset(pts.size());
    r->Render();
}

void OpenGLPainter::endPrimitive()
{
    renderer()->unBindBuffers();
    OpenGLShaderProgram::useDefault();
}

void OpenGLPainter::drawPrimitive(const float *pts, int count, int primType)
{
    Renderer* r = renderer();
    r->bindVertex(Renderer::Vertex, 3, pts);
    r->setVertexCountOffset( count);
    r->setPrimitiveType(primType);
    r->Render();
}

