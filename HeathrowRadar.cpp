#include "HeathrowRadar.h"

HeathrowRadarRigidBody::HeathrowRadarRigidBody()
    : JSONRigidBody("HeathrowRadar")
{
    setMass(100);
    Matrix3x3D m;
    m.LoadIdentity();
    setInertiaMatrix(m);
    setCG(Vector3D());
    setEuler(0, 0, 0);
    setPosition(GPSLocation(51.473206,-0.454542));

    addForceGenerator( "RadarPivot", &_RadarPivot, 15 );
    JSONRigidBodyBuilder body(this);

    try
    {
        body.build(getPath() + "HeathrowRadar.body");
    }
    catch( const std::string& str )
    {
        std::cout << str << " : JSON config parse failed";
    }
}

void HeathrowRadarRigidBody::update(double dt)
{
    _rotation -= 360/ 4.0f * dt;
    _RadarPivot.setAngle(_rotation);
}

void HeathrowRadarRigidBody::drawWindTunnel(double dt)
{
}

void HeathrowRadarRigidBody::setRecorderHook(FlightRecorder &a)
{
    a.addDataRef( _rotation );
}
