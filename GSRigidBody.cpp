#include "stdafx.h"

#include "GsRigidBody.h"

#include "JSONRigidBody.h"
#include "JSONRigidBodyCollection.h"
#include "WorldSystem.h"

GSRigidBody::GSRigidBody()
{
	setPosition(GPSLocation());
	setVelocity(0,0,0);
	setEuler( 0, 0, 0);
	setAngularVelocity( 0, 0, 0 );
	_gravity = Vector3D(0,-9.80665,0);
}

void GSRigidBody::setRecorderHook(FlightRecorder& a)
{
	a.addDataRef( cg() );
	a.addDataRef( _position.get() );
	a.addDataRef( _velocity.get() );
	a.addDataRef( _velocity_body );
	a.addDataRef( _angular_vel.get() );
	a.addDataRef( getGPSLocation() );
}

const Vector3D& GSRigidBody::gravity() const
{
	return _gravity;
}

const Vector3D& GSRigidBody::gravityBody() const
{
	return _gravity_body;
}

const Vector3D& GSRigidBody::velocityBody() const
{
	return _velocity_body;
}

Vector3D GSRigidBody::getTotalBodyVelocity(const Vector3D& pt) const
{
	return getTotalBodyVelocityCG( pt-cg() );
}

Vector3D GSRigidBody::getTotalBodyVelocityCG(const Vector3D& pt) const
{
	return (angularVelocity() ^ pt) + velocityBody();
}

void GSRigidBody::reset()
{
	RigidBody::reset();
	_gravity_body.Reset();
	_velocity_body.Reset();
}

void GSRigidBody::update(double dt)
{
	RigidBody::update(dt);
	_velocity_body = toLocalFrame( _velocity );
	_gravity_body = toLocalFrame( -_gravity.Magnitude() * position().Unit() );
	updateGravity();
}

void GSRigidBody::updateGravity()
{
	//if( getJSONBody() )
		_gravity = Vector3D(0,9.80665,0);
	//VectorD(0,getParent()->getWorld()->GetGravityFromPosition(position()),0);
}


////////////////////////////////////////////////


