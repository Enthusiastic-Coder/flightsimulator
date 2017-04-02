#pragma once

#include <SerializableStdStream.h>
#include "MeshGroupObject.h"
#include "OpenGLTexture2D.h"
#include "MeshModelArgs.h"

class OpenGLTexture2D;

class MeshModel : public ISerializable
{
public:
	MeshModel(std::string sName = "##noname##");

    virtual ~MeshModel() {}

	SERIALIZE_WRITE_BEGIN(1, 0)
		SERIALIZE_WRITE_ENTRY(_sName)
        SERIALIZE_WRITE_ENTRY(_args._BoundingBox)
        SERIALIZE_WRITE_ENTRY(_args._polyCount)
        SERIALIZE_WRITE_ENTRY(_args._fTotalVol)
        SERIALIZE_WRITE_ENTRY(_args._fTotalArea)
        SERIALIZE_WRITE_ENTRY(_args._centroidLocation)
        SERIALIZE_WRITE_RAW_VECTOR(_objects)
		SERIALIZE_WRITE_MAP(_textureMapNames)
		SERIALIZE_WRITE_PTR_VECTOR(_textureObjects)
	SERIALIZE_WRITE_END()

	SERIALIZE_READ_BEGIN(1, 0)
		SERIALIZE_READ_ENTRY(_sName)
        SERIALIZE_READ_ENTRY(_args._BoundingBox)
        SERIALIZE_READ_ENTRY(_args._polyCount)
        SERIALIZE_READ_ENTRY(_args._fTotalVol)
        SERIALIZE_READ_ENTRY(_args._fTotalArea)
        SERIALIZE_READ_ENTRY(_args._centroidLocation)
        SERIALIZE_READ_RAW_VECTOR(_objects)
		SERIALIZE_READ_MAP(_textureMapNames)
        SERIALIZE_READ_OBJ_VECTOR(_textureObjects)
	SERIALIZE_READ_END()

	SERIALIZE_READ_CLASS(BinaryReadStream)
	SERIALIZE_WRITE_CLASS(BinaryWriteStream)

    std::string getName() const;

    void Render(Renderer *r);
    void RenderIndex(Renderer *r, size_t idx);

    MeshGroupObject* addGroup(std::string name);
    MeshGroupObject* operator[](int idx);
    size_t size() const;

    int getTextureIdx(std::string sRootDir, std::string sFilename, int minification = GL_LINEAR_MIPMAP_LINEAR);
    std::vector<OpenGLTexture2D*>& getTextureObjects();

	bool InFrustum(const Frustum& f);
    bool BuildVertexBuffers(GLenum usage = GL_STATIC_DRAW);

    void calcMomentOfInertia(MassChannel &mc);
    void calcBoundingBox();

    BoundingBox getBoundingBox() const;
    size_t getPolyCount();
    float getArea();
    float getVolume();
    Vector3F getCentroidPt();

protected:
    void setName(std::string sName);
    void addNullTexture();
    virtual MeshGroupObject* factoryGroupObject(std::string name, std::string parent);

private:
    std::string _sName;
    std::map< std::string, int> _textureMapNames;
    std::vector<MeshGroupObject*> _objects;
    std::vector<OpenGLTexture2D*> _textureObjects;
    MeshModelArgs _args;
};

