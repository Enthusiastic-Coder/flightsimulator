#include "stdafx.h"

#include <jibbs/gps/GPSModelTransform.h>

#include "MipMapTerrainMeshModel.h"
#include "TileNode.h"
#include "GPSTerrainData.h"
#include "MeshModel.h"
#include "HeightTerrainData.h"
#include "TerrainMeshModel.h"
#include "SRTM30HeightTerrainData.h"
#include "OpenGLRenderer.h"
#include "OpenGLShaderProgram.h"
#include "OpenGLPipeline.h"
#include "Paths.h"

#define CHUNK_DIMENSIONS		(16)


//#define TERRAIN_DIMENSIONS		(128)
//#define TERRAIN_MIPMAP_LEVEL	(2)

#if defined LOCATED_AT_GIBRALTER && !defined _DEBUG
	#define TERRAIN_DIMENSIONS		(1024)
	#define TERRAIN_MIPMAP_LEVEL	(5)
#else
	#define TERRAIN_DIMENSIONS		(256)
	#define TERRAIN_MIPMAP_LEVEL	(3)
#endif

//#define TERRAIN_DIMENSIONS		(512)
//#define TERRAIN_MIPMAP_LEVEL	(4)

//#define TERRAIN_DIMENSIONS		(1024)
//#define TERRAIN_MIPMAP_LEVEL	(5)

//#define TERRAIN_DIMENSIONS		(2048)
//#define TERRAIN_MIPMAP_LEVEL	(6)



MipMapTerrainMeshModel::MipMapTerrainMeshModel() :
	_chunkCount(0), _iMaxLevel(0) {}

void MipMapTerrainMeshModel::BuildVertexBuffers()
{
    for (TerrainMeshModel* model: *this)
        model->BuildVertexBuffers();

    for (const ChildMeshModel& it : _childrenModels)
		it.meshModel->BuildVertexBuffers();
}

bool MipMapTerrainMeshModel::Load(std::string strTitleName)
{
	SRTM30HeightTerrainData srtm30heightData;

	_GroundTexture.setMinification(GL_LINEAR_MIPMAP_LINEAR);
    _GroundTexture.setWrapMode(GL_CLAMP_TO_EDGE);

	if (!_GroundTexture.Load(OUTPUT_IMAGE_ROOT_FOLDER + strTitleName + ".png"))
		return false;

	std::transform(strTitleName.begin(), strTitleName.end(), strTitleName.begin(), ::toupper);

	if (!srtm30heightData.Load(OUTPUT_SRTM3_ROOT_FOLDER + strTitleName + ".hgt"))
		return false;

	_scaledHeightData.Load(srtm30heightData, TERRAIN_DIMENSIONS);
	return true;
}

void MipMapTerrainMeshModel::Build()
{
	_scaledHeightData.Build();

    GPSTerrainData meshObject(_scaledHeightData);
	meshObject.Build(TERRAIN_DIMENSIONS);

	TileNode tileNode(meshObject);
	tileNode.Build(CHUNK_DIMENSIONS, TERRAIN_MIPMAP_LEVEL);

	_chunkCount = tileNode.size();
	_iMaxLevel = tileNode.maxLevel();
	_chunkDims.setDims(tileNode.chunkDim());

	clear();

	//Land
	for (int i = 0; i <= _iMaxLevel; ++i)
	{
        push_back(new TerrainMeshModel);
		back()->Build(true, tileNode, i);
	}

	//Sea
    push_back(new TerrainMeshModel);
	back()->Build(false, tileNode, 0);
}

void MipMapTerrainMeshModel::Render(Renderer* r, bool bLand, const GPSLocation& cameraPos)
{
    if (bLand) _GroundTexture.bind();

	GPSLocation diffGPS = cameraPos - GetLocation();
	Vector2I centralIDX(diffGPS._lng * _chunkDims.getX(), (1 - diffGPS._lat) * _chunkDims.getY());

    OpenGLPipeline& mat = OpenGLPipeline::Get(r->camID);

    for (size_t i = 0; i < _chunkCount; ++i)
	{
		int level(_iMaxLevel + 1);

		if (bLand)
		{
			Vector2I diffIDX = Vector2I(_chunkDims.toMatrixPos(i)) - centralIDX;
            level = std::max(abs(diffIDX.x), abs(diffIDX.z));
            level = std::max(level - 1, 0);
            level = std::max(0, _iMaxLevel - level);
		}

        TerrainMeshModel* terrainMeshModel = operator[](level);

        if(terrainMeshModel->InFrustum(mat.GetFrustum()))
            terrainMeshModel->RenderIndex(r, i);
	}
}

void MipMapTerrainMeshModel::RenderChildren(Renderer *r, unsigned int shadowMapCount) const
{
    OpenGLPipeline& mat = OpenGLPipeline::Get(r->camID);

    for (const ChildMeshModel& it : _childrenModels)
	{
		mat.Push();
		mat.GetModel().TranslateLocation(it.location);
        mat.bindMatrices(r->progId());

		if (it.meshModel->InFrustum(mat.GetFrustum()))
		{
            for (unsigned int shadowID = r->camID + 1; shadowID <= r->camID + shadowMapCount; ++shadowID)
			{
				OpenGLPipeline::Get(shadowID).Push();
				OpenGLPipeline::Get(shadowID).GetModel().TranslateLocation(it.location);
                OpenGLPipeline::Get(shadowID).ApplyLightBPMV(r->progId());
			}

            it.meshModel->Render(r);

            for (unsigned int shadowID = r->camID + 1; shadowID <= r->camID + shadowMapCount; ++shadowID)
			{
				OpenGLPipeline::Get(shadowID).Pop();
                OpenGLPipeline::Get(shadowID).ApplyLightBPMV(r->progId());
			}
		}

		mat.Pop();
	}
}

GPSLocation MipMapTerrainMeshModel::GetLocation() const
{
	return _scaledHeightData.GetLocation();
}

bool MipMapTerrainMeshModel::getHeightFromPosition(const GPSLocation& gpsLocation, HeightData& heightData) const
{
	return _scaledHeightData.getHeightFromPosition(gpsLocation, heightData);
}

void MipMapTerrainMeshModel::attachModel(MeshModel & model, GPSLocation location, float fDisplacement)
{
	_scaledHeightData.Build();

	GPSModelTransform modelTransform(location);

	for (size_t i = 0; i < model.size(); i++)
	{
        for (size_t vertexIdx = 0; vertexIdx < model[i]->_meshData.vertexSize(); vertexIdx++)
		{
            Vector3F itVertex = model[i]->_meshData.getVertex(vertexIdx);

            Vector3F vN(0, 1.0f, 0);
			GPSLocation newLocation(location + itVertex);

			if (model[i]->hasNormalFlag())
			{
				vN = _scaledHeightData.GetNormal(newLocation);
                model[i]->_meshData.setNormal(vertexIdx, vN);
			}

			itVertex.y += _scaledHeightData(newLocation);
			itVertex = modelTransform.LocalFlatToLocal(itVertex + fDisplacement * vN);
            model[i]->_meshData.setVertex(vertexIdx, itVertex);
		}
	}

	model.calcBoundingBox();
	_childrenModels.push_back(ChildMeshModel(location, &model));
}
