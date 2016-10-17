#include "stdafx.h"
#include "HarrierRigidBody.h"
#include "JSONRigidBodyBuilder.h"
#include <iostream>
#include "WorldSystem.h"

HarrierCustomForceGenerator::HarrierCustomForceGenerator()  {}

void HarrierCustomForceGenerator::onApplyForce(Particle *p, double dt)
{	
	float goodForce = 50000.0f;
	JSONRigidBody& body = *reinterpret_cast<JSONRigidBody*>(p);

    if (getWorld()->focusedRigidBody() != p)
		return;

	body.setBodyFrame();

	auto *pFW = body.forceGenerator( std::string("front_wheel") );
	auto *pLW = body.forceGenerator( std::string("left_wheel") );
	auto *pRW = body.forceGenerator( std::string("right_wheel") );

	// Timing
	// debug 0.05 ms  60 queries a millisecond
	// release 0.01 ms 300 queries a millisecond.

	if( GetAsyncKeyState( 'L' ) < 0 )
        body.applyForce( Vector3D(0,0,0), body.toLocalGroundFrame(Vector3D(0, goodForce*3 ,0)));

	if( GetAsyncKeyState( 223 ) < 0 )
        body.applyForce( Vector3D(0,0,-5), Vector3D(0,goodForce/2,0));

	if( GetAsyncKeyState( '1' ) < 0 )
        body.applyForce(Vector3D(-5,0,0), Vector3D(0, goodForce/2, 0));

		//body.applyForce( pLW->getContactPointCG(body.cg()), VectorD(0,goodForce,0));

	if( GetAsyncKeyState( '2' ) < 0 )
        body.applyForce(Vector3D(5, 0, 0), Vector3D(0,goodForce/2,0));

	if( GetAsyncKeyState( 'J' ) < 0 )
        body.applyForce( Vector3D(0,8.74f,18.25f), Vector3D(goodForce, 0 ,0)/4.0);

	if( GetAsyncKeyState( 'K' ) < 0 )
        body.applyForce( Vector3D(0,8.74f,18.25f), Vector3D(-goodForce, 0 ,0)/4.0);

	if( GetAsyncKeyState( 'I') < 0 )
	{
        body.applyForce(pFW->getContactPointCG(body.cg()), Vector3D(0, 0,-goodForce));
	}

	if( GetAsyncKeyState( 'M' ) < 0 )
	{
        body.applyForce( Vector3D(-3.74f,0,0.32f), Vector3D(0, 0, goodForce));
        body.applyForce( Vector3D(3.74f,0,0.32f), Vector3D(0, 0, goodForce));
	}
}


HarrierJSONRigidBody::HarrierJSONRigidBody(std::string name) :
	JSONRigidBody(name),
    //_windTunnel(_left_wing,_right_wing),
	_vertical_tail(&_rudderAoaData),
	_left_wing(&_wingAoaData,true),
	_right_wing(&_wingAoaData, true),
	_left_tail_wing(&_elevatorAoaData),
	_right_tail_wing(&_elevatorAoaData)
{
	setMass(8000);
}

