#include "stdafx.h"
#include "AircraftEngineNozzleBleeder.h"
#include "JSONRigidBody.h"
#include "AircraftEngineNozzleFeeder.h"

AircraftEngineNozzleBleeder::AircraftEngineNozzleBleeder() :
    _bThrustPercentage(false),
	_rateOfRotation(Vector3F(70,70,70)),
    _currentDirection(Vector3F(-180,0,0))
{
}

void AircraftEngineNozzleBleeder::attachBleeder(AircraftEngineNozzleFeeder* pFeeder)
{
	_pNozzleFeeder = pFeeder;
}

void AircraftEngineNozzleBleeder::setDw(Vector3F dw)
{
	_dW = dw;
	_bThrustPercentage = false;
}

void AircraftEngineNozzleBleeder::setDirection(float x, float y, float z)
{
	_desiredRotation = Vector3F(x, y, z);
}

void AircraftEngineNozzleBleeder::setThrustPercentage(float f)
{
	_dW.x = f;
	_bThrustPercentage = true;
}

void AircraftEngineNozzleBleeder::onApplyForce(Particle* p, double dt)
{
	if (_pNozzleFeeder == 0)
		return;

	if (_pNozzleFeeder->bleedOn() == false)
		return;

	JSONRigidBody * pBody = (JSONRigidBody*)p;
	updateNozzleAngle(float(dt));

	float forceApplied(0.0f);

	float fSpeed = (float)p->velocity().Magnitude();
	if (fSpeed <= 50.0f)
	{
		float fFraction = (50.0f - fSpeed) / 50.0f;
		fFraction *= fFraction;
		fFraction *= _pNozzleFeeder->getEnergy();

		if (_bThrustPercentage)
		{
			forceApplied = _pNozzleFeeder->requestThrust(_pNozzleFeeder->availableThrust() * _dW.x);
		}
		else
		{
			Vector3F r = getContactPointCG(pBody->cg()).toFloat();
			float rMag = r.Magnitude();
			Vector3F f = pBody->getMassChannel().MOI() / (rMag * rMag) * (_dW ^ r) / float(dt);

			forceApplied = _pNozzleFeeder->requestThrust(fFraction * f * _currentDirection);
		}
	}

	pBody->setBodyFrame();
	applyForce(p, (forceApplied * _currentDirection).toDouble());
}

void AircraftEngineNozzleBleeder::updateNozzleAngle(float dt)
{
	if (_currentRotation == _desiredRotation)
		return;

	Vector3F rateRot = _rateOfRotation * dt;
	Vector3F diffRot = _desiredRotation - _currentRotation;

	if (std::abs(diffRot.x) < std::abs(rateRot.x))
		_currentRotation.x = _desiredRotation.x;
	else
	{
		if (diffRot.x > 0)
			_currentRotation.x += rateRot.x;
		else
			_currentRotation.x -= rateRot.x;
	}

	if (std::abs(diffRot.y) < std::abs(rateRot.y))
		_currentRotation.y = _desiredRotation.y;
	else
	{
		if( diffRot.y > 0 )
			_currentRotation.y += rateRot.y;
		else
			_currentRotation.y -= rateRot.y;
	}

	if (std::abs(diffRot.z) < std::abs(rateRot.z))
		_currentRotation.z = _desiredRotation.z;
	else
	{
		if (diffRot.z > 0)
			_currentRotation.z += rateRot.z;
		else
			_currentRotation.z -= rateRot.z;
	}

	QuarternionF q = MathSupport<float>::MakeQOrientation(_currentRotation);
	_currentDirection = QVRotate(q, Vector3F(0,0,-1));
}

