#include "stdafx.h"
#include "ScaledHeightTerrainData.h"
#include <limits>

void ScaledHeightTerrainData::Load(const HeightTerrainData & heightData, unsigned short tileDims)
{
	_Location = heightData.GetLocation();
	_dims.setDims(tileDims + 1);
	_heightData.resize(_dims.product());
	_boundary.set(_Location, tileDims);

	double dFactor = double((heightData.GetDimensions() - 2)) / (_dims.getX() - 1);

	double xpos(0.0), zpos(0.0);
	int zz(0), xx(0);

	for (int z = 0; z < _dims.getY(); z++)
	{
		zz = floorl(zpos = z * dFactor);

		for (int x = 0; x < _dims.getX(); x++)
		{
			xx = floorl(xpos = x * dFactor);

			Vector3D vTopLeft(xx, heightData(xx, zz), zz);
			Vector3D vTopRight(xx + 1, heightData(xx + 1, zz), zz);
			Vector3D vBottomLeft(xx, heightData(xx, zz + 1), zz + 1);

			Vector3D vA = vBottomLeft - vTopLeft;
			Vector3D vB = vTopRight - vTopLeft;
			Vector3D vN = (vA ^ vB).Unit();
			Vector3D fullPt = vA + vB;

			Vector3D offSet((xpos - xx) * fullPt.x + vTopLeft.x, 0, (zpos - zz) * fullPt.z + vTopLeft.z);

			_heightData[_dims.toArrayPos(x, z)] = PlaneF(vN.toFloat(), vTopLeft.toFloat()).Height(offSet.toFloat());
		}
	}
}

void ScaledHeightTerrainData::Build()
{
	if (!_bDirty)
		return;

	GPSTileBoundary space;
	space.set(GetLocation(), _dims.getX() - 1);
	_planeTopLeft.resize(_dims.product());
	_planeBottomRight.resize(_dims.product());
	double dFactor = double((_dims.getX() - 2)) / (_dims.getX() - 1);
	int zz(0), xx(0);

	//GPSLocation gps[4];
	//gps[0] = GPSLocation( 36.128996,-5.363291);
	//gps[1] = GPSLocation( 36.128996, -5.334875 );
	//gps[2] = GPSLocation( 36.135094, -5.334875 );
	//gps[3] = GPSLocation( 36.129000, -5.363291 );
	//float fAvgHeight = calcAverageHeight(gps);

	////////////////////////////////////////////////////////////

	for (int z = 0; z < _dims.getY(); z++)
	{
		zz = floorl(z * dFactor);

		for (int x = 0; x < _dims.getX(); x++)
		{
			xx = floorl(x * dFactor);

			float fHeightTopLeft = HeightTerrainData::operator()(xx, zz);
			float fHeightTopRight = HeightTerrainData::operator()(xx + 1, zz);
			float fHeightBottomLeft = HeightTerrainData::operator()(xx, zz + 1);
			float fHeightBottomRight = HeightTerrainData::operator()(xx + 1, zz + 1);

			Vector3D vTopLeft(xx, fHeightTopLeft, zz);
			Vector3D vTopRight(xx + 1, fHeightTopRight, zz);
			Vector3D vBottomLeft(xx, fHeightBottomLeft, zz + 1);
			Vector3D vBottomRight(xx + 1, fHeightBottomRight, zz + 1);

			Vector3D vScaledTopLeft = space.position(vTopLeft);
			Vector3D vScaledTopRight = space.position(vTopRight);
			Vector3D vScaledBottomLeft = space.position(vBottomLeft);
			Vector3D vScaledBottomRight = space.position(vBottomRight);

			bool bEvenNode = (x + z) % 2 == 0;

			Vector3D vScaledA;
			Vector3D vScaledB;

			if (bEvenNode)
			{
				vScaledA = vScaledBottomRight - vScaledTopLeft;
				vScaledB = vScaledTopRight - vScaledTopLeft;
			}
			else
			{
				vScaledA = vScaledTopLeft - vScaledTopRight;
				vScaledB = vScaledBottomLeft - vScaledTopRight;
			}

			Vector3D vScaledN = (vScaledA ^ vScaledB).Unit();

			int idx = _dims.toArrayPos(x, z);

			_planeTopLeft[idx] = PlaneF(vScaledN.toFloat(), Vector3F(0, fHeightTopLeft, 0));

			Vector3D vScaledA2;
			Vector3D vScaledB2;

			if (bEvenNode)
			{
				vScaledA2 = vScaledBottomLeft - vScaledTopLeft;
				vScaledB2 = vScaledBottomRight - vScaledTopLeft;
			}
			else
			{
				vScaledA2 = vScaledBottomLeft - vScaledTopRight;
				vScaledB2 = vScaledBottomRight - vScaledTopRight;
			}

			Vector3D vScaledN2 = (vScaledA2 ^ vScaledB2).Unit();

			_planeBottomRight[idx] = PlaneF(vScaledN2.toFloat(),
				bEvenNode ? Vector3F(0, fHeightTopLeft, 0) : Vector3F((vScaledBottomRight - vScaledTopLeft).x, fHeightTopRight, 0));
		}
	}

	_bDirty = false;
}

