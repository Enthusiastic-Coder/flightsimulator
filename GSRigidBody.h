#pragma once

#include "FlightRecorder.h"
#include "Interfaces.h"
#include "RigidBody.h"

class GSRigidBody :
        public RigidBody, public GPSReferenceFrame, public IDataRecordable<FlightRecorder>
	//public IRigidBodyAttachable,
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

    void setPosition( double x, double y, double z)
    {
        _gpsLocation.setFromPosition( Vector3D(x,y,z) );
        RigidBody::setPosition( x,y,z );
    }

    void setPosition( const GPSLocation& gpsLocation )
    {
        _gpsLocation = gpsLocation;
        RigidBody::setPosition( _gpsLocation.position() );
    }

    const GPSLocation& getGPSLocation() const
    {
        return _gpsLocation;
    }

    virtual Vector3D toLocalTranslateFrame( const Vector3D &v) const override
    {
        return toTranslateFrame( true, v );
    }

    virtual Vector3D toNonLocalTranslateFrame( const Vector3D &v ) const override
    {
        return toTranslateFrame( false, v );
    }

protected:
    void updateGravity();

    void resetFrame() override
    {
        GPSReferenceFrame::resetFrame();
        _gpsLocation = GPSLocation();

    }

    void setOrientationHelper(double x, double y, double z) override
    {
        _orientation = getGPSLocation().makeQ( x, y, z );
    }

    void updateEuler() override
    {
        _gpsLocation = position();
        _gpsOrientation = _gpsLocation.makeQ();
        _localOrientation = ~_gpsOrientation * getOrientation();
        _euler = MathSupport<double>::MakeEuler(_localOrientation);
    }

protected:
    Vector3D toTranslateFrame(bool bToLocal, const Vector3D &v ) const
    {
        const QuarternionD& qLocal = getGroundOrientation();
        const QuarternionD& qGps = getQGps();

        if( bToLocal )
            return toFrame( ~qLocal, toFrame( ~qGps, v - position(), - cg() ),  cg() );
        else
            return toFrame( qGps, toFrame( qLocal, v - cg(), cg() ), position() );
    }

private:
        GPSLocation _gpsLocation;

	Vector3D _gravity_body;
	Vector3D _velocity_body;
	Vector3D _gravity;
};


