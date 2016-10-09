#pragma once

#include "JSONRigidBody.h"
#include "JSONRigidBodyBuilder.h"
#include "AircraftEngineForceGenerator.h"
#include <iostream>


class HeathrowRadarRigidBody : public JSONRigidBody
{
public:
    HeathrowRadarRigidBody();

    void update(double dt) override;

    void drawWindTunnel(double dt);

    virtual void setRecorderHook(FlightRecorder& a) override;

private:
    float _rotation =0.0;
    AircraftEngineForceGenerator _RadarPivot;
};
