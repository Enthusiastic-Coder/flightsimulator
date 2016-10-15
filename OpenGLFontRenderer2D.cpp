#include "OpenGLFontRenderer2D.h"
#include "OpenGLFontTexture.h"
#include "OpenGLPipeline.h"
#include "OpenGLRenderer.h"
#include "OpenGLShaderProgram.h"

void OpenGLFontRenderer2D::selectShader(OpenGLShaderProgram *p)
{
    _shader = p;
}

void OpenGLFontRenderer2D::selectRenderer(Renderer *r)
{
    _renderer = r;
}

void OpenGLFontRenderer2D::selectFont(OpenGLFontTexture *texture)
{
    _fontMesh.selectFont(texture);
}

OpenGLFontTexture *OpenGLFontRenderer2D::getFont()
{
    return  _fontMesh.getFontTexture();
}

void OpenGLFontRenderer2D::setFontColor(const Vector4F &color)
{
    _fontColor = color;
}

void OpenGLFontRenderer2D::beginRender()
{
    if( _renderer == 0)
        return;
    if( _shader == 0)
        return;

    _renderer->useProgram(*_shader);
    OpenGLPipeline::Get(_renderer->camID).bindMatrices(*_shader);
    _renderer->progId().sendUniform("textColor", _fontColor);
    _fontMesh.beginRender(_renderer);
}

void OpenGLFontRenderer2D::renderText( int x, int y, std::string str)
{
    if( _renderer ==0)
        return;

    _fontMesh.clear();
    _fontMesh.add( x, y, str);
    _fontMesh.render(_renderer);
}

void OpenGLFontRenderer2D::endRender()
{
    if( _renderer ==0)
        return;

    _fontMesh.endRender(_renderer);
    OpenGLShaderProgram::useDefault();
}

Renderer *OpenGLFontRenderer2D::renderer()
{
    return _renderer;
}
