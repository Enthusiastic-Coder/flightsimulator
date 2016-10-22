#pragma once

#include "FlightRecorder.h"
#include "Interfaces.h"
#include "RigidBody.h"

class GSRigidBody : 
    public GPSRigidBodyReferenceFrame<GSRigidBody,RigidBody>,
	//public IRigidBodyAttachable,
    public IDataRecordable<FlightRecorder>
{
public:
	GSRigidBody();
	
	virtual void setRecorderHook(FlightRecorder& a) override;
	virtual void reset() override;
	virtual void update(double dt) override;

    double Height() const override
    {
        return getGPSLocation()._height;
    }

	const Vector3D& gravityBody() const;
	const Vector3D& velocityBody() const;
	Vector3D getTotalBodyVelocity(const Vector3D& pt) const;
	Vector3D getTotalBodyVelocityCG(const Vector3D& pt) const;
	const Vector3D& gravity() const;

protected:
	void updateGravity();

private:
	Vector3D _gravity_body;
	Vector3D _velocity_body;
	Vector3D _gravity;
};


