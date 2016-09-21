#include "stdafx.h"
#include "SpringModel.h"
#include <algorithm>

SpringModel::SpringModel()
{
	_channel_mass = 0.0;
	_spring_mass = 0.0;
	_restitution = 0.0;
	_spring_friction = 0.0;
	_spring_minimum = 0.0;
	_spring_equilibrium = 0.0;
	_spring_length = 0.0;
	_spring_constant = 0.0;
	_separator_velocity = 0.0;
	_separator_distance = 0.0;
	_resting_mass_velocity = 0.0;
	_normalMass = 0.0;
	_ground_friction_coeff = 0.0;
	_dMaxFriction = 0.0;
	_spring_normal = Vector3D(0, -1.0, 0);
	_bHasWheels = false;
	_qSteering = QuarternionD::Q0();
	_steering_angle = 0.0;
	_wheel_linear_velocity = 0.0;
	_wheel_radius = 0.0;
	_wheel_angle_position = 0.0;
	_DrivingState = DrivingState::BRAKE;
}

void SpringModel::reset()
{
	_separator_velocity = 0.0;
	_separator_distance = _spring_equilibrium;
	_resting_mass_velocity = 0.0;
	_dMaxFriction = 0.0;
	_normalMass = 0.0;
	_springForce = 0.0;
	_velocityBody.Reset();
	_wheel_linear_velocity = 0.0;
	_wheel_angle_position = 0.0;
	_steering_angle = 0.0;
	_qSteering = QuarternionD::Q0();
	_DrivingState = DrivingState::BRAKE;
}

void SpringModel::setChannelMass(double v)
{
	_channel_mass = v;
}

void SpringModel::setSpringMass(double v)
{
	_spring_mass = v;
}

void SpringModel::setMinimumDistance(double v)
{
	_spring_minimum = v;
}

void SpringModel::setEquilibriumDistance(double v)
{
	_spring_equilibrium = v;
	_separator_distance = v;
}

void SpringModel::setLength(double v)
{
	_spring_length = v;
}

void SpringModel::setMinEqLen(double min, double eq, double dx)
{
	setMinimumDistance(min);
	setEquilibriumDistance(eq);
	setLength(eq + dx);
}

void SpringModel::setRestitution(double v)
{
	_restitution = v;
}

void SpringModel::setContactFrictionCoefficient(double v)
{
	_ground_friction_coeff = tan(DegreesToRadians(v));
}

void SpringModel::setSpringFriction(double v)
{
	_spring_friction = v;
}

void SpringModel::calculateSpringConstant()
{
	_spring_constant = _channel_mass * 9.80665 / fabs(_spring_length - _spring_equilibrium);
}

double SpringModel::normalMass() const
{
	return _normalMass;
}

double SpringModel::channelMass() const
{
	return _channel_mass;
}

double SpringModel::springMass() const
{
	return _spring_mass;
}

double SpringModel::springForce() const
{
	return _springForce;
}

double SpringModel::getMaxFrictionForce() const
{
	return _dMaxFriction;
}

double SpringModel::groundFrictionCoefficient() const
{
	return _ground_friction_coeff;
}

void SpringModel::setSpringNormal(Vector3D n)
{
	_spring_normal = n;
}

void SpringModel::setRecorderHook(FlightRecorder& a)
{
	a.addDataRef(_separator_distance);
	a.addDataRef(_steering_angle);
	a.addDataRef(_wheel_angle_position);
}

void SpringModel::resetSpringForces()
{
	_qGround = _qSteering * ~(_heightData.qNormal().toDouble()) * _qBodyOrient;
	_velocityBodyGroundFrame = QVRotate(_qGround, _velocityBody);
	double velocity = -_velocityBodyGroundFrame * _spring_normal;
	_springForce = 0.0;
	_separator_velocity += velocity - _resting_mass_velocity;
	_resting_mass_velocity = velocity;
	_forceBody.Reset();
}

void SpringModel::applySpringForce(double dt)
{
	double dx = _spring_length - _separator_distance;
	double factor = dt / _spring_mass;
	double tension = _spring_constant * dx;

	_separator_velocity += factor * tension;
	double dvel = factor * _spring_friction * _separator_velocity;

	_separator_velocity -= fabs(dvel) > fabs(_separator_velocity) ? _separator_velocity : dvel;
	_separator_distance += _separator_velocity * dt;

	double friction = _spring_friction * _resting_mass_velocity;
	tension -= fabs(friction) > fabs(tension) ? tension : friction;

	if (tension > 0)
		_springForce += tension;
}

void SpringModel::applyGravity(double dt)
{
	_separator_velocity -= QVRotate(~(_heightData.qNormal().toDouble()), -_gravity).y * dt;
	_separator_distance += _separator_velocity * dt;
}

void SpringModel::applyConstraints(double dt)
{
	if (_separator_distance < _spring_minimum)
	{
		double tension = -(_restitution + 1) * _channel_mass * _spring_mass
			*  _separator_velocity
			/ (_channel_mass + _spring_mass) / dt;

		if (tension > 0)
			_springForce += tension;

		_separator_velocity += tension / _spring_mass * dt;
		_separator_distance += _separator_velocity * dt;
	}

    double dMaxSepDist = std::min(_spring_length, _heightData.Height() + _separator_distance);

	if (_separator_distance > dMaxSepDist)
	{
		_separator_velocity = 0;
		_separator_distance = dMaxSepDist;
	}

	if (_separator_distance < _spring_minimum)
	{
		_separator_distance = _spring_minimum;

		if (_separator_velocity  < 0)
			_separator_velocity = 0.0f;

		// _resting_mass is calculated by JSONRigidBody distribution algorithm
		double tension = -(_restitution + 1) * _channel_mass * _resting_mass_velocity / dt;

		if (tension > 0)
			_springForce += tension;
	}
}

