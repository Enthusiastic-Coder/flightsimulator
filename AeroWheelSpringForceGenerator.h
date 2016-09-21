#pragma once

#include "AirProperties.h"
#include "WheelSpringForceGenerator.h"

class AeroWheelSpringForceGenerator : public WheelSpringForceGenerator
{
public:
	FORCEGENERATOR_TYPE(Type_AeroWheelSpringNormal)
	void onApplyForce(Particle *p, double dt) override;
};