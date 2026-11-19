#include "stdafx.h"

#include <jibbs/gps/GPSModelTransform.h>

#include "MipMapTerrainMeshModel.h"
#include "TileNode.h"
#include "GPSTerrainData.h"
#include "MeshModel.h"
#include "TerrainMeshModel.h"
#include "SRTM30HeightTerrainData.h"
#include "OpenGLRenderer.h"
#include "OpenGLPipeline.h"
#include "Paths.h"

#define CHUNK_DIMENSIONS		(16)


//#define TERRAIN_DIMENSIONS		(128)
//#define TERRAIN_MIPMAP_LEVEL	(2)

#if defined LOCATED_AT_GIBRALTER
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

void MipMapTerrainMeshModel::setGroundDetail(const std::string& filename, const GPSLocation& uvOrigin,
                                            const GPSLocation& uEdge, const GPSLocation& vEdge)
{
    const GPSLocation tileUV = uvOrigin - GetLocation();
    const GPSLocation uAxis = uEdge - uvOrigin;
    const GPSLocation vAxis = vEdge - uvOrigin;
    const double determinant = uAxis._lng * vAxis._lat - uAxis._lat * vAxis._lng;
    if (std::abs(determinant) < 1.0e-10)
        return;

    // Terrain UVs are longitude and latitude offsets within the one-degree tile.
    _groundDetailOrigin = Vector2F(tileUV._lng, tileUV._lat);
    _groundDetailU = Vector2F(vAxis._lat / determinant, -vAxis._lng / determinant);
    _groundDetailV = Vector2F(-uAxis._lat / determinant, uAxis._lng / determinant);
    _groundDetail.setMinification(GL_LINEAR_MIPMAP_LINEAR);
    _groundDetail.setWrapMode(GL_CLAMP_TO_EDGE);
    _groundDetail.Load(filename);
}

void MipMapTerrainMeshModel::Render(Renderer* r, bool bLand, const GPSLocation& cameraPos)
{
    constexpr int groundDetailUnit = 1;
    auto& shader = r->progId();
    const bool renderGroundDetail = bLand && _groundDetail.getId() != 0 &&
        shader.getUniformLocation("groundDetailEnabled") >= 0;
    if (renderGroundDetail)
    {
        glActiveTexture(GL_TEXTURE0 + groundDetailUnit);
        _groundDetail.bind();
        glActiveTexture(GL_TEXTURE0);
        shader.sendUniform("groundDetailEnabled", 1);
        shader.sendUniform("groundDetailTexture", groundDetailUnit);
        shader.sendUniform("groundDetailOrigin", _groundDetailOrigin.x, _groundDetailOrigin.z);
        shader.sendUniform("groundDetailU", _groundDetailU.x, _groundDetailU.z);
        shader.sendUniform("groundDetailV", _groundDetailV.x, _groundDetailV.z);
    }

    if (bLand)
        _GroundTexture.bind();

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

    if (renderGroundDetail)
    {
        shader.sendUniform("groundDetailEnabled", 0);
        glActiveTexture(GL_TEXTURE0 + groundDetailUnit);
        OpenGLTexture2D::unBind();
        glActiveTexture(GL_TEXTURE0);
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
