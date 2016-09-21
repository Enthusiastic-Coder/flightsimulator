#include "stdafx.h"
#include "MeshSurfaceObject.h"
#include "MeshObject.h"
#include "OpenGLRenderer.h"
#include "OpenGLTexture2D.h"
#include "OpenGLShaderProgram.h"
#include "MeshModelArgs.h"

MeshSurfaceObject::MeshSurfaceObject()
{}

MeshObject* MeshSurfaceObject::addMesh()
{
    _meshSubObjects.push_back(new MeshObject);
    return _meshSubObjects.back();
}

bool MeshSurfaceObject::hasDiffuse() const
{
    return _ColorFlags & ColorFlagsEnum_Diffuse;
}

bool MeshSurfaceObject::hasSpecular() const
{
    return _ColorFlags & ColorFlagsEnum_Specular;
}

bool MeshSurfaceObject::hasShininess() const
{
    return _ColorFlags & ColorFlagsEnum_Shininess;
}

bool MeshSurfaceObject::hasTexture() const
{
    return _ColorFlags & ColorFlagsEnum_Texture;
}

int MeshSurfaceObject::getTextureIdx() const
{
    return _textureIdx;
}

Vector3F MeshSurfaceObject::getDiffuse() const
{
    return _diffuseColor;
}

Vector3F MeshSurfaceObject::getSpecular() const
{
    return _specularColor;
}

float MeshSurfaceObject::getShininess() const
{
    return _shininess;
}

void MeshSurfaceObject::setShininess(float shininess)
{
    (int&)_ColorFlags |= ColorFlagsEnum_Shininess;
    _shininess = shininess;
}

void MeshSurfaceObject::setDiffuse(Vector3F diffuseColor)
{
    (int&)_ColorFlags |= ColorFlagsEnum_Diffuse;
    _diffuseColor = diffuseColor;
}

void MeshSurfaceObject::setSpecular(Vector3F specularColor)
{
    (int&)_ColorFlags |= ColorFlagsEnum_Specular;
    _specularColor = specularColor;
}

void MeshSurfaceObject::setTextureIdx(int id)
{
    (int&)_ColorFlags |= ColorFlagsEnum_Texture;
    _textureIdx = id;
}

void MeshSurfaceObject::calcBoundingBox(MeshModelArgs* args, const float* VertexData, const unsigned short* Indices)
{
    for (MeshObject* it : _meshSubObjects)
        it->calcBoundingBox(args, VertexData, Indices);
}

void MeshSurfaceObject::calcMomentOfInertia(MassChannel & mc, const float * VertexData, const unsigned short *Indices)
{
    for (MeshObject* it : _meshSubObjects)
        it->calcMomentOfInertia(mc, VertexData, Indices);
}

void MeshSurfaceObject::Render(Renderer* r, std::vector<OpenGLTexture2D*>& textureObjects)
{
    r->bindMaterial(_diffuseColor, _specularColor, _shininess);

    if (_ColorFlags & ColorFlagsEnum_Texture)
    {
        if( _texture == 0)
            _texture = textureObjects[getTextureIdx()];

        r->bindTexture(_texture);
        r->progId().sendUniform("useTex", _texture->getId() == 0 ?0:1);
     }

    for (MeshObject* it : _meshSubObjects)
        it->Render(r);

    if (_ColorFlags & ColorFlagsEnum_Texture)
    {
        r->progId().sendUniform("useTex", 0);
        r->unBindTexture();
    }
}