std::pair<double, double> ScaledHeightTerrainData::indexFromGPS(GPSLocation pt) const
{
	return _boundary.getIndexFromGPSOffset(pt - GetLocation());
}

GPSLocation ScaledHeightTerrainData::getGPSOffsetFromIndex(int x, int z) const
{
	return _boundary.getGPSOffsetFromIndex(x, z) + GetLocation();
}

float ScaledHeightTerrainData::calcAverageHeight(GPSLocation boundary[4]) const
{
	GPSLocation gpsOffsets[4];

	for (int i = 0; i < 4; i++)
		gpsOffsets[i] = boundary[i] - GetLocation();

	float fHeight(0.0f);
	int iCount(0);

	for (int z = 0; z < _dims.getY(); z++)
	{
		for (int x = 0; x < _dims.getX(); x++)
		{
			if (_boundary.isIndexContained(gpsOffsets, x, z))
			{
				fHeight += HeightTerrainData::operator()(x, z);
				iCount++;
			}
		}
	}

	if (iCount == 0)
		return 0.0f;

	return fHeight / iCount;
}

bool ScaledHeightTerrainData::getHeightFromPosition(const GPSLocation& location, HeightData& heightData) const /*override*/
{
	PlaneF plane;
	Vector3F offset;

	if (!GetPlane(location, plane, &offset))
		return false;

	heightData.setData(location._height - plane.Height(offset), plane.GetQ());

	return true;
}

float ScaledHeightTerrainData::operator()(const GPSLocation &location) const
{
	PlaneF plane;
	Vector3F offset;

	if (!GetPlane(location, plane, &offset))
		return 0.0f;

	return plane.Height(offset);
}

bool ScaledHeightTerrainData::PointIsContained(const GPSLocation& location) const
{
	GPSLocation offset = location - GetLocation();
	return(offset._lat >= 0 && offset._lat <= 1.0 && offset._lng >= 0.0 && offset._lng <= 1.0);
}

Vector3F ScaledHeightTerrainData::GetNormal(const GPSLocation& location) const
{
	PlaneF plane;

	if (!GetPlane(location, plane))
		return Vector3F(0, 1, 0);

	return plane.N;
}

bool ScaledHeightTerrainData::GetPlane(const GPSLocation& location, PlaneF& plane, Vector3F * pPtOffset) const
{
	if (_planeTopLeft.size() == 0)
		return false;

	if (_planeBottomRight.size() == 0)
		return false;

	if (!PointIsContained(location))
		return false;

	std::pair<double, double>  dMatrixPos = _boundary.getIndexFromGPSOffset(location - GetLocation());
	std::pair<long, long> lMatrixPos = std::make_pair((long)floorl(dMatrixPos.first), (long)floorl(dMatrixPos.second));

	double dU = (dMatrixPos.first - lMatrixPos.first);
	double dV = (dMatrixPos.second - lMatrixPos.second);

	if (fabs(dV) < std::numeric_limits<float>::epsilon())
		if (dV < 0.0)
			dV = -std::numeric_limits<float>::epsilon();
		else
			dV = std::numeric_limits<float>::epsilon();

	bool bEvenNode = (lMatrixPos.first + lMatrixPos.second) % 2 == 0;
	double dGradUV = (bEvenNode ? dU : 1.0 - dU) / dV;

	int idx = _dims.toArrayPos(lMatrixPos.first, lMatrixPos.second);
	plane = (dGradUV > 1.0) ? _planeTopLeft[idx] : _planeBottomRight[idx];

	if (pPtOffset)
		*pPtOffset = (_boundary.position(dMatrixPos.first, location._height, dMatrixPos.second) -
			_boundary.position(lMatrixPos.first, 0, lMatrixPos.second)).toFloat();

	return true;
}
