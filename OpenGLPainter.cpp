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
    Renderer* r = renderer();

    r->setPrimitiveType(GL_POINTS);
    r->setVertexCountOffset(1);

}

void OpenGLPainter::drawPoints(float *pts, int count)
{

}

void OpenGLPainter::drawQuad(float x, float y, float w, float h)
{
    Renderer* r = renderer();

    float vertices[] = {
        x, -y, 0,
        x+w, -y, 0,
        x+w, -y-h, 0,
        x, -y-h, 0
    };
    r->bindVertex(Renderer::Vertex, 3, vertices);
    r->setVertexCountOffset( indicesCount(vertices,3));
    r->setPrimitiveType(GL_QUADS);
    r->Render();
}

void OpenGLPainter::endPrimitive()
{
    renderer()->unBindBuffers();
    OpenGLShaderProgram::useDefault();
}

