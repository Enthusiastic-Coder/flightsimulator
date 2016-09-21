#pragma once
#include <SerializableStdStream.h>
#include "HeightTerrainData.h"
#include "interfaces.h"

class ScaledHeightTerrainData : public HeightTerrainData, public ISceneryEnvironment
{
public:
	ScaledHeightTerrainData() : _bDirty(true) {}
    SERIALIZE_WRITE_BEGIN(1, 0)
        SERIALIZE_WRITE_OBJECT(_boundary)
        SERIALIZE_WRITE_VECTOR(_planeTopLeft)
        SERIALIZE_WRITE_VECTOR(_planeBottomRight)
        SERIALIZE_WRITE_SUPER(HeightTerrainData)
        SERIALIZE_WRITE_END()

        SERIALIZE_READ_BEGIN(1, 0)
        SERIALIZE_READ_OBJECT(_boundary)
        SERIALIZE_READ_VECTOR(_planeTopLeft)
        SERIALIZE_READ_VECTOR(_planeBottomRight)
        SERIALIZE_READ_SUPER(HeightTerrainData)
        SERIALIZE_READ_END()

        SERIALIZE_READ_CLASS(BinaryReadStream)
        SERIALIZE_WRITE_CLASS(BinaryWriteStream)

        void Load(const HeightTerrainData & heightdata, unsigned short tileDims);
	void Build();

	float operator()(const GPSLocation &location) const;
	bool PointIsContained(const GPSLocation& location) const;
    Vector3F GetNormal(const GPSLocation& location) const;
    bool GetPlane(const GPSLocation& location, PlaneF & plane, Vector3F * pPtOffset = nullptr) const;
    bool getHeightFromPosition(const GPSLocation& location, HeightData& heightData) const override;

	float calcAverageHeight(GPSLocation boundary[4]) const;
	std::pair<double, double> indexFromGPS(GPSLocation pt) const;
	GPSLocation getGPSOffsetFromIndex(int x, int z) const;

private:
	bool _bDirty;
	GPSTileBoundary _boundary;
	std::vector<PlaneF> _planeTopLeft;
	std::vector<PlaneF> _planeBottomRight;
};
