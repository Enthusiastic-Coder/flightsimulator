#include "stdafx.h"
#include "HeightTerrainData.h"
#include <jibbs/matrix/MatrixArrayCoords.h>

GPSLocation HeightTerrainData::GetLocation() const
{
	return _Location;
}

short HeightTerrainData::operator()(int x, int z) const
{
	return _heightData[_dims.toArrayPos(x, z)];
}

int HeightTerrainData::GetDimensions() const
{
	return _dims.getX();
}

MatrixArrayCoords HeightTerrainData::GetDimCoord() const
{
	return _dims;
}

void HeightTerrainData::interpolateZ(int z)
{
	int startX = 0;
	bool bInBadMode(false);
	const short sBadHeight = -32768;

	HeightTerrainData& terrain = *this;

	for (int x = 0; x < _dims.getX(); ++x)
	{
		short sHeight = terrain(x, z);

		if (bInBadMode)
		{
			if (sHeight != sBadHeight)
			{
				bInBadMode = false;

				short sStartHeight = terrain(startX - 1, z);
				short sHeightDiff = sHeight - sStartHeight;
				short sDiffX = x - (startX - 1);
				float fGrad = float(sHeightDiff) / sDiffX;

				for (int xx = startX; xx < x; ++xx)
				{
					int idx = _dims.toArrayPos(xx, z);
					_heightData[idx] = sStartHeight + fGrad * (xx - startX + 1);
				}
			}
		}
		else
		{
			if (sHeight == sBadHeight)
			{
				startX = x;
				bInBadMode = true;
			}
		}
	}

	if (bInBadMode)
	{
		for (int x = startX; x < _dims.getX(); ++x)
		{
			int idx = _dims.toArrayPos(x, z);
			_heightData[idx] = terrain(startX - 1, z);
		}
	}
}