void HarrierJSONRigidBody::onInitialise(WorldSystem* pWorldSystem)
{
	addForceGenerators();

    JSONRigidBody::onInitialise(pWorldSystem);


#pragma region CONTACTS CONFIGURATION

	//float mTest = getMassChannel().GetMass(VectorF(0, 0.3f, -1.9));

	_front_wheel.spring().setChannelMass(4348.17f);
    _front_wheel.setContactPoint(Vector3D(0, 0.35f, -1.9));

	_front_wheel.spring().setMinEqLen(0.1f, 0.2f, 0.2);

	_front_wheel.spring().setRestitution(0.1f);

	_front_wheel.spring().setSpringFriction(3500.0f * 4);
	_front_wheel.spring().setSpringMass(300.0f);
	_front_wheel.spring().setContactFrictionCoefficient(20.0);
	_front_wheel.spring().calculateSpringConstant();
	_front_wheel.spring().setWheelRadius(0.32f);

	_left_wheel.spring().setChannelMass(883.546f);
    _left_wheel.setContactPoint(Vector3D(-2.5f, 0.6f, 1.45f));

	_left_wheel.spring().setMinEqLen(0.1f, 0.2f, 0.2f);

	_left_wheel.spring().setRestitution(0.1f);
	_left_wheel.spring().setSpringFriction(3500.0f);
	_left_wheel.spring().setSpringMass(30.0f);
	_left_wheel.spring().setContactFrictionCoefficient(10.0);
	_left_wheel.spring().setRestitution(0.1f);
	_left_wheel.spring().calculateSpringConstant();
	_left_wheel.spring().setWheelRadius(0.167f);

	//mTest = getMassChannel().GetMass(VectorF(0.0, 0.6f, 1.43f));

	_middle_wheel.spring().setChannelMass(1884.74f);
    _middle_wheel.setContactPoint(Vector3D(0.0, 0.6f, 1.41f));

	_middle_wheel.spring().setMinEqLen(0.1f, 0.2f, 0.2f);

	_middle_wheel.spring().setRestitution(0.1f);
	_middle_wheel.spring().setSpringFriction(1500.0f * 4);
	_middle_wheel.spring().setSpringMass(300.0f);
	_middle_wheel.spring().setContactFrictionCoefficient(30.0);
	_middle_wheel.spring().setRestitution(0.8f);
	_middle_wheel.spring().calculateSpringConstant();
	_middle_wheel.spring().setWheelRadius(0.3377f);

	_right_wheel.spring().setChannelMass(883.546f);
    _right_wheel.setContactPoint(Vector3D(2.5f, 0.6f, 1.45f));

	_right_wheel.spring().setMinEqLen(0.1, 0.2, 0.2);

	_right_wheel.spring().setRestitution(0.1f);
	_right_wheel.spring().setSpringFriction(1500.0f);
	_right_wheel.spring().setSpringMass(30.0f);
	_right_wheel.spring().setContactFrictionCoefficient(10.0);
	_right_wheel.spring().setRestitution(0.1f);

	_right_wheel.spring().calculateSpringConstant();
	_right_wheel.spring().setWheelRadius(0.167f);

	//////////////////////////////////////////////
	// Rear Body -(done)
	_lower_rear_body.spring().setChannelMass(getMass() / 4);
    _lower_rear_body.setContactPoint(Vector3D(0.0f, 1.27f + 0.3f, 6.0f));
	_lower_rear_body.spring().setMinEqLen(0.1f, 0.2f, 0.2);
	_lower_rear_body.spring().setSpringMass(30.0f);
	_lower_rear_body.spring().setContactFrictionCoefficient(20.0f);
	_lower_rear_body.spring().calculateSpringConstant();

	//////////////////////////////////////////
	float m = getMass() / 20 * 20 / 4;
	_left_wing_horiz.spring().setChannelMass(m);
    _left_wing_horiz.setContactPoint(Vector3D(-4.36, 1.42f, 2.13f));
	_left_wing_horiz.spring().setMinEqLen(0.1f, 0.2f, 0.2);
	_left_wing_horiz.spring().setSpringMass(300.0f);
	_left_wing_horiz.spring().setContactFrictionCoefficient(20.0f);
	_left_wing_horiz.spring().calculateSpringConstant();

	_right_wing_horiz.spring().setChannelMass(m);
    _right_wing_horiz.setContactPoint(Vector3D(4.36, 1.42f, 2.13f));
	_right_wing_horiz.spring().setMinEqLen(0.1f, 0.2f, 0.2);
	_right_wing_horiz.spring().setSpringMass(300.0f);
	_right_wing_horiz.spring().setContactFrictionCoefficient(20.0f);
	_right_wing_horiz.spring().calculateSpringConstant();

	//////////////////////////////////////////
	_left_tail_horiz.spring().setChannelMass(getMass() / 8);
    _left_tail_horiz.setContactPoint(Vector3D(-2.12f, 1.8f, 6.48f));
	_left_tail_horiz.spring().setMinEqLen(0.1f, 0.2f, 0.2);
	_left_tail_horiz.spring().setSpringMass(300.0f);
	_left_tail_horiz.spring().setContactFrictionCoefficient(20.0f);
	_left_tail_horiz.spring().calculateSpringConstant();

	_right_tail_horiz.spring().setChannelMass(getMass() / 8);
    _right_tail_horiz.setContactPoint(Vector3D(2.12f, 1.8f, 6.48f));
	_right_tail_horiz.spring().setMinEqLen(0.1f, 0.2f, 0.2);
	_right_tail_horiz.spring().setSpringMass(300.0f);
	_right_tail_horiz.spring().setContactFrictionCoefficient(20.0f);
	_right_tail_horiz.spring().calculateSpringConstant();

	//////////////////////////////////////////

	_above_nose.spring().setChannelMass(getMass() / 4.0f);
    _above_nose.setContactPoint(Vector3D(0.0f, 3.10f, -3.37f));
	_above_nose.spring().setMinEqLen(0.1f, 0.2f, 0.2);
	_above_nose.spring().setSpringMass(300.0f);
	_above_nose.spring().setContactFrictionCoefficient(20.0f);
	_above_nose.spring().calculateSpringConstant();

	_front_nose.spring().setChannelMass(getMass() / 8);
    _front_nose.setContactPoint(Vector3D(0.0f, 1.75f, -6.4f));
	_front_nose.spring().setMinEqLen(0.1f, 0.2f, 0.2);
	_front_nose.spring().setSpringMass(300.0f);
	_front_nose.spring().setContactFrictionCoefficient(20.0f);
	_front_nose.spring().calculateSpringConstant();

	_rear_body.spring().setChannelMass(getMass() / 8);
    _rear_body.setContactPoint(Vector3D(0.0f, 2.36f, 7.04f));
	_rear_body.spring().setMinEqLen(0.1f, 0.2f, 0.2);
	_rear_body.spring().setSpringMass(300.0f);
	_rear_body.spring().setContactFrictionCoefficient(20.0f);
	_rear_body.spring().calculateSpringConstant();

	_above_vtail.spring().setChannelMass(getMass() / 6.0f);
    _above_vtail.setContactPoint(Vector3D(0.0f, 4.3f, 6.62f));
	_above_vtail.spring().setMinEqLen(0.1f, 0.2f, 0.2);
	_above_vtail.spring().setSpringMass(300);
	_above_vtail.spring().setContactFrictionCoefficient(20.0f);
	_above_vtail.spring().calculateSpringConstant();

	_engineFG.attachBleeder(&_leftWingBleed);
	_engineFG.attachBleeder(&_rightWingBleed);
	_engineFG.attachBleeder(&_frontPitchBleed);
	_engineFG.attachBleeder(&_rearPitchBleed);
	_engineFG.attachBleeder(&_rearYawBleed);

	_engineFG.attachBleeder(&_leftEngineFrontBleed);
	_engineFG.attachBleeder(&_rightEngineFrontBleed);
	_engineFG.attachBleeder(&_leftEngineBackBleed);
	_engineFG.attachBleeder(&_rightEngineBackBleed);


    _leftWingBleed.setContactPoint(Vector3D(-4.34, 1.58, 2.13));
	_leftWingBleed.setDirection(-90, 0, 0);

    _rightWingBleed.setContactPoint(Vector3D(4.34, 1.58, 2.13));
	_rightWingBleed.setDirection(-90, 0, 0);
	
    _frontPitchBleed.setContactPoint(Vector3D(0, 1.51, -5.90));
	_frontPitchBleed.setDirection(-90, 0, 0);
	
    _rearPitchBleed.setContactPoint(Vector3D(0, 2.37, 6.82));
	_rearPitchBleed.setDirection(-90, 0, 0);
	
    _rearYawBleed.setContactPoint(Vector3D(0, 2.37, 6.82));
	_rearYawBleed.setDirection(0, 90, 0);

    _leftEngineFrontBleed.setContactPoint(Vector3D(-1.13, 1.92, -0.91));
	_leftEngineFrontBleed.setDirection(-180, 0, 0);

    _rightEngineFrontBleed.setContactPoint(Vector3D(1.13, 1.92, -0.91));
	_rightEngineFrontBleed.setDirection(-180, 0, 0);

    _leftEngineBackBleed.setContactPoint(Vector3D(-0.91, 1.75, 0.94));
	_leftEngineBackBleed.setDirection(-180, 0, 0);

    _rightEngineBackBleed.setContactPoint(Vector3D(0.91, 1.75, 0.94));
	_rightEngineBackBleed.setDirection(-180, 0, 0);

///////////////////////////////////////// Bleed system sin(THETA)

	setNozzleAngle(90.0f);

	_leftEngineFrontBleed.setThrustPercentage(0.25f);
	_rightEngineFrontBleed.setThrustPercentage(0.33333f);
	_leftEngineBackBleed.setThrustPercentage(0.5f);
	_rightEngineBackBleed.setThrustPercentage(1.0f);

///////////////////////////////////////// 


	_front_wheel.spring().setDrivingState(SpringModel::DrivingState::NEUTRAL);
#pragma endregion

	initSpeedAndPos();
}

