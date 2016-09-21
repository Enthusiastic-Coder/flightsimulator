#pragma once

#include "Serializable.h"
#include "interfaces.h"
#include "MeshSurfaceObject.h"
#include "OpenGLVertexBuffer.h"
#include <MassChannel.h>
#include "MeshData.h"
#include "MeshModelArgs.h"

class OpenGLTexture2D;
class Renderer;

class MeshGroupObject : public ISceneryEnvironment, public ISerializable
{
public:
    MeshGroupObject(std::string sName = "##NO_NAME##", std::string sParentName="##NO_PARENT##");
    virtual ~MeshGroupObject() { }

	SERIALIZE_WRITE_BEGIN(1, 0)
		SERIALIZE_WRITE_ENTRY((int&)_BufferFlags)
        SERIALIZE_WRITE_OBJECT(_meshData)
		SERIALIZE_WRITE_ENTRY(_sName)
        SERIALIZE_WRITE_ENTRY(_sParentName)
		//SERIALIZE_WRITE_ENTRY(_PlaneData)
        SERIALIZE_WRITE_RAW_VECTOR(_meshObjects)
    SERIALIZE_WRITE_END()

    SERIALIZE_READ_BEGIN(1, 0)
		SERIALIZE_READ_ENTRY((int&)_BufferFlags)
        SERIALIZE_READ_OBJECT(_meshData)
		SERIALIZE_READ_ENTRY(_sName)
        SERIALIZE_READ_ENTRY(_sParentName)
		//SERIALIZE_READ_ENTRY(_PlaneData)
        SERIALIZE_READ_RAW_VECTOR(_meshObjects)
    SERIALIZE_READ_END()

    enum BufferFlagsEnum
	{
		BufferFlags_None = 0x0,
		BufferFlags_Vertex = 0x1,
        BufferFlags_Color = 0x2,
        BufferFlags_Normal = 0x4,
        BufferFlags_Texture = 0x8,
        BufferFlags_Index = 0x10,
        BufferFlags_Plane = 0x20,
    };

	void setMOISkipFlag(bool bValue);
	void setVertexFlag();
    void setColorFag();
	void setNormalFlag();
	void setTexCoordFlag();
	void setIndexFlag();
    void setPlaneFlag();

    bool hasVertexFlag() const;
    bool hasColorFlag() const;
    bool hasNormalFlag() const;
    bool hasTexCoordFlag() const;
    bool hasIndexFlag() const;

	std::string getName() const;
    size_t getTextureCount() const;
    bool getHeightFromPosition(const GPSLocation& gpsLocation, HeightData& heightData) const override;

    bool BuildVertexBuffers(GLenum usage, bool bClearMesh=true);

    meshData _meshData;

    void Render(Renderer *r, std::vector<OpenGLTexture2D*>& textureObjects);

    MeshSurfaceObject* addSurface();

    void calcBoundingBox(MeshModelArgs *args);
    void calcMomentOfInertia(MassChannel &mc);

private:
    BufferFlagsEnum _BufferFlags;
	OpenGLVertexBuffer _VertexBuffer;
    OpenGLVertexBuffer _ColorBuffer;
    OpenGLVertexBuffer _NormalBuffer;
    OpenGLVertexBuffer _TexCoordBuffer;
    OpenGLVertexBuffer _IndicesBuffer;

    std::string _sParentName;
	std::string _sName;
    //TrianglePlaneBoundaryArrayT _PlaneData;
    bool _bMOISkip;
    std::vector<MeshSurfaceObject*> _meshObjects;
};

