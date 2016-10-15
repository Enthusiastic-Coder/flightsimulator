#include "OpenGLFontMeshBuilder.h"
#include "OpenGLFontTexture.h"
#include "OpenGLRenderer.h"

OpenGLFontMeshBuilder::OpenGLFontMeshBuilder() :
    _fontTexture(0)
{
}

void OpenGLFontMeshBuilder::clear()
{
    _meshData.clear();
}

void OpenGLFontMeshBuilder::add(int x, int y, std::string str, bool bHorizontal)
{

    for( size_t i =0; i < str.length(); ++i)
    {
        MathSupport<int>::size sz = _fontTexture->getFontSize(str[i]);
        add( x, y, sz.width, sz.height, str[i]);

        if( bHorizontal)
            x += sz.width;
        else
            y += sz.height;
    }
}

void OpenGLFontMeshBuilder::add(int x, int y, int dx, int dy, char ch)
{
    if( _fontTexture ==0)
        return;

    std::pair<float,float> uvs[4];
    _fontTexture->getUV( ch, uvs);

    //add **TWO** triangles to make QUAD

    //Triangle A
    _meshData.addVertex(Vector3F(x,-y,0));
    _meshData.addTexture(uvs[0].first, uvs[0].second);

    _meshData.addVertex(Vector3F(x+dx,-y,0));
    _meshData.addTexture(uvs[1].first, uvs[1].second);

    _meshData.addVertex(Vector3F(x,-y+dy,0));
    _meshData.addTexture(uvs[3].first, uvs[3].second);

    //Triangle B

    _meshData.addVertex(Vector3F(x+dx,-y,0));
    _meshData.addTexture(uvs[1].first, uvs[1].second);

    _meshData.addVertex(Vector3F(x+dx,-y+dy,0));
    _meshData.addTexture(uvs[2].first, uvs[2].second);

    _meshData.addVertex(Vector3F(x,-y+dy,0));
    _meshData.addTexture(uvs[3].first, uvs[3].second);
}

void OpenGLFontMeshBuilder::selectFont(OpenGLFontTexture *texture)
{
    _fontTexture = texture;
}

void OpenGLFontMeshBuilder::beginRender(Renderer *r)
{
    r->setPrimitiveType(GL_TRIANGLES);
    r->setUseIndex(false);
    if( _fontTexture != 0)  r->bindTexture(&_fontTexture->texture());
}

void OpenGLFontMeshBuilder::render(Renderer *r)
{
    r->setVertexCountOffset( _meshData.vertexSize());
    r->bindVertex(Renderer::Vertex, 3, _meshData.vertexPtr());
    r->bindVertex(Renderer::TexCoord, 2, _meshData.texturePtr());
    r->Render();
    r->unBindBuffers();
}

void OpenGLFontMeshBuilder::endRender(Renderer *r)
{
    r->unBindTexture();
}

OpenGLFontTexture *OpenGLFontMeshBuilder::getFontTexture()
{
    return _fontTexture;
}
