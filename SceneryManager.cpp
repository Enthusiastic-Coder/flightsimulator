#include "stdafx.h"
#include "SceneryManager.h"

#include <GPSLocation.h>
#include <HeightData.h>
#include <vector>

GPSLocation rightSlopeStart(51.467, -0.47166664);
GPSLocation rightSlopeEnd(51.467, -0.45723146);

GPSLocation leftSlopeStart( 51.467, -0.48 );
GPSLocation leftSlopeEnd( 51.467, -0.53 );

double flatGradientFromGPSDiff(GPSLocation from, GPSLocation to, double dGPSGradient )
{
    GPSLocation dGps = to - from;
    double dHeight = 0.0;

    if( dGps._lat == 0 )
        dHeight = dGps._lng * dGPSGradient;
    else
        dHeight = dGps._lat * dGPSGradient;

    double dDist = to.distanceTo( from );
    return dHeight/dDist;
}

SceneryManager::SceneryManager(WorldSystem *pWorldSystem) :
	_pWorldSystem(pWorldSystem)
{
}

bool SceneryManager::getHeightFromPosition(const GPSLocation& position, HeightData & heightData) const
{
	std::vector<HeightData> planes;

//	if (position._lng <= leftSlopeStart._lng && position._lng > leftSlopeEnd._lng
//		&& position._lat < leftSlopeStart._lat + 0.00125
//		&& position._lat > leftSlopeStart._lat - 0.00125
//		)
//	{
//		static float fHeightOffGround = 40.0f;

//		static double dGrad = -flatGradientFromGPSDiff(leftSlopeStart, leftSlopeEnd, LEFT_SLOPE_GRAD);
//		static double dDist = leftSlopeStart.distanceTo(leftSlopeEnd);
//		static VectorF a(0, 0, -100);
//		static VectorF b(-100, 100 * dGrad, 0);
//		static VectorF c(0, fHeightOffGround + 0.1f, -100);
//		static PlaneF plane(a^b, c);
//		double dLng = position._lng - leftSlopeStart._lng;

//		HeightData pe;
//		pe.setData(leftSlopeStart.offSetTo(position), plane);
//		planes.push_back(pe);
//	}
//	else if (position._lng > rightSlopeStart._lng && position._lng <= rightSlopeEnd._lng
//		&& position._lat < rightSlopeStart._lat + 0.00125
//		&& position._lat > rightSlopeStart._lat - 0.00125
//		)
//	{
//		static double dGrad = flatGradientFromGPSDiff(rightSlopeStart, rightSlopeEnd, RIGHT_SLOPE_GRAD);
//		static double dDist = rightSlopeEnd.distanceTo(rightSlopeStart);
//		static VectorF a(100, 100 * dGrad, 0);
//		static VectorF b(0, 0, -100);
//		static PlaneF plane(a^b);
//		double dLng = position._lng - rightSlopeStart._lng;

//		HeightData pe;
//		pe.setData(rightSlopeStart.offSetTo(position), plane);
//		planes.push_back(pe);
//	}

	size_t idx = GetHeightIndexFromPlanes(planes);

	if (idx == -1)
		return false;

	heightData = planes[idx];
	return true;
}
