#include "stdafx.h"
#include "AircraftEngineForceGenerator.h"
#include "AirProperties.h"
#include "GSRigidBody.h"
#include "OpenGLPipeline.h"
#include <algorithm>
#include <cmath>

AircraftEngineForceGenerator::AircraftEngineForceGenerator(ThrustResponse response) :
    _angle_position(0.0),
    _angle_velocity(0.0),
    _thrust_percent(0.0),
	_actual_thrust_percent(0.0),
	_thrustResponse(response),
	_max_thrust(0.0),
    _max_drag(0.0f),
	_vInvFrac(1.0f),
    _density0(AirProperties::Density(0)),
    _max_spd(0.0f),
	_engine_direction(Vector3F(0,0,-1))
{ }

void AircraftEngineForceGenerator::onApplyForce(Particle *p, double dt)
{
	updateRotorVelPos(dt);
	static_cast<GSRigidBody*>(p)->setBodyFrame();
	applyForce(p, (getCurrentThrust(p) * _engine_direction).toDouble());
}

void AircraftEngineForceGenerator::setRecorderHook(FlightRecorder& a)
{
	GSForceGenerator::setRecorderHook(a);
	a.addDataRef(_actual_thrust_percent);
	a.addDataRef(_angle_position);
}

void AircraftEngineForceGenerator::setMaxThrust(float value)
{
	_max_thrust = value;
}

void AircraftEngineForceGenerator::setEngineDir(Vector3F value)
{
    _engine_direction = value;
}

void AircraftEngineForceGenerator::setAngle(float value)
{
    _angle_position = value;
}

Vector3F AircraftEngineForceGenerator::getEngineDir()
{
	return _engine_direction;
}

void AircraftEngineForceGenerator::setMaxSpd(float maxMach)
{
	_max_spd = AirProperties::SpeedOfSound(0, 0) * maxMach;
}

void AircraftEngineForceGenerator::incrDrag(float cd, float area, float n1)
{
	_max_drag += 0.5f * cd * _density0 * _max_spd * _max_spd * area;
	_vInvFrac = (_max_thrust * _max_spd) / (_max_thrust - _max_drag / n1);
}

float AircraftEngineForceGenerator::getMaxThrustAvailable(float spd, float fHeight)
{
	return (1.0f - spd / _vInvFrac) * AirProperties::Density(fHeight) / _density0 * _max_thrust;
}

float AircraftEngineForceGenerator::a320ThrustFraction() const
{
	const float spool = std::clamp(_actual_thrust_percent * 0.01f, 0.0f, 1.0f);
	constexpr float idleThrustFraction = 0.025f;
	return idleThrustFraction + (1.0f - idleThrustFraction) * spool * spool;
}

float AircraftEngineForceGenerator::getThrustOutputPercent() const
{
	return _thrustResponse == ThrustResponse::A320Turbofan ? a320ThrustFraction() * 100.0f : _actual_thrust_percent;
}

float AircraftEngineForceGenerator::getCurrentThrust(Particle * p)
{
	double height = static_cast<GSRigidBody*>(p)->Height();
	double ias = AirProperties::Airspeed(p->velocity().Magnitude(), height, 0);
	if (_thrustResponse == ThrustResponse::A320Turbofan)
		return std::max(getMaxThrustAvailable(ias, height), 0.0f) * a320ThrustFraction();
    return getMaxThrustAvailable(ias, height) * std::max(_actual_thrust_percent - 30, 0.0f) / 70.0f;
}

void AircraftEngineForceGenerator::updateRotorVelPos(double dt)
{
	float fDegPerSec = 20.0f;

	double req_velocity = fDegPerSec * _actual_thrust_percent / 100.0f * 360.0f * dt;

	if (fabs(_angle_velocity - req_velocity) < dt)
		_angle_velocity = req_velocity;
	else if (_angle_velocity < req_velocity)
		_angle_velocity += dt;
	else
		_angle_velocity -= dt;

	if (_thrustResponse == ThrustResponse::A320Turbofan)
	{
		const float target = std::clamp(_thrust_percent, 0.0f, 100.0f);
		// Coupled with the quadratic thrust curve, this reaches 95% thrust
		// in about six seconds from idle, and builds thrust faster from mid spool.
		constexpr double spoolUpSeconds = 1.55;
		constexpr double spoolDownSeconds = 1.9;
		const double timeConstant = target > _actual_thrust_percent ? spoolUpSeconds : spoolDownSeconds;
		_actual_thrust_percent += (target - _actual_thrust_percent) * -std::expm1(-dt / timeConstant);
	}
	else if (fabs(_actual_thrust_percent - _thrust_percent) < dt * fDegPerSec)
		_actual_thrust_percent = _thrust_percent;
	else if (_actual_thrust_percent < _thrust_percent)
		_actual_thrust_percent += dt * fDegPerSec;
	else
		_actual_thrust_percent -= dt * fDegPerSec;

	_angle_position += _angle_velocity;

}

////////////////////////////////////////////////////////////////

AircraftEngineForceGenerator::AircraftEnginePivotObject::AircraftEnginePivotObject(AircraftEngineForceGenerator* pEngine)
	:_pEngine(pEngine) {}

void AircraftEngineForceGenerator::AircraftEnginePivotObject::applyTransform(unsigned int camID)
{
	OpenGLMatrixStack& mv = OpenGLPipeline::Get(camID).GetModel();

	mv.Top().Translate(getT());
	mv.Top().RotateModel(_pEngine->_angle_position, getR());
	mv.Top().Translate(-getT());
}
