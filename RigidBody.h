#pragma once

#include <vector3.h>
#include <Matrix3x3.h>

class Particle
{
public:
	Particle();

	void setMass(double mass);
	double getMass() const;
	void setPosition( double x, double y, double z);
	void setPosition( const Vector3D &v );
	void setVelocity( double x, double y, double z );
	void setVelocity( const Vector3D &v );
	const Vector3D& velocity() const;
	const Vector3D& position() const;
	const Vector3D& getForce() const;
	void setForce( const Vector3D& force );
	virtual void applyForce( const Vector3D &r, const Vector3D &f );
	void applyForce(const Vector3D& f);
	virtual void update(double dt);
	virtual void reset();
	virtual bool isParticle();

protected:
	double _mass;
	Vector3D _position;
	Vector3D _velocity;
	Vector3D _force;
};


//////////////////////////////////////////////////////

#include "ReferenceFrame.h"

class RigidBody : 
	public Particle, 
	public RigidBodyReferenceFrame<RigidBody>
{
public:

/// Initial Conditions
	void setInertiaMatrix(const Matrix3x3D& inertia);
	void setAngularVelocity( double x, double y, double z);
	void setTorque( const Vector3D &rhs );

/// Simulation in RealTime
	virtual void reset();
	virtual void applyForce( const Vector3D &r, const Vector3D &f );
	virtual void update(double dt);
////////////////////////////

/// State of Body
	const Vector3D& angularVelocity() const;
	virtual const Vector3D& getTorque() const;

	virtual bool isParticle() override;
	virtual void onToggleFrame(const QuarternionD& q);

private:
	Matrix3x3D _inertia;
	Matrix3x3D _inverse_inertia;

protected:
	Vector3D _torque;
	Vector3D _angular_vel;
};

