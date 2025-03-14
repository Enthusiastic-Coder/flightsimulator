#include "stdafx.h"
#include "AircraftEngineNozzleBleeder.h"
#include "AircraftEngineNozzleFeeder.h"
#include "GSRigidBody.h"

AircraftEngineNozzleFeeder::AircraftEngineNozzleFeeder() :
	_fThrustLeft(0.0f),
	_bBleedOn(false),
	_fEnergy(0.0f)
{
}

void AircraftEngineNozzleFeeder::onBeforeUpdate(Particle *p, double dt)
{
	_fThrustLeft = getCurrentThrust(p);
}

void AircraftEngineNozzleFeeder::onApplyForce(Particle *p, double dt)
{
	_bBleedOn = true;
	for (auto&it : _nozzles)
	{
		it->onApplyForce(p, dt);
	}
	_bBleedOn = false;

	updateRotorVelPos(dt);
	GSRigidBody *pBody = static_cast<GSRigidBody*>(p);
	pBody->setBodyFrame();
	if (_fThrustLeft > 1.0f)
	{
		int __iBad;
		__iBad = 0;
	}
	applyForce(p, (_fThrustLeft * getEngineDir()).toDouble());
}

bool AircraftEngineNozzleFeeder::bleedOn()
{
	return _bBleedOn;
}

void AircraftEngineNozzleFeeder::setEnergy(float f)
{
    _fEnergy = std::max(std::min(1.0f, f), 0.0f);
}

float AircraftEngineNozzleFeeder::getEnergy()
{
	return _fEnergy;
}

float AircraftEngineNozzleFeeder::requestThrust(float fThrust)
{
	bool bNeg = fThrust < 0.0f;
    float fAvailable = std::min(_fThrustLeft, std::fabs(fThrust));
	_fThrustLeft -= fAvailable;
	return fAvailable * (bNeg?-1:1);
}

void AircraftEngineNozzleFeeder::attachBleeder(AircraftEngineNozzleBleeder* bleeder)
{
	bleeder->attachBleeder(this);
	_nozzles.push_back(bleeder);
}

float AircraftEngineNozzleFeeder::availableThrust()
{
	return _fThrustLeft;
}
