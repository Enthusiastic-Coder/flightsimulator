#pragma once

#include "SpringForceGenerator.h"

class WheelSpringForceGenerator : public SpringForceGenerator
{
public:
	FORCEGENERATOR_TYPE(Type_WheelSpringNormal)

	WheelSpringForceGenerator();
	void onApplyForce(Particle *p, double dt) override;

	class WheelSpringPivotObject : public SpringPivotObject
	{
	public:
		WheelSpringPivotObject(SpringModel& spring, bool bIsSuspensionOnly = false, bool bNoRotation = false);
		void applyTransform(unsigned int camID) override;

	private:
		bool _bIsSuspensionOnly;
		bool _bNoRotation;
	};
};
