#pragma once

#include "JSONRigidBody.h"
#include "JSONRigidBodyBuilder.h"
#include "AircraftEngineForceGenerator.h"
#include <iostream>

class HeathrowRadarForceGenerator : public GSForceGenerator
{
public:
    FORCEGENERATOR_TYPE( Type_Custom )

    void onApplyForce(Particle *p, double dt) override;
};

class HeathrowRadarRigidBody : public JSONRigidBody
{
public:
    HeathrowRadarRigidBody();

    void update(double dt);

    void drawWindTunnel(double dt)
	{
	}

    virtual void setRecorderHook(FlightRecorder& a) override;

private:
    float _rotation =0.0;
    AircraftEngineForceGenerator _RadarPivot;
    HeathrowRadarForceGenerator _custom_fg;
};
