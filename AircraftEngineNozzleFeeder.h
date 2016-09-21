#pragma once

#include "AircraftEngineForceGenerator.h"

class AircraftEngineNozzleBleeder;

class AircraftEngineNozzleFeeder : public AircraftEngineForceGenerator
{
	friend class AircraftEngineNozzleBleeder;
public:
	AircraftEngineNozzleFeeder();

	void attachBleeder(AircraftEngineNozzleBleeder* bleeder);
	float availableThrust();
	bool bleedOn();
	void setEnergy(float f);
	float getEnergy();

protected:
	float requestThrust(float fThrust);

	void onBeforeUpdate(Particle *p, double dt) override;
	void onApplyForce(Particle *p, double dt) override;

private:
	float _fThrustLeft;
	bool _bBleedOn;
	float _fEnergy;
	std::vector<AircraftEngineNozzleBleeder*> _nozzles;
};