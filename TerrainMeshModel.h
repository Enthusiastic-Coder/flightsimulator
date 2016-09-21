#pragma once

#include "MeshModel.h"
#include "TileNode.h"

class TerrainMeshModel : public MeshModel
{
public:
	TerrainMeshModel();
	void Build(bool bLand, const TileNode& tileNode, int iLevel);

protected:
    MeshGroupObject* factoryGroupObject(std::string name, std::string parent) override;
};
