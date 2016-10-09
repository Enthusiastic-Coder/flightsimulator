#include "SkyDome.h"
#include "OpenGLRenderer.h"


SkyDome::SkyDome(float radius, int sides, int slices, bool bFullSphere)
{
    _prop.radius = radius;
    _prop.sides = sides;
    _prop.slices = slices;
    _prop.hemisphereCount = bFullSphere ? 2 : 1;

    float fPolyAng = 2.0f * M_PI / sides;
    Vector3F v;

    for( int y=0; y <= _prop.hemisphereCount*slices; ++y)
    {
        float fAng = (y-slices) * M_PI / 2 / slices;

        for( int x =0; x <= sides; ++x)
        {
            v.x = cos(x*fPolyAng) * cos(fAng);
            v.y = sin(fAng);
            v.z = sin(x* fPolyAng) * cos(fAng);

            _meshData.addVertex(v * radius);
            _meshData.addTexture(float(x)/sides, float(y)/slices/_prop.hemisphereCount);
            _meshData.addIndex(y * (sides+1) + x);
            _meshData.addIndex((y+1) * (sides+1) + x);
        }
    }
}

void SkyDome::Render(Renderer *r)
{
    r->setUseIndex(true);
    r->bindVertex(Renderer::Vertex, 3, _meshData.vertexPtr());
    r->bindVertex(Renderer::TexCoord, 2, _meshData.texturePtr());
    r->bindIndex(_meshData.indexPtr());

    for( int x=0; x < _prop.hemisphereCount*_prop.slices; ++x)
    {
        r->setVertexCountOffset( (_prop.sides +1)* 2, x * (_prop.sides+1)*2);
        r->setPrimitiveType(GL_TRIANGLE_STRIP);
        r->Render();
    }

    r->unBindBuffers();
}
