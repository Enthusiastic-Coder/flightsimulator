#pragma once

#include "HeightTerrainData.h"

class SRTM30HeightTerrainData : public HeightTerrainData
{
public:
	SRTM30HeightTerrainData();
	bool Load(std::string strFilename);
};