void HarrierJSONRigidBody::setNozzleAngle(float fNozzleAngle)
{
    fNozzleAngle = std::max(std::min(100.0f, fNozzleAngle), 0.0f);
	_leftEngineFrontBleed.setDirection(-fNozzleAngle, 0, 0);
	_rightEngineFrontBleed.setDirection(-fNozzleAngle, 0, 0);
	_leftEngineBackBleed.setDirection(-fNozzleAngle, 0, 0);
	_rightEngineBackBleed.setDirection(-fNozzleAngle, 0, 0);
    _engineFG.setEnergy(sin(std::min(90.0f, fNozzleAngle) / 180.0f * M_PI));
}

bool HarrierJSONRigidBody::onMouseWheel(int wheelDelta)
{
	//for (size_t i = 0; i < _engines.size(); ++i)
        //_engines[i]->_thrust_percent += wheelDelta;
    _engineFG._thrust_percent += wheelDelta;
	return true;
}

float HarrierJSONRigidBody::getPower(int engine)
{
	//if (engine < 0 || engine >= _engines.size())
	//	return 0.0f;

	//return _engines[engine]->_thrust_percent;
    return _engineFG._thrust_percent;
}

void HarrierJSONRigidBody::setPower(float value)
{
    _engineFG._thrust_percent = value;
}

