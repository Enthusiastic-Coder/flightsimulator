#include "stdafx.h"
#include "GPSTerrainData.h"
#include "GPSModelTransform.h"

GPSTerrainData::GPSTerrainData(ScaledHeightTerrainData &data) :
    _HeightData(data) {}

Vector3F GPSTerrainData::GetMaxDistance() const
{
	return _maxDistance;
}

int GPSTerrainData::GetDimensions() const
{
	return _dims.getX() - 1;
}

float GPSTerrainData::operator()(double u, double v) const
{
	return operator()(GPSLocation(v, u) + GetLocation());
}

float GPSTerrainData::operator()(GPSLocation location) const
{
	return _HeightData(location);
}

const Vector3F& GPSTerrainData::GetGroundVertex(size_t idx) const
{
	return _VertexGroundTerrain[idx];
}

int GPSTerrainData::GetIndex(int x, int z) const
{
	return _dims.toArrayPos(x, z);
}

GPSLocation GPSTerrainData::GetLocation() const
{
	return _HeightData.GetLocation();
}

void GPSTerrainData::Build(int terrainDimensions)
{
    Vector3F heightOffset(0.0, -0.5f, 0.0f);

	heightOffset.y = -0.1f;

	_dims.setDims(terrainDimensions + 1);
	_VertexData.resize(_dims.product());
	_VertexGroundTerrain.resize(_VertexData.size());
	_TexCoordData.resize(1);
	_TexCoordData[0].resize(_VertexData.size());
	_NormalData.resize(_VertexData.size());
	_maxDistance = GetLocation().offSetTo(GetLocation() + GPSLocation(1, 1));

	GPSModelTransform modelSpace(GetLocation());

	for (int dNS = 0; dNS <= GetDimensions(); dNS++)
	{
		for (int dWE = 0; dWE <= GetDimensions(); dWE++)
		{
			double dWERatio = double(dWE) / GetDimensions();
			double dNSRatio = double(dNS) / GetDimensions();

			GPSLocation loc = GetLocation() + GPSLocation(dNSRatio, dWERatio);

			int arrayPos = _dims.toArrayPos(dWE, GetDimensions() - dNS);

			GPSLocation finalHeight(loc + heightOffset);

			_VertexGroundTerrain[arrayPos] = modelSpace.ToLocal(finalHeight);
			_VertexData[arrayPos] = modelSpace.ToLocal(finalHeight + Vector3F(0, _HeightData(loc), 0));

			_NormalData[arrayPos] = _HeightData.GetNormal(loc);
			_TexCoordData[0][arrayPos] = std::make_pair(float(dWERatio), float(dNSRatio));
		}
	}
}
