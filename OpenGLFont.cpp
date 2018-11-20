#include "stdafx.h"
#include "OpenGLFont.h"
#include "OpenGLFontTexture.h"
#include "OpenGLPipeline.h"
#include "OpenGLRenderer.h"
#include "OpenGLShaderProgram.h"

void OpenGLFont::selectShader(OpenGLShaderProgram *p)
{
    _shader = p;
}

void OpenGLFont::selectFont(OpenGLFontTexture *texture)
{
    _fontMesh.selectFont(texture);
}

OpenGLFontTexture *OpenGLFont::getFont()
{
    return  _fontMesh.getFontTexture();
}

void OpenGLFont::setColor( const Vector4F &color)
{
    _color = color;
}

void OpenGLFont::beginRender(Renderer* r)
{
    r->useProgram(*_shader);
	r->progId().sendUniform("textColor", _color);
    OpenGLPipeline::Get(r->camID).bindMatrices(r->progId());
    _fontMesh.beginRender(r);
}

void OpenGLFont::renderText(Renderer* r, int x, int y, std::string str)
{
    _fontMesh.clear();
    _fontMesh.add( x, y, str);
    _fontMesh.render(r);
}

void OpenGLFont::renderText(Renderer* r, int x, int y, char ch)
{
    std::string str = " ";
    str[0] = ch;
    _fontMesh.clear();
    _fontMesh.add( x, y, str);
    _fontMesh.render(r);

}

void OpenGLFont::endRender(Renderer* r)
{
    _fontMesh.endRender(r);
    OpenGLShaderProgram::useDefault();
}
