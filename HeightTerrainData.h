#pragma once

#include <SerializableStdStream.h>
#include "GPSTileBoundary.h"
#include "MatrixArrayCoords.h"

class HeightTerrainData : public ISerializable
{
public:
	virtual ~HeightTerrainData() {}

    SERIALIZE_WRITE_BEGIN(1, 0)
        SERIALIZE_WRITE_ENTRY(_Location)
        SERIALIZE_WRITE_ENTRY(_dims)
        SERIALIZE_WRITE_VECTOR(_heightData)
        SERIALIZE_WRITE_END()

        SERIALIZE_READ_BEGIN(1, 0)
        SERIALIZE_READ_ENTRY(_Location)
        SERIALIZE_READ_ENTRY(_dims)
        SERIALIZE_READ_VECTOR(_heightData)
    SERIALIZE_READ_END()

    GPSLocation GetLocation() const;
	short operator()(int x, int z) const;
	int GetDimensions() const;
	MatrixArrayCoords GetDimCoord() const;

protected:
	void interpolateZ(int z);

protected:
	GPSLocation _Location;
	MatrixArrayCoords _dims;
	std::vector<short> _heightData;
};

