#pragma once

#include "JSONRigidBody.h"
#include "JSONRigidBodyBuilder.h"
#include <iostream>

class HeathrowRadarRigidBody : public JSONRigidBody
{
public:
    HeathrowRadarRigidBody()
        : JSONRigidBody("HeathrowRadar")
    {
        setMass(100);
        Matrix3x3D m;
        m.LoadIdentity();
        setInertiaMatrix(m);
        setCG(Vector3D());
        setEuler(0, 0, 0);
        setPosition(GPSLocation(51.473206,-0.454542));

        JSONRigidBodyBuilder body(this);

        try
        {
            body.build(getPath() + "HeathrowTower.body");
        }
        catch( const std::string& str )
        {
            std::cout << str << " : JSON config parse failed";
        }
    }

	void update(double dt)
	{
		_rotation -= 360 * dt/ 4.0f;
		// Empty Implementation as this is static body.
	}

    void drawWindTunnel(double dt)
	{
	}

	virtual void setRecorderHook(FlightRecorder& a) override
	{
		a.addDataRef( _rotation );
	}

	float _rotation;
	Vector3D _rotationPt;
};