float HarrierJSONRigidBody::getPowerOutput(int engine)
{
	//if (engine < 0 || engine >= _engines.size())
		//return 0.0f;

	//return _engines[engine]->_actual_thrust_percent;
	return _engineFG._actual_thrust_percent;
}

bool HarrierJSONRigidBody::onSyncKeyPress()
{
    for( int i=0 ; i <10; ++i)
        if(GetKeyState('0'+i) < 0)
        {
            setNozzleAngle(i*10.0f);
            return true;
        }

    if( GetKeyState(VK_F5) < 0) //Reset
	{
		reset();
		initSpeedAndPos();
		return true;
	}

    if( GetKeyState(VK_F7) < 0 )
	{
		double fMilesOut = 3.5f;
		reset();

        setPosition(GPSLocation(51.4648,-0.4719087,fMilesOut * 350/3.2808) + Vector3D( -fMilesOut  * 5280 /3.2808f, 0, rand()%500-250));
		setEuler( 0, 90, 0);

		double dSpeed = 160 * 1.15f * 1.609334f / 3600 * 1000 ;
        setVelocity(toNonLocalFrame( Vector3D(0, 0, -dSpeed) ) );
		setAngularVelocity( 0, 0, 0 );
		return true;
	}

    if( GetKeyState(VK_F1) < 0 || GetKeyState(VK_F2) < 0 ) //Flaps
	{
		AeroControlSurface* pInnerML = _left_wing.element(1)->controlSurfaceN(0);
		AeroControlSurface* pInnerMR = _right_wing.element(1)->controlSurfaceN(0);

		float fDeflection = hydraulics().getDeflection(pInnerML);

        if( GetKeyState(VK_F1) < 0 )
		{
			fDeflection -= 5.0f;
			if (fDeflection <= 0.0f)
				fDeflection = 0.0f;
		}
		else
		{
			fDeflection += 5.0f;
			if (fDeflection >= 40.0f)
				fDeflection = 40.0f;
		}

		hydraulics().setDeflection( pInnerML, fDeflection  );
		hydraulics().setDeflection( pInnerMR, fDeflection  );

		return true;
	}

    return false;
}

