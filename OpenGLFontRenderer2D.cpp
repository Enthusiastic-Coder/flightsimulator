#include "stdafx.h"
#include "OpenGLFontRenderer2D.h"
#include "OpenGLFontTexture.h"
#include "OpenGLPipeline.h"
#include "OpenGLRenderer.h"
#include "OpenGLShaderProgram.h"

void OpenGLFontRenderer2D::selectShader(OpenGLShaderProgram *p)
{
    _shader = p;
}

void OpenGLFontRenderer2D::selectFont(OpenGLFontTexture *texture)
{
    _fontMesh.selectFont(texture);
}

OpenGLFontTexture *OpenGLFontRenderer2D::getFont()
{
    return  _fontMesh.getFontTexture();
}

void OpenGLFontRenderer2D::setFontColor( const Vector4F &color)
{
    _fontColor = color;
}

void OpenGLFontRenderer2D::beginRender(Renderer* r)
{
    r->useProgram(*_shader);
	r->progId().sendUniform("textColor", _fontColor);
    OpenGLPipeline::Get(r->camID).bindMatrices(r->progId());
    _fontMesh.beginRender(r);
}

void OpenGLFontRenderer2D::renderText(Renderer* r, int x, int y, std::string str)
{
    _fontMesh.clear();
    _fontMesh.add( x, y, str);
    _fontMesh.render(r);
}

void OpenGLFontRenderer2D::renderText(Renderer* r, int x, int y, char ch)
{
    std::string str = " ";
    str[0] = ch;
    _fontMesh.clear();
    _fontMesh.add( x, y, str);
    _fontMesh.render(r);

}

void OpenGLFontRenderer2D::endRender(Renderer* r)
{
    _fontMesh.endRender(r);
    OpenGLShaderProgram::useDefault();
}
