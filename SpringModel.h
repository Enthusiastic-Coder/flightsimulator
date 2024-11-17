#pragma once

#include "interfaces.h"
#include "FlightRecorder.h"
#include <HeightData.h>
#include <jibbs/vector/vector3.h>
#include <jibbs/math/Quarternion.h>

class SpringModel : public IDataRecordable<FlightRecorder>
{
public:

	enum class DrivingState { BRAKE, NEUTRAL, POWERED };

	SpringModel();

	void reset();
	void setChannelMass(double v);
	void setSpringMass(double v);

	void setMinimumDistance(double v);
	void setEquilibriumDistance(double v);
	void setLength(double v);

	void setMinEqLen(double min, double eq, double dx);

	void setRestitution(double v);
	void setSpringFriction(double v);
	void setContactFrictionCoefficient(double v);
	void setSpringNormal(Vector3D n);

	void calculateSpringConstant();

	double separatorDistance() const;
	double displacementDistance() const;

	double channelMass() const;
	double normalMass() const;
	double springMass() const;
	double springForce() const;
	double getMaxFrictionForce() const;
	double groundFrictionCoefficient() const;

	// ************ Simulation in RealTime ******************
	void update(double dt);
	void setRecorderHook(FlightRecorder& a) override;

	void setBodyOrientation(const QuarternionD& qOrient);
	void setSteeringOrientation(const QuarternionD& qSteering);
	void setVelocityBody(const Vector3D& velocityBody);
	void setGravity(const Vector3D& gravity);
	void setWheelRadius(double r);
	void setHasWheels(bool bHasWheels);
	void setDrivingState(DrivingState d);
    void setSteeringAngle(double v);
    void incrSteeringAngle(double diff);
    void setWheelSpeed(double v);
    void incrWheelSpeed(double diff);

    bool hasWheels();
	double getWheelRadius();
	const Vector3D& getAppliedForce() const;
	Vector3D getBodyForce();
	HeightData& getHeightData();
	const Vector3D& getGravity() const;
	const Vector3D& getVelocityBody() const;
	DrivingState getDrivingState();
    double getWheelSpeed();
    double getSteeringAngle();
	bool isGroundContact();
	Vector3D separatorPosition(const Vector3D& contactPos) const;

	double wheelAnglePosition() const;
	double getLocalSteeringDirection();


protected:
	void resetSpringForces();
	void applyGravity(double dt);
	void applySpringForce(double dt);
	void applyConstraints(double dt);
	void applyGroundForce(double dt);
	void onApplySteeringForce(double dt);
	void onUpdateWheelRotation(double dt);

private:
	bool _bHasWheels;
	double _channel_mass;
	double _restitution;
	double _spring_mass;
	double _spring_friction;
	double _spring_minimum;
	double _spring_equilibrium;
	double _spring_length;
	double _spring_constant;
	double _separator_distance;
	double _separator_velocity;
	double _springForce;
	double _resting_mass_velocity;
	double _ground_friction_coeff;

	double _dMaxFriction;
	double _normalMass;

	DrivingState _DrivingState;
	Vector3D _velocityBodyGroundFrame;
	Vector3D _velocityBody;

	Vector3D _gravity;
	Vector3D _spring_normal;
	QuarternionD _qBodyOrient;
	QuarternionD _qSteering;
	QuarternionD _qGround;

	double _wheel_angle_position;
	double _wheel_radius;
	double _wheel_linear_velocity;
	double _steering_angle;

	HeightData _heightData;
	Vector3D _forceBody;
};
