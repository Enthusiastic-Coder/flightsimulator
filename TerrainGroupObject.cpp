#include "stdafx.h"
#include "TerrainGroupObject.h"
#include "MeshModel.h"
#include "TileNode.h"
#include "GPSTerrainData.h"
#include "MeshSurfaceObject.h"

TerrainGroupObject::TerrainGroupObject(std::string sName, std::string sParent) :
    MeshGroupObject(sName, sParent)
{
}

void TerrainGroupObject::Build(bool bLand, const TileNode& tileNode, int chunkNo, int level)
{
	setVertexFlag();
	setNormalFlag();
	setTexCoordFlag();
    setIndexFlag();

	std::pair<Vector2I, Vector2I> rect = tileNode.chunkNode(chunkNo).quad->getBoundaryIDX();
    const GPSTerrainData& gpsMeshObject = tileNode.getGPSGroupObject();
	const std::vector<Vector2I>& tileVertexIndexData = tileNode.BuildIndexData(chunkNo, level);

	std::vector<unsigned short> indexList;
	indexList.resize(tileVertexIndexData.size());

	for (size_t i = 0; i < tileVertexIndexData.size(); ++i)
		indexList[i] = gpsMeshObject.GetIndex(tileVertexIndexData[i].x, tileVertexIndexData[i].z);

    BuildTerrainData(bLand, gpsMeshObject, indexList);

    MeshSurfaceObject* surface = addSurface();
    MeshObject* mesh = surface->addMesh();
    mesh->setPrimitveType(GL_TRIANGLES);
    mesh->setCountOffset(_meshData.indexSize(), 0);

	if (bLand)
        surface->setDiffuse( Vector3F(0.8f, 0.8f, 0.8f));
	else
        surface->setDiffuse(Vector3F(0.32f, 0.39f, 0.42f));

    surface->setSpecular(Vector3F(0.8f, 0.8f, 0.8f));
    surface->setShininess( 32.0f / 2.0f);

	if (bLand && level < tileNode.maxLevel())
	{
        size_t indexCount = _meshData.indexSize();
		BuildCurtains(rect, gpsMeshObject, tileVertexIndexData);
        MeshObject* mesh = surface->addMesh();
        mesh->setPrimitveType(GL_TRIANGLES);
        mesh->setCountOffset(_meshData.indexSize() - indexCount, indexCount);
	}
}

void TerrainGroupObject::BuildTerrainData(bool bLand, const GPSTerrainData& terrainData, std::vector<unsigned short>& indexList)
{
	std::map<unsigned short, unsigned short> indexMappingList;

    for (const GLuint& it : indexList)
	{
        if (indexMappingList.find(it) != indexMappingList.end())
        {
            _meshData.addIndex(indexMappingList[it]);
            continue;
        }

        //Not visited yet
        if (bLand)
        {
            _meshData.addVertex(terrainData._VertexData[it]);
            _meshData.addNormal(terrainData._NormalData[it]);
        }
        else
        {
            _meshData.addVertex(terrainData.GetGroundVertex(it));
            _meshData.addNormal(Vector3F(0, 1.0f, 0));
        }

        auto textureUV = terrainData._TexCoordData[0][it];

        if (!bLand)
        {
            textureUV.first *= terrainData.GetMaxDistance().x / 50.0f ;
            textureUV.second *= terrainData.GetMaxDistance().z / 50.0f;
        }

        _meshData.addTexture(textureUV.first, textureUV.second);
        _meshData.addIndex(_meshData.vertexSize() - 1);

        indexMappingList[it] = _meshData.getIndex(_meshData.indexSize()-1);

    }
}

void TerrainGroupObject::BuildCurtains(const std::pair<Vector2I, Vector2I> rect, const GPSTerrainData& meshobject, const std::vector<Vector2I>& vertexIdxList)
{
    std::pair<bool, bool> params[4] = { {true, true},
                                     {true, false},
                                     {false, true},
                                     {false, false}};

    for( int i =0; i < 4; ++i)
        BuildCurtainsHelper(rect, meshobject, vertexIdxList, params[i].first, params[i].second);
}

void TerrainGroupObject::BuildCurtainsHelper(const std::pair<Vector2I, Vector2I> rect, const GPSTerrainData& meshobject, const std::vector<Vector2I>& vertexIdxList, bool bHoriz, bool bTop)
{
    size_t lastIdx = _meshData.vertexSize();

	const int *pFirstT = nullptr;
	const int *pSecondT = nullptr;

	if (bHoriz)
	{
		pFirstT = &rect.first.x;
		pSecondT = &rect.second.x;
	}
	else
	{
		pFirstT = &rect.first.z;
		pSecondT = &rect.second.z;
	}

	for (int T = *pFirstT; T <= *pSecondT; ++T)
	{
		Vector2I matrixPos;

		if (bHoriz)
		{
			matrixPos.x = T;

			if (bTop)
				matrixPos.z = rect.first.z;
			else
				matrixPos.z = rect.second.z;
		}
		else
		{
			matrixPos.z = T;

			if (bTop)
				matrixPos.x = rect.first.x;
			else
				matrixPos.x = rect.second.x;
		}

		if (std::find(vertexIdxList.cbegin(), vertexIdxList.cend(), matrixPos) == vertexIdxList.cend())
			continue;

		int idx = meshobject.GetIndex(matrixPos.x, matrixPos.z);

        const Vector3F& ptGround = meshobject.GetGroundVertex(idx);
        const Vector3F& pt = meshobject._VertexData[idx];
        const Vector3F& normal = meshobject._NormalData[idx];
        std::pair<float, float> texture = meshobject._TexCoordData[0][idx];

        _meshData.addVertex(pt);
        _meshData.addNormal(normal);
        _meshData.addTexture(texture.first, texture.second);

        _meshData.addVertex(ptGround);
        _meshData.addNormal(normal);
        _meshData.addTexture(texture.first, texture.second);
	}

    for (size_t i = 0; i < _meshData.vertexSize() - lastIdx - 2; ++i)
	{
        _meshData.addIndex(lastIdx + i);
        _meshData.addIndex(lastIdx + i + ((i % 2) ? 2 : 1));
        _meshData.addIndex(lastIdx + i + ((i % 2) ? 1 : 2));
	}
}
