#include "stdafx.h"
#include "SRTM30HeightTerrainData.h"
#include "SRTMFilename.h"
#include <fstream>


#define SMRT30_SIZE				(1201)
#define SMRT30_SIZE_LESS		(SMRT30_SIZE-1)


SRTM30HeightTerrainData::SRTM30HeightTerrainData()
{
	_dims.setDims(SMRT30_SIZE);
}

bool SRTM30HeightTerrainData::Load(std::string strFilename)
{

	_heightData.clear();
	_heightData.resize(_dims.product());

	std::ifstream file(strFilename, std::ios::binary);

	if (!file.is_open())
		return false;

	_Location = SRTMFilename::GetGPSFromSRTM3(strFilename);
	file.read((char*)&_heightData[0], sizeof(_heightData[0])*_dims.product());

    unsigned char b1, b2;

	for (int z = 0; z < _dims.getY(); z++)
	{
		for (int x = 0; x < _dims.getX(); x++)
		{
			int idx = _dims.toArrayPos(x, z);

            b1 = unsigned char(_heightData[idx] >> 8);
            b2 = unsigned char(_heightData[idx]);

			short height = (b2 << 8) | b1;

			_heightData[idx] = height;
		}

		interpolateZ(z);
	}

	return true;
}
