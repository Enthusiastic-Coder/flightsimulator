#include "stdafx.h"
#include "CircularRunwayRigidBody.h"



CircularRunwayRigidBody::CircularRunwayRigidBody(std::string name) :
	JSONRigidBody(name)
{
}


CircularRunwayRigidBody::~CircularRunwayRigidBody()
{
}

bool CircularRunwayRigidBody::getHeightFromPosition(const GPSLocation & position, HeightData & heightData) const
{
	return false;
}
