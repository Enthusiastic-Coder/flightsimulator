#pragma once

#include "JSONRigidBodyBuilder.h"
#include "SpringForceGenerator.h"
#include "GravityForceGenerator.h"
#include "WindTunnelForceGenerator.h"
#include "ConstrainedSpringForceGenerator.h"

class AircraftCarrierCustomForceGenerator : public GSForceGenerator
{
public:
	FORCEGENERATOR_TYPE( Type_Custom )

	AircraftCarrierCustomForceGenerator();
    void onApplyForce(Particle *p, double dt) override;
};

class Renderer;

/////////////////////////////////////////////////////////

class AircraftCarrierJSONRigidBody : public JSONRigidBody
{
	friend class AircraftCarrierCustomForceGenerator;
public:
	AircraftCarrierJSONRigidBody(std::string name);

//////////////////////////// Common //////////////////////////////
	std::vector<std::unique_ptr<AircraftEngineForceGenerator>> _engines;

//////////////////////////////////////////////////////////////////


	AircraftCarrierCustomForceGenerator _custom_fg;

	//	PFDView _pfdInstrument;
	
	void update(double dt) override;
    bool onMouseWheel(int wheelDelta) override;
    bool onSyncKeyPress() override;

	float getPower(int engine) override;
	float getPowerOutput(int engine) override;

	bool getHeightFromPosition(const GPSLocation & position, HeightData &heightData) const override;
	void UpdatePlane();

    void renderMesh(Renderer*r, unsigned int shadowMapCount) override;
 
private:
	void initSpeedAndPos();

	Vector3F _orientation;
	Vector3F _angularVelocity;
	double _heightRate;
	QuadPlaneBoundaryT _Planes[3];
};

