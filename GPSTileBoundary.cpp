#include "stdafx.h"
#include "GPSTileBoundary.h"


GPSTileBoundary::GPSTileBoundary() :
	_tileDimensions(0)
{}

void GPSTileBoundary::set(GPSLocation location, int tileDimensions)
{
	_tileDimensions = tileDimensions;
	_boundary = Vector2D(location.distanceTo(location + GPSLocation(0, 1)),
								location.distanceTo(location + GPSLocation(1, 0)));
}

Vector3D GPSTileBoundary::position(const Vector3D& normal) const
{
	return position(normal.x, normal.y, normal.z);

}
Vector3D GPSTileBoundary::position(double x, double fHeight, double z) const
{
	return Vector3D(x * _boundary.x / _tileDimensions, fHeight, z * _boundary.z / _tileDimensions);
}

GPSLocation GPSTileBoundary::getGPSOffsetFromIndex(int x, int z) const
{
	return GPSLocation(1.0 - double(z) / _tileDimensions, double(x) / _tileDimensions);
}

std::pair<double, double> GPSTileBoundary::getIndexFromGPSOffset(const GPSLocation& offset) const
{
	return std::make_pair(offset._lng * _tileDimensions, _tileDimensions * (1.0 - offset._lat));
}

bool GPSTileBoundary::isIndexContained(GPSLocation offsets[4], int x, int z) const
{
	GPSLocation pt = getGPSOffsetFromIndex(x, z);

	for (int i = 0; i < 2; i++)
	{
		GPSLocation a = offsets[i ? 3 : 1] - offsets[i * 2];
		GPSLocation b = offsets[i ? 1 : 3] - offsets[i * 2];
		GPSLocation p = pt - offsets[i * 2];

		Vector3D vA(a._lng, 0, -a._lat);
		Vector3D vB(b._lng, 0, -b._lat);
		Vector3D vP(p._lng, 0, -p._lat);

		if ((vA ^ vP).y < 0)
			return false;

		if ((vB ^ vP).y > 0)
			return false;
	}

	return true;
}
