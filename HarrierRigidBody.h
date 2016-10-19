#pragma once

#include "JSONRigidBodyBuilder.h"
#include "SpringForceGenerator.h"
#include "GravityForceGenerator.h"
#include "WindTunnelForceGenerator.h"
#include "ConstrainedSpringForceGenerator.h"
#include "AircraftEngineNozzleFeeder.h"
#include "AircraftEngineNozzleBleeder.h"

class HarrierCustomForceGenerator : public GSForceGenerator
{
public:
	FORCEGENERATOR_TYPE( Type_Custom )

	HarrierCustomForceGenerator();
	void onApplyForce(Particle *p, double dt) override;
};

/////////////////////////////////////////////////////////

class HarrierJSONRigidBody : public JSONRigidBody, public WheelConstrainedSpring<0,HarrierJSONRigidBody>
{
public:
	HarrierJSONRigidBody(std::string name);

//////////////////////////// Common //////////////////////////////
	std::vector<std::unique_ptr<AircraftEngineForceGenerator>> _engines;
//////////////////////////////////////////////////////////////////

	HarrierCustomForceGenerator _custom_fg;

	AeroWheelSpringForceGenerator _front_wheel, _left_wheel, _middle_wheel, _right_wheel;

	SpringForceGenerator _left_wing_horiz, _right_wing_horiz;
	SpringForceGenerator _left_tail_horiz, _right_tail_horiz;

	SpringForceGenerator _lower_rear_body;
	SpringForceGenerator _above_nose, _above_vtail;
	SpringForceGenerator _front_nose, _rear_body;

	AeroForceGenerator _vertical_tail;
	AeroForceGenerator _left_wing;
	AeroForceGenerator _right_wing;
	AeroForceGenerator _left_tail_wing;
	AeroForceGenerator _right_tail_wing;

	GravityForceGenerator _gravityFG;
	AoaClData _wingAoaData;
	AoaClData _rudderAoaData;
	AoaClData _elevatorAoaData;
	AircraftEngineNozzleFeeder _engineFG;

	AircraftEngineNozzleBleeder _leftWingBleed, _rightWingBleed;
	AircraftEngineNozzleBleeder _frontPitchBleed;
	AircraftEngineNozzleBleeder _rearPitchBleed, _rearYawBleed;

	AircraftEngineNozzleBleeder _leftEngineFrontBleed, _rightEngineFrontBleed;
	AircraftEngineNozzleBleeder _leftEngineBackBleed, _rightEngineBackBleed;


	//	PFDView _pfdInstrument;
	
    bool onMouseWheel(int wheelDelta) override;
    bool onSyncKeyPress() override;
    bool onAsyncKeyPress(IScreenMouseInfo* scrn, float dt) override;
    void updateCameraView() override;

	float getPower(int engine) override;
    void setPower(float value) override;
	float getPowerOutput(int engine) override;
    void onInitialise(WorldSystem*pWorldSystem) override;

protected:
	void setNozzleAngle(float fNozzleAngle);

 
private:
	void initSpeedAndPos();
	void addForceGenerators();

};

