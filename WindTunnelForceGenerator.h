#pragma once

#define ARROW_L		0.2f

#include <shlwapi.h>
#pragma comment( lib, "shlwapi.lib" )  // needed for the ColorHLSToRGB() function
#include "GSForceGenerator.h"

class AeroForceGenerator;

class WindTunnelForceGenerator : public GSForceGenerator
{
public:
    FORCEGENERATOR_TYPE( Type_Custom )

    WindTunnelForceGenerator();

    void setLeftRightWing( AeroForceGenerator* lAfg , AeroForceGenerator* rAfg );

    AeroForceGenerator* _lAfg;
    AeroForceGenerator* _rAfg;

    void onApplyForce( Particle *p, double dt ) override;
    void drawForceGenerator(GSRigidBody* p, Renderer* args) override;
    void GenerateArrowHead(const Vector3D& p1, const Vector3D& p2, Vector3D out[2]);
    void drawWindTunnelLines(GSRigidBody *pBody, Renderer *r);

protected:
    bool applyDownwash(AeroForceGenerator &afg, Vector3F& position, Vector3D &downwash );

private:
    bool _bPitchRate;
};