bool HarrierJSONRigidBody::onAsyncKeyPress(IScreenMouseInfo* scrn, float dt)
{
    IJoystick *joy = _custom_fg.getJoystick();

    double max_decel = 6.0f;
    double max_accel = 6.0f;
    max_decel = 12.0f;

    if( joy && joy->isAvailable() )
    {
            float fAileron(0.0f);
            float fPitch(0.0f);
            float fYaw(0.0f);
            float fThrust(0.0f);
            fAileron = joy->joyGetZ();
            fPitch = joy->joyGetR();
            fYaw = joy->joyGetX();
            fThrust = std::fabs(joy->joyGetY()) > 0.2f ? joy->joyGetY() : 0.0f;
            _engineFG._thrust_percent -= fThrust;

            fThrust = _engineFG._thrust_percent;

            float fSpeedFactor = 1.0f - std::min(1.0,fabs(velocityBody().z/50.0f));

            _front_wheel.spring().setSteeringAngle(fYaw * 20.0f);
            hydraulics().setDeflection(_vertical_tail.element(0)->controlSurfaceN(0), fYaw * 15.0f *fSpeedFactor );
            _engineFG._thrust_percent = fThrust;

            hydraulics().setDeflection( _left_wing.element(0)->controlSurfaceN(0), fAileron * 5.0f );
            hydraulics().setDeflection( _right_wing.element(0)->controlSurfaceN(0), -fAileron * 5.0f );


//            float lV = hydraulics().getDeflection(_left_tail_wing.controlSurface0());
//            float rV = hydraulics().getDeflection(_right_tail_wing.controlSurface0());

//            lV -= dt * 20 * fPitch;
//            rV -= dt * 20 * fPitch;

//            if( lV < -25) lV = -25;
//            if( lV > 25) lV = 25;

//            if( rV < -25) rV = -25;
//            if( rV > 25) rV = 25;


            //hydraulics().setDeflection( _left_tail_wing.controlSurface0(),lV );
            //hydraulics().setDeflection( _right_tail_wing.controlSurface0(),rV );

            hydraulics().setDeflection( _left_tail_wing.controlSurface0(), -fPitch * 20.0f );
            hydraulics().setDeflection( _right_tail_wing.controlSurface0(), -fPitch * 20.0f );

        float fReqAng = fAileron * 4 * M_PI / 180.0f;
        float fDestAng = 0.0f;

        if (fabs(fAileron) <= 0.05)
        {
            fDestAng = std::min(fabs(4 * M_PI / 180.0f), fabs(angularVelocity().z / 4.0f/4.0f));

            if (angularVelocity().z > 0.0f)
                fDestAng = -fDestAng;
        }
        else
        {
            float fDiff = fReqAng - angularVelocity().z;
            fDestAng = std::min(fabs(fReqAng), fabs(fDiff));
            if (fReqAng > 0.0f)
                fDestAng = -fDestAng;
        }

        _leftWingBleed.setDw(Vector3F(0, 0, fDestAng / 2.0f));
        _rightWingBleed.setDw(Vector3F(0, 0, fDestAng / 2.0f));


        fReqAng = fPitch * 4 * M_PI / 180.0f;
        float fDiff = fReqAng - angularVelocity().x;
        fDestAng = std::min(fabs(fReqAng), fabs(fDiff));

        if (fabs(fPitch) <= 0.05)
        {
            fDestAng = std::min(2 * M_PI / 180.0f, fabs(angularVelocity().x / 4.0f));

            if (angularVelocity().x < 0.0f)
            {
                _frontPitchBleed.setDw(Vector3F(fDestAng, 0, 0));
                _rearPitchBleed.setDw(Vector3F());
            }
            else
            {
                _frontPitchBleed.setDw(Vector3F());
                _rearPitchBleed.setDw(Vector3F(-fDestAng, 0, 0));
            }
        }
        else if (fPitch > 0.0f)
        {
            _frontPitchBleed.setDw(Vector3F(fDestAng, 0, 0));
            _rearPitchBleed.setDw(Vector3F());
        }
        else
        {
            _frontPitchBleed.setDw(Vector3F());
            _rearPitchBleed.setDw(Vector3F(-fDestAng, 0, 0));
        }


        if (fabs(fYaw) <= 0.05)
        {
            fDestAng = std::min(2 * M_PI / 180.0f, fabs(angularVelocity().y / 4.0f));

            if (angularVelocity().y > 0.0f)
                fDestAng = -fDestAng;

            _rearYawBleed.setDw(Vector3F(0, fDestAng, 0));
        }
        else
        {
            fReqAng = fYaw * 4 * M_PI / 180.0f;
            fDiff = fReqAng - angularVelocity().y;
            fDestAng = std::min(fabs(fReqAng), fabs(fDiff));

            if (fYaw > 0)
                fDestAng = -fDestAng;

            _rearYawBleed.setDw(Vector3F(0, fDestAng, 0));
        }

        //if( _right_engine._thrust_percent > 100 ) _right_engine._thrust_percent  = 100;
        //if( _right_engine._thrust_percent < 0 ) _right_engine._thrust_percent = 0;

        //_left_engine._thrust_percent = _right_engine._thrust_percent;

        DWORD dwWord = joy->buttonFlagPressed();

        //128(down) - 32 (up)
        if( dwWord & 8 || dwWord & 4 )
        {
            float diff = (dwWord&4? 1 : -1) * 0.5f;
            float MAX_DEFL = 15;
            float deflection_l = _left_tail_wing.controlSurface0()->getDeflection();
            if( deflection_l < -MAX_DEFL )	deflection_l = -MAX_DEFL;
            if( deflection_l > MAX_DEFL )	deflection_l = MAX_DEFL;
            hydraulics().setDeflection(_left_tail_wing.controlSurface0(), deflection_l+diff );

            float deflection_r = _right_tail_wing.controlSurface0()->getDeflection();
            if( deflection_r < -MAX_DEFL )	deflection_r = -MAX_DEFL;
            if( deflection_r > MAX_DEFL )	deflection_r  = MAX_DEFL;
            hydraulics().setDeflection( _right_tail_wing.controlSurface0(), deflection_r+diff );
        }

        if( dwWord & 2 ) //Regular Braking.
        {
            if( _front_wheel.spring().getWheelSpeed()  > 0 )
                _front_wheel.spring().incrWheelSpeed( - std::min( max_decel * dt, _front_wheel.spring().getWheelSpeed() ));
            else
                _front_wheel.spring().incrWheelSpeed(std::min( max_decel * dt, (-_front_wheel.spring().getWheelSpeed()) ));

            if( _left_wheel.spring().getWheelSpeed()  > 0 )
                _left_wheel.spring().incrWheelSpeed( - std::min( max_decel * dt, _left_wheel.spring().getWheelSpeed() ));
            else
                _left_wheel.spring().incrWheelSpeed( std::min( max_decel * dt, (-_left_wheel.spring().getWheelSpeed()) ));

            if( _right_wheel.spring().getWheelSpeed()  > 0 )
                _right_wheel.spring().incrWheelSpeed(- std::min( max_decel * dt, _right_wheel.spring().getWheelSpeed() ));
            else
                _right_wheel.spring().incrWheelSpeed( std::min( max_decel * dt, (-_right_wheel.spring().getWheelSpeed()) ));
        }
    }

    if( GetAsyncKeyState( '3' ) < 0 )
    {
        if( GetAsyncKeyState( VK_CONTROL ) == 0 )
        {
            float deflection_l = _left_tail_wing.element(0)->controlSurfaceN(0)->getDeflection();
            deflection_l -= dt * 30;
            if( deflection_l < -25 )
                deflection_l = -25;
            _left_tail_wing.element(0)->controlSurfaceN(0)->setDeflection( deflection_l);

            float deflection_r = _right_tail_wing.element(0)->controlSurfaceN(0)->getDeflection();
            deflection_r -= dt * 30;
            if( deflection_r < -25 )
                deflection_r = -25;
            _right_tail_wing.element(0)->controlSurfaceN(0)->setDeflection( deflection_r);
        }
        else
        {
            float MAX_DEFL = 10;
            float deflection_l = _left_tail_wing.controlSurface0()->getDeflection();
            deflection_l -= dt * 30;
            if( deflection_l < -MAX_DEFL )
                deflection_l = -MAX_DEFL;
            _left_tail_wing.controlSurface0()->setDeflection( deflection_l);

            float deflection_r = _right_tail_wing.controlSurface0()->getDeflection();
            deflection_r -= dt * 30;
            if( deflection_r < -MAX_DEFL )
                deflection_r = -MAX_DEFL;
            _right_tail_wing.controlSurface0()->setDeflection( deflection_r);
        }
    }

    if( GetAsyncKeyState( '4' ) < 0 )
    {
        if( GetAsyncKeyState( VK_CONTROL ) == 0 )
        {
            float deflection_l = _left_tail_wing.element(0)->controlSurfaceN(0)->getDeflection();
            deflection_l += dt * 30;
            if( deflection_l > 25 )
                deflection_l = 25;
            _left_tail_wing.element(0)->controlSurfaceN(0)->setDeflection( deflection_l);

            float deflection_r = _right_tail_wing.element(0)->controlSurfaceN(0)->getDeflection();
            deflection_r += dt * 30;
            if( deflection_r > 25 )
                deflection_r = 25;
            _right_tail_wing.element(0)->controlSurfaceN(0)->setDeflection( deflection_r);
        }
        else
        {
            float MAX_DEFL = 10;
            float deflection_l = _left_tail_wing.controlSurface0()->getDeflection();
            deflection_l += dt * 30;
            if( deflection_l > MAX_DEFL )
                deflection_l = MAX_DEFL;
            _left_tail_wing.controlSurface0()->setDeflection( deflection_l);

            float deflection_r = _right_tail_wing.controlSurface0()->getDeflection();
            deflection_r += dt * 30;
            if( deflection_r > MAX_DEFL )
                deflection_r = MAX_DEFL;
            _right_tail_wing.controlSurface0()->setDeflection( deflection_r);
        }
    }

    if( _front_wheel.spring().getSteeringAngle() < -75 )	_front_wheel.spring().setSteeringAngle( -75);
    if( _front_wheel.spring().getSteeringAngle() > 75 )	_front_wheel.spring().setSteeringAngle(75);

    if( GetAsyncKeyState( 'B' ) < 0 )
    {
        if( GetAsyncKeyState( VK_SHIFT ) < 0 )
        {
            _front_wheel.spring().setDrivingState( SpringModel::DrivingState::BRAKE );
            _left_wheel.spring().setDrivingState( SpringModel::DrivingState::BRAKE );
            _right_wheel.spring().setDrivingState( SpringModel::DrivingState::BRAKE );
            _middle_wheel.spring().setDrivingState( SpringModel::DrivingState::BRAKE );
        }
        else
        {
            _front_wheel.spring().setDrivingState( SpringModel::DrivingState::NEUTRAL );
            _left_wheel.spring().setDrivingState( SpringModel::DrivingState::NEUTRAL );
            _right_wheel.spring().setDrivingState( SpringModel::DrivingState::NEUTRAL );
            _middle_wheel.spring().setDrivingState( SpringModel::DrivingState::NEUTRAL );

            if (_front_wheel.spring().getWheelSpeed()  > 0)
                _front_wheel.spring().incrWheelSpeed( -std::min(max_decel * dt, _front_wheel.spring().getWheelSpeed()));
            else
                _front_wheel.spring().incrWheelSpeed( std::min(max_decel * dt, (-_front_wheel.spring().getWheelSpeed())));

            if (_middle_wheel.spring().getWheelSpeed()  > 0)
                _middle_wheel.spring().incrWheelSpeed( - std::min(max_decel * dt, _middle_wheel.spring().getWheelSpeed()));
            else
                _middle_wheel.spring().incrWheelSpeed( std::min(max_decel * dt, (-_middle_wheel.spring().getWheelSpeed())));

            if( _left_wheel.spring().getWheelSpeed()  > 0 )
                _left_wheel.spring().incrWheelSpeed( - std::min( max_decel * dt, _left_wheel.spring().getWheelSpeed() ));
            else
                _left_wheel.spring().incrWheelSpeed(std::min( max_decel * dt, (-_left_wheel.spring().getWheelSpeed()) ));

            if( _right_wheel.spring().getWheelSpeed()  > 0 )
                _right_wheel.spring().incrWheelSpeed(- std::min( max_decel * dt, _right_wheel.spring().getWheelSpeed() ));
            else
                _right_wheel.spring().incrWheelSpeed(std::min( max_decel * dt, (-_right_wheel.spring().getWheelSpeed()) ));
        }
    }

    _engineFG._thrust_percent = std::min(std::max(0.0f,_engineFG._thrust_percent),100.0f);

    if( isUsingMouse() )
    {
        int x, y;
        scrn->GetMousePos(x, y);

        int width, height;
        scrn->GetScreenDims(width, height);

        int mx = width / 2;
        int my = height / 2;

        float dx = (x - mx) / 12.0f;
        float dy = (y - my) / 24.0f;

        if( dx < -25 ) dx = -25;
        if( dx > 25 ) dx = 25;
        if( dy < -15 ) dy = -15;
        if( dy > 15 ) dy = 15;

        hydraulics().setDeflection( _left_wing.element(0)->controlSurfaceN(0), dx/4);
        hydraulics().setDeflection(_right_wing.element(0)->controlSurfaceN(0), -dx / 4);
        hydraulics().setDeflection( _left_tail_wing.controlSurface0(), -dy );
        hydraulics().setDeflection( _right_tail_wing.controlSurface0(), -dy );

        dx = (x - mx) / 10.0f;

        float aeroFrac = std::min(pow(velocity().Magnitude() - 55, 2) / (55.0f * 55), 1.0);

        _front_wheel.spring().setSteeringAngle(dx * aeroFrac);

        if( dx < -25 ) dx = -25;
        if( dx > 25 ) dx = 25;

        hydraulics().setDeflection( _vertical_tail.element(0)->controlSurfaceN(0), dx * aeroFrac);
    }

    return true;
}

