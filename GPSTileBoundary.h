#pragma once

#include <SerializableStdStream.h>

class GPSTileBoundary : public ISerializable
{
public:
	SERIALIZE_WRITE_BEGIN(1, 0)
		SERIALIZE_WRITE_ENTRY(_tileDimensions)
		SERIALIZE_WRITE_ENTRY(_boundary)
		SERIALIZE_WRITE_END()

		SERIALIZE_READ_BEGIN(1, 0)
		SERIALIZE_READ_ENTRY(_tileDimensions)
		SERIALIZE_READ_ENTRY(_boundary)
		SERIALIZE_READ_END()

	GPSTileBoundary();

	void set(GPSLocation location, int tileDimensions);
	Vector3D position(const Vector3D& normal) const;
	Vector3D position(double x, double fHeight, double z) const;
	GPSLocation getGPSOffsetFromIndex(int x, int z) const;
	std::pair<double, double> getIndexFromGPSOffset(const GPSLocation& offset) const;
	bool isIndexContained(GPSLocation offsets[4], int x, int z) const;

private:
	int _tileDimensions;
	Vector2D _boundary;
};
