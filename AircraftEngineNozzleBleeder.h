#pragma once

#include "GSForceGenerator.h"

class AircraftEngineNozzleFeeder;

class AircraftEngineNozzleBleeder : public GSForceGenerator
{
	friend class AircraftEngineNozzleFeeder;
	FORCEGENERATOR_TYPE(Type_AircraftEngine)
public:

	AircraftEngineNozzleBleeder();
	
	void setDirection(float x, float y, float z);
	void setDw(Vector3F);
	void setThrustPercentage(float);

protected:
	void updateNozzleAngle(float dt);
	void attachBleeder(AircraftEngineNozzleFeeder*);

	void onApplyForce(Particle *p, double dt) override;

private:
	AircraftEngineNozzleFeeder* _pNozzleFeeder;
	bool _bThrustPercentage;
	Vector3F _rateOfRotation;
	Vector3F _currentRotation;
	Vector3F _desiredRotation;

	Vector3F _currentDirection;
	Vector3F _dW;
};