void HarrierJSONRigidBody::updateCameraView()
{
    JSONRigidBody::updateCameraView();

    switch((ViewPosition)getCameraProvider()->curViewIdx())
    {
    case CockpitForwards :
        getCameraProvider()->getCameraView()->setPosition(toNonLocalTranslateFrame(Vector3D(0, 2.67,-4.38)));
        break;
    case PassengerLeftMiddle :
        break;
    case RightGearWheel :
        break;
    }
}

void HarrierJSONRigidBody::initSpeedAndPos()
{
	double deg = 5+20;
	double deg2 = 0;
	double ang = DegreesToRadians(deg2);
	double speed = 70.0f/5;

	setVelocity( -speed*sin(ang), 0, -speed*cos(ang) );
	setPosition( 0, 120, 40 );

	setPosition( GPSLocation() );
	setEuler( 0, deg, 0 );

	setVelocity(0,0,0);
	setAngularVelocity( 0, 0, 0 );

	setPosition(GPSLocation(51.4648,-0.4719087,22));
	setVelocity(0,0,0);
	setEuler( 5,90,-0.1);
	
#if defined LOCATED_AT_GIBRALTER
	
	setPosition( GPSLocation(36.151473, -5.339665,25));
	//setVelocity(150, 0, 0);

    setPosition(GPSLocation(36.151473, -5.339665, 50) + Vector3F(1000, 10000*0, 0));
	setEuler(0, 270, 0);
#else
	setPosition( GPSLocation(51.464951, -0.434115,22));
#endif
} 

