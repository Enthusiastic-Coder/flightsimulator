
#pragma once

#include "JSONRigidBodyBuilder.h"

class HeathrowTowerRigidBody : public JSONRigidBody
{
public:
	HeathrowTowerRigidBody()
		: JSONRigidBody("HeathrowTower")
	{
		setMass(100);
		Matrix3x3D m;
		m.LoadIdentity();
		setInertiaMatrix(m);
		setCG(Vector3D());
		setEuler(0, 0, 0);
		setPosition(GPSLocation(51.471866, -0.465477,2.0));
	}

	void update(double dt)
	{
		// Empty Implementation as this is static body.

	}
};

class StaticAirplane : public JSONRigidBody
{
public:
	StaticAirplane(std::string name, GPSLocation loc)
		: JSONRigidBody(name)
	{
		setMass(100);
		Matrix3x3D m;
		m.LoadIdentity();
		setInertiaMatrix(m);
		setCG(Vector3D());

		setEuler(0, -90, 0);
		setPosition(loc);
	}

	void update(double dt)
	{
		// Empty Implementation as this is static body.
	}

};