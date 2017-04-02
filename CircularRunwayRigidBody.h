#pragma once
#include "JSONRigidBody.h"
#include <string>
class CircularRunwayRigidBody :
	public JSONRigidBody
{
public:
	CircularRunwayRigidBody(std::string name);
	~CircularRunwayRigidBody();

	/// ISceneryEnvironment
	bool getHeightFromPosition(const GPSLocation& position, HeightData & heightData) const override;

};

