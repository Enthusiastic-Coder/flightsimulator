#include "OpenGLFontRenderer2D.h"
#include "OpenGLFontTexture.h"
#include <OpenGLPipeline.h>
#include "OpenGLRenderer.h"

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

void OpenGLFontRenderer2D::onSize(int cx, int cy)
{
    _screenSize.width = cx;
    _screenSize.height = cy;
}

void OpenGLFontRenderer2D::setFontColor(const Vector4F &color)
{
    _fontColor = color;
}

void OpenGLFontRenderer2D::beginRender()
{
    if( _renderer == 0)
        return;

    OpenGLPipeline& pipeline = OpenGLPipeline::Get(_renderer->camID);

    pipeline.Push();
    pipeline.GetProjection().LoadIdentity();
    pipeline.GetProjection().SetOrthographic(0, _screenSize.width, 0, _screenSize.height, -1, 1);
    pipeline.GetModel().LoadIdentity();
    pipeline.GetView().LoadIdentity();

    pipeline.bindMatrices(_renderer->progId());
    _renderer->progId().sendUniform("textColor", _fontColor);
    _fontMesh.beginRender(_renderer);
}

void OpenGLFontRenderer2D::renderText( int x, int y, std::string str)
{
    if( _renderer ==0)
        return;

    _fontMesh.clear();
    _fontMesh.add( x, _screenSize.height - y, str);
    _fontMesh.render(_renderer);
}

void OpenGLFontRenderer2D::endRender()
{
    if( _renderer ==0)
        return;

    OpenGLPipeline& pipeline = OpenGLPipeline::Get(_renderer->camID);

    _fontMesh.endRender(_renderer);
    pipeline.Pop();
}
