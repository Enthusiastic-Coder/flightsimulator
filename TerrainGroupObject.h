#pragma once

#include <sstream>

#include "TileNode.h"
#include "GPSTerrainData.h"
#include "MeshGroupObject.h"

class TerrainGroupObject : public MeshGroupObject
{
public:
    TerrainGroupObject(std::string sName, std::string sParent);
	void Build(bool bLand, const TileNode& tileNode, int chunkNo, int level);
private:
    void BuildTerrainData(bool bLand, const GPSTerrainData& terrainData, std::vector<unsigned int>& indexList);
	void BuildCurtains(const std::pair<Vector2I, Vector2I> rect, const GPSTerrainData&, const std::vector<Vector2I>& vertexIdxList);
	void BuildCurtainsHelper(const std::pair<Vector2I, Vector2I> rect, const GPSTerrainData&, const std::vector<Vector2I>& vertexIdxList, bool bHoriz, bool bTop);
};

