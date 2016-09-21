#pragma once

#include "HeightTerrainData.h"
#include "ScaledHeightTerrainData.h"
#include "MatrixArrayCoords.h"

class GPSTerrainData
{
public:
    GPSTerrainData(ScaledHeightTerrainData& data);

	void Build(int terrainDimensions);

	GPSLocation GetLocation() const;
	float operator()(double u, double v) const;
	float operator()(GPSLocation location) const;
	int GetIndex(int x, int z) const;
	int GetDimensions() const;
    Vector3F GetMaxDistance() const;

    const Vector3F& GetGroundVertex(size_t idx) const;

    std::vector<Vector3F> _VertexData;
    std::vector<Vector3F> _NormalData;
    std::vector<std::vector<std::pair<float, float>>> _TexCoordData;

private:
	ScaledHeightTerrainData& _HeightData;
	MatrixArrayCoords _dims;

    std::vector<Vector3F> _VertexGroundTerrain;
    Vector3F _maxDistance;
};
