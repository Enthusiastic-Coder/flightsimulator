#include "stdafx.h"
#include <utility>
#include "vector3.h"
#include "MeshObject.h"
#include "OpenGLRenderer.h"
#include "MeshModelArgs.h"
#include <MassChannel.h>
#include <MeshHelper.h>

MeshObject::MeshObject() : _primitiveType(0) {}

void MeshObject::setPrimitveType(int type)
{
    _primitiveType = type;
}

void MeshObject::setCountOffset(unsigned int count, unsigned int offSet)
{
    _countOffset = std::make_pair(count, offSet);
}

void MeshObject::enumeratePoints(const float* VertexData, const unsigned short* Indices,void (MeshObject::*pCallback)(void *, size_t, Vector3F[]), void *pVoid)
{
    unsigned int count = _countOffset.first;
    unsigned int offset = _countOffset.second;
    int incr(1);

    if (_primitiveType != GL_TRIANGLE_STRIP && _primitiveType != GL_TRIANGLES)
        return;

    if (_primitiveType == GL_TRIANGLES)
        incr = 3;

    Vector3F pts[3];

    for (int i = offset; i < (int)(offset + count - 2); i += incr)
    {
        if (Indices)
        {
            pts[0] = Vector3F(VertexData[Indices[i] * incr],
                    VertexData[Indices[i] * incr + 1],
                    VertexData[Indices[i] * incr + 2]);

            pts[1] = Vector3F(VertexData[Indices[i + 1] * incr],
                    VertexData[Indices[i + 1] * incr + 1],
                    VertexData[Indices[i + 1] * incr + 2]);

            pts[2] = Vector3F(VertexData[Indices[i + 2] * incr],
                    VertexData[Indices[i + 2] * incr + 1],
                    VertexData[Indices[i + 2] * incr + 2]);
        }
        else
        {
            pts[0] = Vector3F(VertexData[i * incr],
                    VertexData[i * incr + 1],
                    VertexData[i * incr + 2]);

            pts[1] = Vector3F(VertexData[(i + 1) * incr],
                    VertexData[(i + 1) * incr + 1],
                    VertexData[(i + 1) * incr + 2]);

            pts[2] = Vector3F(VertexData[(i + 2) * incr],
                    VertexData[(i + 2) * incr + 1],
                    VertexData[(i + 2) * incr + 2]);
        }

        (this->*pCallback)(pVoid, i, pts);
    }
}

void MeshObject::calcBoundingBoxPtEnum(void *pVoid, size_t i, Vector3F pts[3])
{
    float fVol(0.0f);

    if (!(i % 2) || _primitiveType == GL_TRIANGLES)
        fVol = MeshHelper::CalcVolume(pts[0], pts[1], pts[2]);
    else
        fVol = MeshHelper::CalcVolume(pts[0], pts[2], pts[1]);

    float fArea(MeshHelper::CalcArea(pts[0], pts[1], pts[2]));
    Vector3F avgPt((pts[0] + pts[1] + pts[2]) / 3.0f);

    MeshModelArgs* args = (MeshModelArgs*)pVoid;

    args->_polyCount++;
    args->_fTotalVol += fVol;
    args->_fTotalArea += fArea;
    args->_centroidLocation.x += avgPt.x;
    args->_centroidLocation.y += avgPt.y * fArea;
    args->_centroidLocation.z += avgPt.z;
}

void MeshObject::calcBoundingBox(MeshModelArgs* args, const float* VertexData, const unsigned short* Indices)
{
    enumeratePoints(VertexData, Indices, &MeshObject::calcBoundingBoxPtEnum, args);
}

void MeshObject::calcMomentOfInertiaPtEnum(void *pVoid, size_t i, Vector3F pts[3])
{
    Vector3F vVol = Vector3F(1.0f, 1.0f, 1.0f) * sqrtf(MeshHelper::CalcArea(pts[0], pts[1], pts[2]));
    reinterpret_cast<MassChannel*>(pVoid)->AddPoint((pts[0] + pts[1] + pts[2]) / 3.0f, vVol);
}

void MeshObject::calcMomentOfInertia(MassChannel &mc, const float *VertexData, const unsigned short *Indices)
{
    enumeratePoints(VertexData, Indices, &MeshObject::calcMomentOfInertiaPtEnum, reinterpret_cast<void*>(&mc));
}

void MeshObject::Render(Renderer* r)
{
    r->setPrimitiveType(_primitiveType);
    r->setVertexCountOffset(_countOffset.first, _countOffset.second );
    r->Render();
    r->clearPrimitive();
}
