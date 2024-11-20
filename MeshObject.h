#pragma once

#include "Serializable.h"
#include <jibbs/math/MassChannel.h>
class MeshModelArgs;
class Renderer;

class MeshObject : public ISerializable
{
public:
    MeshObject();

	SERIALIZE_WRITE_BEGIN(1, 0)
		SERIALIZE_WRITE_ENTRY(_primitiveType)
		SERIALIZE_WRITE_ENTRY_PAIR(_countOffset)
    SERIALIZE_WRITE_END()

    SERIALIZE_READ_BEGIN(1, 0)
		SERIALIZE_READ_ENTRY(_primitiveType)
		SERIALIZE_READ_ENTRY_PAIR(_countOffset)
    SERIALIZE_READ_END()

    void setPrimitveType(int type);
    void setCountOffset(unsigned int count, unsigned int offSet);

    void Render(Renderer *r);

    void calcBoundingBoxPtEnum(void*, size_t i, Vector3F pts[3]);
    void calcBoundingBox(MeshModelArgs *args, const float *VertexData, const unsigned short *Indices);
    void calcMomentOfInertiaPtEnum(void* pVoid, size_t i, Vector3F pts[3]);
    void calcMomentOfInertia(MassChannel &mc, const float *VertexData, const unsigned short *Indices);

protected:
    void enumeratePoints(const float *VertexData, const unsigned short *Indices, void (MeshObject::*pCallback)(void* pVoid, size_t, Vector3F pts[3]), void* pVoid);

private:
	int _primitiveType;
    std::pair<int, int> _countOffset; //count, offset
};

