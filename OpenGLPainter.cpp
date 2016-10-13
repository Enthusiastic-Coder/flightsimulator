#include "OpenGLPainter.h"
#include "OpenGLFontRenderer2D.h"
#include "OpenGLShaderProgram.h"
#include "OpenGLFontTexture.h"
#include "OpenGLRenderer.h"

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
    _fontRenderer->setFontColor(color);
    _fontRenderer->beginRender();
}

OpenGLFontRenderer2D *OpenGLPainter::fontRenderer()
{
    return _fontRenderer;
}

void OpenGLPainter::endFont()
{
    _fontRenderer->endRender();
}

void OpenGLPainter::beginPrimitive()
{
    _fontRenderer->renderer()->useProgram(*_primitiveShader);
}

void OpenGLPainter::endPrimitive()
{
    OpenGLShaderProgram::useDefault();
}
