#pragma once

#include "JSONRigidBodyBuilder.h"

#include "WindTunnelForceGenerator.h"
#include "SpringForceGenerator.h"
#include "GravityForceGenerator.h"
#include "ConstrainedSpringForceGenerator.h"

class BAAirbus380CustomForceGenerator : public GSForceGenerator
{
public:
	FORCEGENERATOR_TYPE( Type_Custom )

	BAAirbus380CustomForceGenerator();
	void onApplyForce(Particle *p, double dt) override;
};

/////////////////////////////////////////////////////////

class BAAirbus380JSONRigidBody : public JSONRigidBody, public WheelConstrainedSpring<0,BAAirbus380JSONRigidBody>
{
public:
	BAAirbus380JSONRigidBody(std::string name);

	WindTunnelForceGenerator _windTunnel;

	BAAirbus380CustomForceGenerator _custom_fg;

	AeroWheelSpringForceGenerator	_front_wheel,
		_left_front_wheel,
		_left_back_wheel,
		_right_front_wheel,
		_right_back_wheel;

	SpringForceGenerator _under_engine_pod[4];

	SpringForceGenerator _front_nose, _rear_body;

	SpringForceGenerator _above_nose, _above_vtail;

	SpringForceGenerator _lower_rear_body;
	SpringForceGenerator _left_wing_horiz, _right_wing_horiz;
	SpringForceGenerator _left_tail_horiz, _right_tail_horiz;

	AircraftEngineForceGenerator _engine[4];

	AeroForceGenerator _vertical_tail;
	AeroForceGenerator _left_wing;
	AeroForceGenerator _right_wing;
	AeroForceGenerator _left_tail_wing;
	AeroForceGenerator _right_tail_wing;

	GravityForceGenerator _gravityFG;
	AoaClData _wingAoaData;
	AoaClData _rudderAoaData;
	AoaClData _elevatorAoaData;

	//	PFDView _pfdInstrument;

    bool onMouseWheel(int wheelDelta) override;
    bool onSyncKeyPress() override;
    bool onAsyncKeyPress(IScreenMouseInfo* scrn, float dt) override;
    void updateCameraView() override;

    float getPower(int engine) override;
	float getPowerOutput(int engine) override;
 
private:
	void initSpeedAndPos();
	void addForceGenerators();

};

