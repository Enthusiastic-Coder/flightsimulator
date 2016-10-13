#include "OpenGLPainter.h"
#include "OpenGLFontRenderer2D.h"
#include "OpenGLShaderProgram.h"

void OpenGLPainter::selectFontRenderer(OpenGLFontRenderer2D *f)
{
    fontRenderer = f;
}

void OpenGLPainter::selectPrimitiveShader(OpenGLShaderProgram *shader)
{
    _primitiveShader = shader;
}