void HarrierJSONRigidBody::addForceGenerators()
{
    std::string path = getPath();

	if( !_wingAoaData.load( path + "main root.afl" ) )
        std::cout << "Failed to load WingData : Aerofoil Loading";

	if( !_rudderAoaData.load( path + "vert.afl" ) )
        std::cout << "Failed to load vert.afl : Aerofoil Loading";

	if( !_elevatorAoaData.load( path + "horiz root.afl" ) )
        std::cout << "Failed to load horiz root.afl : Aerofoil Loading";

	addForceGenerator( "gravityFG", &_gravityFG, 15 );

	for (size_t i = 0; i < _engines.size(); ++i)
	{
		std::stringstream ss;
		ss << "engine_" << i;
		addForceGenerator(ss.str(), &*_engines[i]);
	}

	addForceGenerator( "vertical_tail", &_vertical_tail, 15 );

	addForceGenerator( "left_wing", &_left_wing, 15 );
	addForceGenerator( "right_wing", &_right_wing, 15 );

	addForceGenerator( "left_tail_wing", &_left_tail_wing, 15 );
	addForceGenerator( "right_tail_wing", &_right_tail_wing, 15 );

    _engineFG.setContactPoint(Vector3D(0, 1.71f, 0.4236f));

	addForceGenerator("leftWingBleed", &_leftWingBleed, 15);
	addForceGenerator("rightWingBleed", &_rightWingBleed, 15);
	addForceGenerator("frontPitchBleed", &_frontPitchBleed, 15);
	addForceGenerator("rearPitchBleed", &_rearPitchBleed, 15);
	addForceGenerator("rearYawBleed", &_rearYawBleed, 15);

	addForceGenerator("leftEngineFrontBleed", &_leftEngineFrontBleed, 15);
	addForceGenerator("rightEngineFrontBleed", &_rightEngineFrontBleed, 15);
	addForceGenerator("leftEngineBackBleed", &_leftEngineBackBleed, 15);
	addForceGenerator("rightEngineBackBleed", &_rightEngineBackBleed, 15);

	addForceGenerator("engine", &_engineFG, 15);

	int fps = 150;

	addForceGenerator( "custom_fg", &_custom_fg );
    addForceGenerator( "windtunnel_fg", &_windTunnel);

	/*addForceGenerator( "left_engine_under_pod", &_left_engine_under_pod, fps );
	addForceGenerator( "right_engine_under_pod", &_right_engine_under_pod, fps );*/

	addForceGenerator( "left_wing_horiz", &_left_wing_horiz,fps );
	addForceGenerator( "right_wing_horiz", &_right_wing_horiz,fps );

	addForceGenerator( "left_tail_horiz", &_left_tail_horiz,fps );
	addForceGenerator( "right_tail_horiz", &_right_tail_horiz,fps );

	addForceGenerator( "above_nose", &_above_nose,fps);
	addForceGenerator( "front_nose", &_front_nose,fps);
	addForceGenerator( "rear_body", &_rear_body,fps);
	addForceGenerator( "above_vtail", &_above_vtail,fps);
	addForceGenerator( "lower_rear_body", &_lower_rear_body, fps );

	addForceGenerator("front_wheel", &_front_wheel, fps);
	addForceGenerator("left_wheel", &_left_wheel, fps);
	addForceGenerator("middle_wheel", &_middle_wheel, fps);
	addForceGenerator("right_wheel", &_right_wheel, fps);

	//MOJ_JEB
	//WheelConstrainedSpring<0,HarrierJSONRigidBody>::Attach( this, fps);

	JSONRigidBodyBuilder body(this);

	_engineFG.setMaxSpd(0.85f);
	double cl, cd, cm;

	try
	{
		body.build(path + "setup.json");

		float area(0.0f);
		// Total Area : test.
		{

			for (size_t i = 0; i < _left_wing.size(); i++)
				area += _left_wing.element(i)->getArea();

			for (size_t i = 0; i < _right_wing.size(); i++)
				area += _right_wing.element(i)->getArea();

			_wingAoaData.ClCdCm(0, cl, cd, cm);
			_engineFG.incrDrag(cd, area);

		}

		float area2(0.0f);
		// Total Area : test.
		{

			for (size_t i = 0; i < _left_tail_wing.size(); i++)
				area2 += _left_tail_wing.element(i)->getArea();

			for (size_t i = 0; i < _right_tail_wing.size(); i++)
				area2 += _right_tail_wing.element(i)->getArea();

			_elevatorAoaData.ClCdCm(0, cl, cd, cm);
			_engineFG.incrDrag(cd, area2);
		}

		float area3(0.0f);
		// Total Area : test.
		{

			for (size_t i = 0; i < _vertical_tail.size(); i++)
				area3 += _vertical_tail.element(i)->getArea();

			for (size_t i = 0; i < _vertical_tail.size(); i++)
				area3 += _vertical_tail.element(i)->getArea();

			_rudderAoaData.ClCdCm(0, cl, cd, cm);
			_engineFG.incrDrag(cd, area3);
		}

		float totArea = area + area2 + area3;


		float fResponseRate = 160.0f;
		hydraulics().setResponseRate( _vertical_tail.element(0)->controlSurfaceN(0), fResponseRate );

		hydraulics().setResponseRate( _left_wing.element(0)->controlSurfaceN(0), fResponseRate);
		hydraulics().setResponseRate( _left_wing.element(1)->controlSurfaceN(0), 5 );

		hydraulics().setResponseRate( _right_wing.element(0)->controlSurfaceN(0), fResponseRate );
		hydraulics().setResponseRate( _right_wing.element(1)->controlSurfaceN(0), 5 );

		hydraulics().setResponseRate( _left_tail_wing.controlSurface0(), fResponseRate);
		//hydraulics().setResponseRate( _left_tail_wing.element(0)->controlSurfaceN(0), fResponseRate );

		hydraulics().setResponseRate( _right_tail_wing.controlSurface0(), fResponseRate);
		//hydraulics().setResponseRate( _right_tail_wing.element(0)->controlSurfaceN(0), fResponseRate );

	}
	catch( const std::string& str )
	{
        std::cout << str << " : JSON config parse failed";
	}
}

