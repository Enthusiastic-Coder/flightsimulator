#include "stdafx.h"
#include "TerrainMeshModel.h"
#include "TerrainGroupObject.h"
#include <iostream>

TerrainMeshModel::TerrainMeshModel()
{
}

void TerrainMeshModel::Build(bool bLand, const TileNode& tileNode, int iLevel)
{
	GPSLocation loc = tileNode.getGPSGroupObject().GetLocation();
	std::string sep("|");
	std::stringstream ss;

	ss << "N" << loc._lat << sep << "E" << loc._lng << sep << (bLand ? "Land" : "Sea");
	setName(ss.str());

    std::cout << "Building Terrain " << ss.str() << std::endl;

	for (size_t chunkNo = 0; chunkNo < tileNode.size(); ++chunkNo)
	{
		std::stringstream ss;
		ss << iLevel << sep << chunkNo;

        TerrainGroupObject* group = (TerrainGroupObject*)addGroup(ss.str());
        group->Build(bLand, tileNode, chunkNo, iLevel);
	}
    calcBoundingBox();
}

MeshGroupObject* TerrainMeshModel::factoryGroupObject(std::string name, std::string parent)
{
    return new TerrainGroupObject(name, parent);
}

