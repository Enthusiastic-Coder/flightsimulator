#pragma once

#include <SerializableStdStream.h>
#include "HeightTerrainData.h"
#include "TerrainMeshModel.h"
#include "MeshModel.h"
#include <jibbs/matrix/MatrixArrayCoords.h>

class Renderer;

class MipMapTerrainMeshModel :
    public std::vector<TerrainMeshModel*>,
	public ISceneryEnvironment,
	public ISerializable
{
public:

	SERIALIZE_WRITE_BEGIN(1, 0)
		SERIALIZE_WRITE_OBJECT(_GroundTexture)
        SERIALIZE_WRITE_ENTRY(_chunkDims)
		SERIALIZE_WRITE_OBJECT(_scaledHeightData)
		SERIALIZE_WRITE_ENTRY(_chunkCount)
		SERIALIZE_WRITE_ENTRY(_iMaxLevel)
		SERIALIZE_WRITE_THIS_PTR_VECTOR()
		SERIALIZE_WRITE_END()

		SERIALIZE_READ_BEGIN(1, 0)
		SERIALIZE_READ_OBJECT(_GroundTexture)
        SERIALIZE_READ_ENTRY(_chunkDims)
		SERIALIZE_READ_OBJECT(_scaledHeightData)
		SERIALIZE_READ_ENTRY(_chunkCount)
		SERIALIZE_READ_ENTRY(_iMaxLevel)
        SERIALIZE_READ_THIS_RAW_VECTOR()
		_scaledHeightData.Build();
	SERIALIZE_READ_END()

	MipMapTerrainMeshModel();

	bool Load(std::string strTitleName);
	void Build();
	void BuildVertexBuffers();

    void Render(Renderer *r, bool bLand, const GPSLocation& cameraPos);
    void RenderChildren(Renderer* r, unsigned int shadowMapCount) const;
	GPSLocation GetLocation() const;
	bool getHeightFromPosition(const GPSLocation& gpsLocation, HeightData& heightData) const override;

	void attachModel(MeshModel & model, GPSLocation location, float fDisplacement = 0.0f);

private:
	OpenGLTexture2D _GroundTexture;
	ScaledHeightTerrainData _scaledHeightData;
	MatrixArrayCoords _chunkDims;
	size_t _chunkCount;
	int _iMaxLevel;

	struct ChildMeshModel
	{
		ChildMeshModel(GPSLocation l, MeshModel* m)
			:location(l), meshModel(m)
		{}
		GPSLocation location;
		MeshModel* meshModel;
	};

	std::vector<ChildMeshModel> _childrenModels;
};
