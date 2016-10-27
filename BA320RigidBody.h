#pragma once

#include "JSONRigidBodyBuilder.h"
#include "SpringForceGenerator.h"
#include "GravityForceGenerator.h"
#include "ConstrainedSpringForceGenerator.h"

class BAAirbus320CustomForceGenerator : public GSForceGenerator
{
public:
	FORCEGENERATOR_TYPE( Type_Custom )

	BAAirbus320CustomForceGenerator();
	void onApplyForce(Particle *p, double dt) override;
};

/////////////////////////////////////////////////////////

class BAAirbus320JSONRigidBody : public JSONRigidBody, public WheelConstrainedSpring<0,BAAirbus320JSONRigidBody>
{
public:
	BAAirbus320JSONRigidBody(std::string name);

	BAAirbus320CustomForceGenerator _custom_fg;

	AeroWheelSpringForceGenerator _front_wheel, _left_wheel, _right_wheel;

	SpringForceGenerator _left_engine_under_pod, _right_engine_under_pod, _lower_rear_body;
	SpringForceGenerator _left_wing_horiz, _right_wing_horiz;
	SpringForceGenerator _left_tail_horiz, _right_tail_horiz;
	AircraftEngineForceGenerator _left_engine, _right_engine;

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

	//	PFDView _pfdInstrument;
	
    bool onMouseWheel(int wheelDelta) override;
    bool onSyncKeyPress() override;
    bool onAsyncKeyPress(IScreenMouseInfo* scrn, float dt) override;
    void updateCameraView() override;

    void airResetPos() override;
    void airResetApproachPos() override;
    void airSpoilerToggle(bool bLeft) override;
    void airFlapIncr(int incr) override;
    void applyBrakes(bool bApply) override;

    float getPower(int engine) override;
	float getPowerOutput(int engine) override;
    void setPower(float value) override;

    void applyBrakes(float dt, bool bShift);
 
private:
	void initSpeedAndPos();
	void addForceGenerators();

};