double SpringModel::separatorDistance() const
{
	return _separator_distance;
}

double SpringModel::displacementDistance() const
{
	return _separator_distance - _spring_equilibrium;
}

// ************ Simulation in RealTime ******************
void SpringModel::update(double dt)
{
	// Normal Force Y
	resetSpringForces();
	applyGravity(dt);
	applySpringForce(dt);
	applyConstraints(dt);

	// Ground X-Z Friction + use of Normal Y force
	if (isGroundContact())
		applyGroundForce(dt);

	onUpdateWheelRotation(dt);
}

void SpringModel::setBodyOrientation(const QuarternionD& qOrient)
{
	_qBodyOrient = qOrient;
}

void SpringModel::setSteeringOrientation(const QuarternionD& qSteering)
{
	_qSteering = qSteering;
}

void SpringModel::setVelocityBody(const Vector3D& velocityBody)
{
	_velocityBody = velocityBody;
}

void SpringModel::setGravity(const Vector3D& gravity)
{
	_gravity = gravity;
}

void SpringModel::setWheelRadius(double r)
{
	_wheel_radius = r;
}

void SpringModel::setHasWheels(bool bHasWheels)
{
	_bHasWheels = bHasWheels;
}

void SpringModel::setDrivingState(SpringModel::DrivingState d)
{
    _DrivingState = d;
}

void SpringModel::setSteeringAngle(double v)
{
    _steering_angle = v;
}

void SpringModel::incrSteeringAngle(double diff)
{
    _steering_angle += diff;
}

void SpringModel::setWheelSpeed(double v)
{
    _wheel_linear_velocity = v;
}

void SpringModel::incrWheelSpeed(double diff)
{
    _wheel_linear_velocity += diff;
}

bool SpringModel::hasWheels()
{
	return _bHasWheels;
}

double SpringModel::getWheelRadius()
{
	return _wheel_radius;
}

const Vector3D& SpringModel::getAppliedForce() const
{
	return _forceBody;
}

Vector3D SpringModel::getBodyForce()
{
	return QVRotate(~_qGround, _forceBody);
}

HeightData& SpringModel::getHeightData()
{
	return _heightData;
}

const Vector3D& SpringModel::getGravity() const
{
	return _gravity;
}

const Vector3D& SpringModel::getVelocityBody() const
{
	return _velocityBody;
}

SpringModel::DrivingState SpringModel::getDrivingState()
{
	return _DrivingState;
}

double SpringModel::getWheelSpeed()
{
	return _wheel_linear_velocity;
}

double SpringModel::getSteeringAngle()
{
	return _steering_angle;
}

bool SpringModel::isGroundContact()
{
	return getHeightData().Height() <= 0.1;
}

Vector3D SpringModel::separatorPosition(const Vector3D& contactPos) const
{
	// Local Model coordinates
	return contactPos + _separator_distance * _spring_normal;
}

void SpringModel::applyGroundForce(double dt)
{
	_normalMass = _springForce / _gravity.y;
	_dMaxFriction = fabs(groundFrictionCoefficient() * _springForce);

	_forceBody = _velocityBodyGroundFrame * _normalMass / dt;
    _forceBody.x = (_forceBody.x > 0 ? -1 : 1) * std::min(fabs(_forceBody.x), _dMaxFriction);
	_forceBody.y = _springForce;
    _forceBody.z = (_forceBody.z > 0 ? -1 : 1) * std::min(fabs(_forceBody.z), _dMaxFriction);

	if (_bHasWheels)
		onApplySteeringForce(dt);
}

void SpringModel::onApplySteeringForce(double dt)
{
	double fMaxLinearAcceleration = groundFrictionCoefficient() * getGravity().y * dt;
	double fDv = getVelocityBody().z - _wheel_linear_velocity;
	double fMaxRotDv = 2.0 * _normalMass / _spring_mass * fMaxLinearAcceleration;

	// Wheel is Slave to friction
	if (fabs(fDv) < fMaxRotDv)
	{
		_wheel_linear_velocity += fDv;
	}
	else if (fDv>0)
	{
		_wheel_linear_velocity += fMaxRotDv;
	}
	else
	{
		_wheel_linear_velocity -= fMaxRotDv;
	}

	if (_DrivingState == DrivingState::BRAKE)
		return;

	double dReactiveRestingMassForce = fabs(_normalMass * fDv / dt);

	if (_DrivingState == DrivingState::POWERED)
        _forceBody.z = std::min(dReactiveRestingMassForce, getMaxFrictionForce());
	else
        _forceBody.z = std::min(dReactiveRestingMassForce, fabs(_forceBody.z));

	if (fDv > 0)
		_forceBody.z = -_forceBody.z;
}

void SpringModel::onUpdateWheelRotation(double dt)
{
	if (_bHasWheels)
	{
		_wheel_angle_position += _wheel_linear_velocity / getWheelRadius() * dt;

		if (_wheel_angle_position > 360)
			_wheel_angle_position -= 360;

		if (_wheel_angle_position < -360)
			_wheel_angle_position += 360;
	}
}

double SpringModel::wheelAnglePosition() const
{
	return RadiansToDegrees(_wheel_angle_position);
}

double SpringModel::getLocalSteeringDirection()
{
	return -_steering_angle;
}

//////////////////////////////////////////////////////////

