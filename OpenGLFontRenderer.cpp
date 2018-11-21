#include "stdafx.h"
#include "OpenGLFontRenderer.h"
#include "OpenGLFontTexture.h"
#include "OpenGLPipeline.h"
#include "OpenGLRenderer.h"
#include "OpenGLShaderProgram.h"

OpenGLFontRenderer::OpenGLFontRenderer(Renderer * r) : _r(r)
{
}

void OpenGLFontRenderer::selectShader(OpenGLShaderProgram *p)
{
    _shader = p;
}

void OpenGLFontRenderer::selectFont(OpenGLFontTexture *texture)
{
    _fontMesh.setFont(texture);
}

void OpenGLFontRenderer::setColor( const Vector4F &color)
{
    _color = color;
}

void OpenGLFontRenderer::beginRender()
{
    _r->useProgram(*_shader);
	_r->progId().sendUniform("textColor", _color);
    OpenGLPipeline::Get(_r->camID).bindMatrices(_r->progId());
    _fontMesh.beginRender(_r);
}

void OpenGLFontRenderer::renderText(int x, int y, std::string str)
{
    _fontMesh.clear();
    _fontMesh.add( x, y, str);
    _fontMesh.render(_r);
}

void OpenGLFontRenderer::renderText(int x, int y, char ch)
{
    std::string str = " ";
    str[0] = ch;
    _fontMesh.clear();
    _fontMesh.add( x, y, str);
    _fontMesh.render(_r);

}

void OpenGLFontRenderer::endRender()
{
    _fontMesh.endRender(_r);
    OpenGLShaderProgram::useDefault();
}
