#include "OpenGLPainter.h"
#include "OpenGLFontRenderer2D.h"
#include "OpenGLShaderProgram.h"
#include "OpenGLFontTexture.h"
#include "OpenGLRenderer.h"

void OpenGLPainter::selectFontRenderer(OpenGLFontRenderer2D *f)
{
    _fontRenderer = f;
    _fontShader = _fontRenderer->shader();
}

void OpenGLPainter::selectPrimitiveShader(OpenGLShaderProgram *shader)
{
    _primitiveShader = shader;
}

void OpenGLPainter::beginFont(OpenGLFontTexture *font, Vector4F color)
{
    _fontRenderer->selectShader(_fontShader);
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
    OpenGLShaderProgram::useDefault();
}

void OpenGLPainter::beginPrimitive()
{
    _fontRenderer->renderer()->useProgram(*_primitiveShader);
}

void OpenGLPainter::endPrimitive()
{
    OpenGLShaderProgram::useDefault();
}
