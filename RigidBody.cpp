#include "StdAfx.h"
#include "RigidBody.h"


Particle::Particle() 
	: _mass(0.0f) 
{}

void Particle::setMass(double mass)
{
	_mass = mass;
}

double Particle::getMass() const
{
	return _mass;
}

void Particle::setPosition( double x, double y, double z)
{
	setPosition(Vector3D(x,y,z));
}

void Particle::setPosition( const Vector3D &v )
{
	_position = v;
}

void Particle::setVelocity( double x, double y, double z )
{
	setVelocity( Vector3D(x,y,z) );
}

void Particle::setVelocity( const Vector3D &v )
{
	_velocity = v;
}

const Vector3D& Particle::velocity() const
{
	return _velocity;
}

const Vector3D& Particle::position() const
{
	return _position;
}

const Vector3D& Particle::getForce() const
{
	return _force;
}

void Particle::setForce( const Vector3D& force )
{
	_force = force;
}

void Particle::applyForce( const Vector3D &r, const Vector3D &f )
{
	applyForce( f );
}

void Particle::applyForce(const Vector3D& f)
{
	_force += f;
}

void Particle::update(double dt)
{
	_velocity += _force / _mass * dt;
	_position += _velocity * dt;
}

void Particle::reset()
{
	_force.Reset();
	_velocity.Reset();
}

bool Particle::isParticle()
{
	return true;
}

//////////////////////////////////////////////////////////////////////////////


void RigidBody::setInertiaMatrix(const Matrix3x3D& inertia)
{
	_inertia = inertia;
    _inverse_inertia = _inertia.Inverse();
}

void RigidBody::setAngularVelocity(const Vector3D &v)
{
    setAngularVelocity(v.x, v.y, v.z);
}

void RigidBody::setAngularVelocity( double x, double y, double z)
{
	_angular_vel = Vector3D(x,y,z);
}

void RigidBody::setTorque( const Vector3D &rhs ) 
{ 
	_torque = rhs; 
}

void RigidBody::reset()
{
	Particle::reset();
	_torque.Reset();
	_angular_vel.Reset();
}

const Vector3D& RigidBody::angularVelocity() const
{
	return _angular_vel;
}

void RigidBody::applyForce( const Vector3D &r, const Vector3D &f )
{
	Particle::applyForce(f);
	_torque += r ^ f;
}

bool RigidBody::isParticle() 
{	
	return false;
}

void RigidBody::onToggleFrame(const QuarternionD& q)
{
	_force = QVRotate( q, _force );
	_torque = QVRotate( q, _torque );
}

const Vector3D& RigidBody::getTorque() const
{
	return _torque;
}

void RigidBody::update(double dt)
{
	setWorldFrame();
	Particle::update(dt);
	setBodyFrame();

	double MAX_FORCE = 8e8;

	MAX_FORCE = 1e8; // Configurable setting per Body

	if( fabs(_torque.x) > MAX_FORCE || fabs(_torque.y) > MAX_FORCE || fabs(_torque.z) > MAX_FORCE)
		_torque = _torque.Unit() * MAX_FORCE;

	_angular_vel += (_inverse_inertia * ( _torque - (_angular_vel  ^ (_inertia * _angular_vel ) ) ) ) * dt;

	if( fabs(_angular_vel.x) > MAX_FORCE || fabs(_angular_vel.y) > MAX_FORCE || fabs(_angular_vel.z) > MAX_FORCE)
		_angular_vel = _angular_vel.Unit() * MAX_FORCE;

	setOrientation( getOrientation() + getOrientation() * _angular_vel * dt / 2.0);
}
