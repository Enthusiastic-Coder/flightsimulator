#include "stdafx.h"
#include "OpenGLTextureRenderer2D.h"
#include <OpenglTexture2d.h>
#include <OpenGLRenderer.h>
#include <OpenGLPipeline.h>


OpenGLTextureRenderer2D::OpenGLTextureRenderer2D(Renderer *r):
    _renderer(r)
{
}

void OpenGLTextureRenderer2D::setColorModulator(Vector4F colorModulator)
{
    _colorModulator = colorModulator;
}

void OpenGLTextureRenderer2D::setScreenDims(int x, int y, int width, int height)
{
    _X = x;
    _Y = y;
    _cx = width;
    _cy = height;
}

int OpenGLTextureRenderer2D::width() const
{
    return _cx;
}

int OpenGLTextureRenderer2D::height() const
{
    return _cy;
}

void OpenGLTextureRenderer2D::beginRender()
{
    OpenGLPipeline& pipeline = OpenGLPipeline::Get( _renderer->camID);
    pipeline.Push();
    pipeline.GetView().LoadIdentity();
    pipeline.GetProjection().LoadIdentity();
    pipeline.GetProjection().SetOrthographic(_X, _X+ _cx, _Y+ _cy, _Y, -1, 1);

    glDisable(GL_DEPTH_TEST);
}

void OpenGLTextureRenderer2D::render(int x, int y, int dx, int dy)
{
    OpenGLPipeline& pipeline = OpenGLPipeline::Get( _renderer->camID);
    pipeline.GetModel().Push();
    pipeline.GetModel().LoadIdentity();
    pipeline.GetModel().Translate(x, y, 0);

    pipeline.bindMatrices(_renderer->progId());
    _renderer->progId().sendUniform("colorModulator", _colorModulator);

    float vertices[] = {
        0.0f, 0.0f, 0.0f,
        dx, 0.0f ,0.0f,
        dx, dy, 0.0f,
        0.0f, dy, 0.0f
    };

    float texcoord[] = {
        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 0.0f
    };

    _renderer->setUseIndex(false);
    _renderer->bindVertex(Renderer::Vertex, 3, vertices);
    _renderer->bindVertex(Renderer::TexCoord, 2, texcoord);

    _renderer->setVertexCountOffset(indicesCount(vertices,3));
    _renderer->setPrimitiveType(GL_TRIANGLE_FAN);

    _renderer->Render();
    _renderer->unBindBuffers();
    pipeline.GetModel().Pop();
}

void OpenGLTextureRenderer2D::renderLineBorder(int x, int y, int dx, int dy)
{
    OpenGLPipeline& pipeline = OpenGLPipeline::Get( _renderer->camID);
    pipeline.GetModel().Push();
    pipeline.GetModel().LoadIdentity();
    pipeline.GetModel().Translate(x, y, 0);

    pipeline.bindMatrices(_renderer->progId());

    float vertices[] = {
        0.0f, 0.0f, 0.0f,
        dx, 0.0f ,0.0f,
        dx, dy, 0.0f,
        0.0f, dy, 0.0f
    };

    float colors[] = {
        0, 0, 0, 1,
        0, 0, 0, 1,
        0, 0, 0, 1,
        0, 0, 0, 1
    };

    _renderer->bindVertex(Renderer::Vertex, 3, vertices);
    _renderer->bindVertex(Renderer::Color, 4, colors);

    _renderer->setVertexCountOffset(indicesCount(vertices,3));
    _renderer->setPrimitiveType(GL_LINE_LOOP);

    _renderer->Render();
    _renderer->unBindBuffers();
    pipeline.GetModel().Pop();

}

void OpenGLTextureRenderer2D::endRender()
{
    if(_renderer ==0)
        return;

    OpenGLPipeline& pipeline = OpenGLPipeline::Get( _renderer->camID);
    pipeline.Pop();
}

