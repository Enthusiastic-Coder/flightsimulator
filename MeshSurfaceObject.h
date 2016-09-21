#pragma once

#include <SerializableStdStream.h>


#include "MeshObject.h"

class OpenGLTexture2D;
class Renderer;

class MeshSurfaceObject : public ISerializable
{
public:

	enum ColorFlagsEnum
	{
		ColorFlagsEnum_None = 0x0,
		ColorFlagsEnum_Shininess = 0x1,
		ColorFlagsEnum_Diffuse = 0x2,
		ColorFlagsEnum_Specular = 0x4,
		ColorFlagsEnum_Texture = 0x8,
    };

    MeshSurfaceObject();

	SERIALIZE_WRITE_BEGIN(1, 0)
		SERIALIZE_WRITE_ENTRY((int&)_ColorFlags)
		SERIALIZE_WRITE_ENTRY(_shininess)
		SERIALIZE_WRITE_ENTRY(_diffuseColor)
		SERIALIZE_WRITE_ENTRY(_specularColor)
        SERIALIZE_WRITE_ENTRY(_textureIdx)
        SERIALIZE_WRITE_RAW_VECTOR(_meshSubObjects)
		SERIALIZE_WRITE_END()

		SERIALIZE_READ_BEGIN(1, 0)
		SERIALIZE_READ_ENTRY((int&)_ColorFlags)
		SERIALIZE_READ_ENTRY(_shininess)
		SERIALIZE_READ_ENTRY(_diffuseColor)
		SERIALIZE_READ_ENTRY(_specularColor)
        SERIALIZE_READ_ENTRY(_textureIdx)
        SERIALIZE_READ_RAW_VECTOR(_meshSubObjects)
		SERIALIZE_READ_END()

    void Render(Renderer* r, std::vector<OpenGLTexture2D *> &textureObjects);
    MeshObject* addMesh();

    void setDiffuse(Vector3F diffuseColor);
    void setSpecular(Vector3F specularColor);
    void setShininess(float shininess);
    void setTextureIdx(int id);

    bool hasDiffuse() const;
    bool hasSpecular() const;
    bool hasShininess() const;
    bool hasTexture() const;

    Vector3F getDiffuse() const;
    Vector3F getSpecular() const;
    float getShininess() const;
    int getTextureIdx() const;

    void calcBoundingBox(MeshModelArgs *args, const float* VertexData, const unsigned short *Indices);
    void calcMomentOfInertia(MassChannel &mc, const float *VertexData, const unsigned short *Indices);

private:
    ColorFlagsEnum _ColorFlags = ColorFlagsEnum_None;
    int _textureIdx = 0;
    OpenGLTexture2D* _texture = 0;
    float _shininess = 0.0f;
    Vector3F _diffuseColor;
    Vector3F _specularColor;
    std::vector<MeshObject*> _meshSubObjects;
};
