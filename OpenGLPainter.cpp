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
    renderer()->useProgram(*_primitiveShader);
}

void OpenGLPainter::endPrimitive()
{
    OpenGLShaderProgram::useDefault();
}

Renderer *OpenGLPainter::renderer()
{
    return _fontRenderer->renderer();
}
