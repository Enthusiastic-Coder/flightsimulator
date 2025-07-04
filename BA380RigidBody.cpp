#include "stdafx.h"
#include "BA380RigidBody.h"
#include "JSONRigidBodyBuilder.h"
#include <iostream>
#include "WorldSystem.h"

BAAirbus380CustomForceGenerator::BAAirbus380CustomForceGenerator() {}

void BAAirbus380CustomForceGenerator::onApplyForce(Particle *p, double dt)
{	
	float goodForce = 1000000.0f;
	JSONRigidBody& body = *reinterpret_cast<JSONRigidBody*>(p);

    if (getWorld()->focusedRigidBody() != p)
        return;

	body.setBodyFrame();

	auto *pFW = body.forceGenerator( std::string("front_wheel") );
	auto *pLW = body.forceGenerator( std::string("left_front_wheel") );
	auto *pRW = body.forceGenerator( std::string("right_front_wheel") );

	// Timing
	// debug 0.05 ms  60 queries a millisecond
	// release 0.01 ms 300 queries a millisecond.

	if( GetAsyncKeyState( 'L' ) < 0 )
		body.applyForce( Vector3D(0,0,0), body.toLocalGroundFrame(Vector3D(0, goodForce*4 ,0)));

	if( GetAsyncKeyState( 223 ) < 0 )
		body.applyForce( pFW->getContactPointCG(body.cg()), Vector3D(0,goodForce,0));

	if( GetAsyncKeyState( '1' ) < 0 )
		body.applyForce( pLW->getContactPointCG(body.cg()), Vector3D(0,goodForce*2,0));

	if( GetAsyncKeyState( '2' ) < 0 )
		body.applyForce( pRW->getContactPointCG(body.cg()), Vector3D(0,goodForce*2,0));

	if( GetAsyncKeyState( 'J' ) < 0 )
		body.applyForce( Vector3D(0,2.74f,-28.25f), Vector3D(-goodForce, 0 ,0));

	if( GetAsyncKeyState( 'K' ) < 0 )
		body.applyForce( Vector3D(0,2.74f,-28.25f), Vector3D(goodForce, 0 ,0));

	if( GetAsyncKeyState( 'I') < 0 )
	{
		body.applyForce( Vector3D(-5.86f,-1.75,-1.1f), Vector3D(0, 0,-goodForce));
		body.applyForce( Vector3D(5.86f,-1.75,-1.1f), Vector3D(0, 0,-goodForce));
	}

	if( GetAsyncKeyState( 'M' ) < 0 )
	{
		body.applyForce( Vector3D(-3.74f,0,0.32f), Vector3D(0, 0, goodForce));
		body.applyForce( Vector3D(3.74f,0,0.32f), Vector3D(0, 0, goodForce));
	}
}


BAAirbus380JSONRigidBody::BAAirbus380JSONRigidBody(std::string name) :
	JSONRigidBody(name),
//    _windTunnel(_left_wing,_right_wing),
	_vertical_tail(&_rudderAoaData),
	_left_wing(&_wingAoaData,true),
	_right_wing(&_wingAoaData, true),
	_left_tail_wing(&_elevatorAoaData),
	_right_tail_wing(&_elevatorAoaData)
{
	setMass( 276800.0f );
	float springfricton = 140000.0f;

#pragma region CONTACTS CONFIGURATION
	_front_wheel.spring().setChannelMass(72226.5);
	_front_wheel.setContactPoint( Vector3D(0, 0.76, -31.06 ));
	_front_wheel.spring().setMinimumDistance( 0.36f );
	_front_wheel.spring().setEquilibriumDistance( 0.56f );
	_front_wheel.spring().setLength( 0.77f);
	_front_wheel.spring().setRestitution( 0.1f );

	_front_wheel.spring().setSpringFriction(springfricton);
	_front_wheel.spring().setSpringMass( 3000.0f );
	_front_wheel.spring().setContactFrictionCoefficient(35.0f);
	_front_wheel.spring().setWheelRadius(0.6f);
	_front_wheel.spring().calculateSpringConstant();
	

//////////////////////////////////////////////////////////
	_left_front_wheel.spring().setChannelMass( 56790.4 );
	_left_front_wheel.setContactPoint(Vector3D(-6.46, 0.76, -3));
	_left_front_wheel.spring().setMinimumDistance(0.56f);
	_left_front_wheel.spring().setEquilibriumDistance(0.76f);
	_left_front_wheel.spring().setLength(0.97f);
	_left_front_wheel.spring().setRestitution(0.85f);
	_left_front_wheel.spring().setSpringFriction(springfricton);
	_left_front_wheel.spring().setSpringMass(3000.0f);
	_left_front_wheel.spring().setContactFrictionCoefficient(15);

	_left_front_wheel.spring().setRestitution(0.1f);
	_left_front_wheel.spring().setWheelRadius(0.62);

	_left_front_wheel.spring().calculateSpringConstant();

///////////////////////////////////////////////////////////////////////


	_left_back_wheel.spring().setChannelMass(45494.2);
	_left_back_wheel.setContactPoint(Vector3D(-3.46, 0.76, 5.5));
	_left_back_wheel.spring().setMinimumDistance(0.56f);
	_left_back_wheel.spring().setEquilibriumDistance(0.76f);
	_left_back_wheel.spring().setLength(0.97f);
	_left_back_wheel.spring().setRestitution(0.85f);
	_left_back_wheel.spring().setSpringFriction(springfricton);
	_left_back_wheel.spring().setSpringMass(3000.0f);
	_left_back_wheel.spring().setContactFrictionCoefficient(15);

	_left_back_wheel.spring().setRestitution(0.1f);
	_left_back_wheel.spring().setWheelRadius(0.62);

	_left_back_wheel.spring().calculateSpringConstant();

///////////////////////////////////////////////////////////////////////
	_right_front_wheel.spring().setChannelMass(56790.4);
	_right_front_wheel.setContactPoint(Vector3D(6.46, 0.76, -3));
	_right_front_wheel.spring().setMinimumDistance(0.56f);
	_right_front_wheel.spring().setEquilibriumDistance(0.76f);
	_right_front_wheel.spring().setLength(0.97f);
	_right_front_wheel.spring().setRestitution(0.85f);
	_right_front_wheel.spring().setSpringFriction(springfricton);
	_right_front_wheel.spring().setSpringMass(3000.0f);
	_right_front_wheel.spring().setContactFrictionCoefficient(15);
	_right_front_wheel.spring().setRestitution(0.1f);
	_right_front_wheel.spring().setWheelRadius(0.62);
	_right_front_wheel.spring().calculateSpringConstant();
	

///////////////////////////////////////////////////////////////////////
	_right_back_wheel.spring().setChannelMass(45494.2);
	_right_back_wheel.setContactPoint(Vector3D(3.46, 0.76, 5.5));
	_right_back_wheel.spring().setMinimumDistance(0.56f);
	_right_back_wheel.spring().setEquilibriumDistance(0.76f);
	_right_back_wheel.spring().setLength(0.97f);
	_right_back_wheel.spring().setRestitution(0.85f);
	_right_back_wheel.spring().setSpringFriction(springfricton);
	_right_back_wheel.spring().setSpringMass(3000.0f);
	_right_back_wheel.spring().setContactFrictionCoefficient(15);
	_right_back_wheel.spring().setRestitution(0.1f);
	_right_back_wheel.spring().setWheelRadius(0.62);
	_right_back_wheel.spring().calculateSpringConstant();


	//////////////////////////////////////////////
	Vector3D enginePos[4];
	enginePos[0] = Vector3D(-26.4, 2.33, -1.83);
	enginePos[1] = Vector3D(-14.8, 1.18, -10.72);
	enginePos[2] = Vector3D(14.8, 1.18, -10.72);
	enginePos[3] = Vector3D(26.4, 2.33, -1.83);

	for (int i = 0; i < 4; ++i)
	{
		_under_engine_pod[i].setContactPoint(enginePos[i]);

		_under_engine_pod[i].spring().setChannelMass(getMass() / 2);
		_under_engine_pod[i].spring().setMinimumDistance(0.2f);
		_under_engine_pod[i].spring().setEquilibriumDistance(0.4f);
		_under_engine_pod[i].spring().setLength(0.6f);

		_under_engine_pod[i].spring().setMinimumDistance(0.1);
		_under_engine_pod[i].spring().setEquilibriumDistance(0.15);
		_under_engine_pod[i].spring().setLength(0.2);

		_under_engine_pod[i].spring().setSpringMass(300.0f);
		_under_engine_pod[i].spring().setContactFrictionCoefficient(20.0f);
		_under_engine_pod[i].spring().calculateSpringConstant();
	}


	// Rear Body
	_lower_rear_body.spring().setChannelMass(getMass() / 4);
	_lower_rear_body.setContactPoint( Vector3D(0.0f,3.55f, 21.0) );
	_lower_rear_body.spring().setMinimumDistance( 0.2f );
	_lower_rear_body.spring().setEquilibriumDistance( 0.4f );
	_lower_rear_body.spring().setLength( 0.8f );
	_lower_rear_body.spring().setSpringMass( 300.0f );
	_lower_rear_body.spring().setContactFrictionCoefficient( 20.0f );
	_lower_rear_body.spring().calculateSpringConstant();

	//////////////////////////////////////////
	float m = getMass() / 20;
	_left_wing_horiz.spring().setChannelMass(m);
	_left_wing_horiz.setContactPoint(Vector3D(-40.58f, 7.0f, 15.0f));
	_left_wing_horiz.spring().setMinimumDistance( 0.1f );
	_left_wing_horiz.spring().setEquilibriumDistance( 0.2f );
	_left_wing_horiz.spring().setLength( 0.8f);
	_left_wing_horiz.spring().setSpringMass( 300.0f );
	_left_wing_horiz.spring().setContactFrictionCoefficient( 20.0f );
	_left_wing_horiz.spring().calculateSpringConstant();

	_right_wing_horiz.spring().setChannelMass(m);
	_right_wing_horiz.setContactPoint(Vector3D(40.58f, 7.0f, 15.0f));
	_right_wing_horiz.spring().setMinimumDistance( 0.1f );
	_right_wing_horiz.spring().setEquilibriumDistance( 0.2f );
	_right_wing_horiz.spring().setLength( 0.8f);
	_right_wing_horiz.spring().setSpringMass( 300.0f );
	_right_wing_horiz.spring().setContactFrictionCoefficient( 20.0f );
	_right_wing_horiz.spring().calculateSpringConstant();

	//////////////////////////////////////////

	//////////////////////////////////////////
	_left_tail_horiz.spring().setChannelMass(getMass()/8);
	_left_tail_horiz.setContactPoint( Vector3D(-15.44f, 8.75f, 38.0f) );
	_left_tail_horiz.spring().setMinimumDistance( 0.5f );
	_left_tail_horiz.spring().setEquilibriumDistance( 0.6f );
	_left_tail_horiz.spring().setLength( 0.8f);
	_left_tail_horiz.spring().setSpringMass( 300.0f );
	_left_tail_horiz.spring().setContactFrictionCoefficient( 20.0f );
	_left_tail_horiz.spring().calculateSpringConstant();

	_right_tail_horiz.spring().setChannelMass(getMass() / 8);
	_right_tail_horiz.setContactPoint( Vector3D(15.44f, 8.75f, 38.0f) );
	_right_tail_horiz.spring().setMinimumDistance( 0.5f );
	_right_tail_horiz.spring().setEquilibriumDistance( 0.6f );
	_right_tail_horiz.spring().setLength( 0.8f);
	_right_tail_horiz.spring().setSpringMass( 300.0f );
	_right_tail_horiz.spring().setContactFrictionCoefficient( 20.0f );
	_right_tail_horiz.spring().calculateSpringConstant();

	//////////////////////////////////////////

	_above_nose.spring().setChannelMass(getMass()/4.0f);
	_above_nose.setContactPoint( Vector3D(0.0f,11.3f,-24.0f) );
	_above_nose.spring().setMinimumDistance( 0.2f );
	_above_nose.spring().setEquilibriumDistance( 0.6f );
	_above_nose.spring().setLength( 0.8f);
	_above_nose.spring().setSpringMass( 300.0f );
	_above_nose.spring().setContactFrictionCoefficient( 20.0f );
	_above_nose.spring().calculateSpringConstant();

	_front_nose.spring().setChannelMass(getMass() / 8);
	_front_nose.setContactPoint( Vector3D(0.0f, 5.26f,-36.55f) );
	_front_nose.spring().setMinimumDistance( 0.2f );
	_front_nose.spring().setEquilibriumDistance( 0.6f );
	_front_nose.spring().setLength( 0.8f);
	_front_nose.spring().setSpringMass( 300.0f );
	_front_nose.spring().setContactFrictionCoefficient( 20.0f );
	_front_nose.spring().calculateSpringConstant();

	_rear_body.spring().setChannelMass(getMass() / 8);
	_rear_body.setContactPoint( Vector3D(0.0f,9.0f,37.0f) );
	_rear_body.spring().setMinimumDistance( 0.2f );
	_rear_body.spring().setEquilibriumDistance( 0.6f );
	_rear_body.spring().setLength( 0.8f);
	_rear_body.spring().setSpringMass( 300.0f );
	_rear_body.spring().setContactFrictionCoefficient( 20.0f );
	_rear_body.spring().calculateSpringConstant();

	_above_vtail.spring().setChannelMass(getMass()/6.0f);
	_above_vtail.setContactPoint( Vector3D(0.0f,25.0f,38.38) );
	_above_vtail.spring().setMinimumDistance( 0.2f );
	_above_vtail.spring().setEquilibriumDistance( 0.4f );
	_above_vtail.spring().setLength( 0.6f);
	_above_vtail.spring().setSpringMass( 300 );
	_above_vtail.spring().setContactFrictionCoefficient( 20.0f );
	_above_vtail.spring().calculateSpringConstant();

	_engine[0].setContactPoint(Vector3D(-26.42, 4.4, -4.85));
	_engine[1].setContactPoint(Vector3D(-14.85, 3.25, -4.85));
	_engine[2].setContactPoint(Vector3D(14.85, 3.25, -4.85));
	_engine[3].setContactPoint(Vector3D(26.42, 4.4, -4.85));

	_front_wheel.spring().setDrivingState( SpringModel::DrivingState::NEUTRAL );
#pragma endregion

	addForceGenerators();
	initSpeedAndPos();
}

bool BAAirbus380JSONRigidBody::onAsyncKeyPress(IScreenMouseInfo* scrn, float dt)
{
    IJoystick *joy = _custom_fg.getJoystick();

    double max_decel = 6.0f;
    double max_accel = 6.0f;
    max_decel = 12.0f;


    /////////////TEST CODE///////////////////////
#if defined DANGEROUS_CG_CHANGE
    if( Height() > 100/3.2808 )
    {
        static Vector3D shift(0,0,0);
        shift.z += 0.1 * dt;
        if( shift.z > 4.0 )
            shift.z = 4.0;

        setCG(Vector3D(0,3.34,-1.75)+ shift);
    }
#endif
    //////////////////////////////////////

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
        _engine[0]._thrust_percent -= fThrust;

        fThrust = _engine[0]._thrust_percent;

        _front_wheel.spring().setSteeringAngle(fYaw * 55.0f);
        hydraulics().setDeflection(_vertical_tail.element(0)->controlSurfaceN(0), fYaw * 15.0f );
        _engine[0]._thrust_percent = fThrust;

        hydraulics().setDeflection( _left_wing.element(0)->controlSurfaceN(0), fAileron * 15.0f );
        hydraulics().setDeflection( _right_wing.element(0)->controlSurfaceN(0), -fAileron * 15.0f );

        hydraulics().setDeflection( _left_tail_wing.element(0)->controlSurfaceN(0), -fPitch * 20.0f );
        hydraulics().setDeflection( _right_tail_wing.element(0)->controlSurfaceN(0), -fPitch * 20.0f );


        float deflection_l = _left_tail_wing.controlSurface0()->getDeflection();
        deflection_l -= dt * fPitch;

        float MAX_DEFL = 10;
        if( deflection_l < -MAX_DEFL )
            deflection_l = -MAX_DEFL;

        _left_tail_wing.controlSurface0()->setDeflection(deflection_l);
        _right_tail_wing.controlSurface0()->setDeflection(deflection_l);


        if( _engine[0]._thrust_percent > 100 )
            _engine[0]._thrust_percent  = 100;

        if( _engine[0]._thrust_percent < 0 )
            _engine[0]._thrust_percent = 0;

        _engine[1]._thrust_percent = _engine[0]._thrust_percent;
        _engine[2]._thrust_percent = _engine[0]._thrust_percent;
        _engine[3]._thrust_percent = _engine[0]._thrust_percent;

        DWORD dwWord = joy->buttonFlagPressed();

        //128(down) - 32 (up)
        if( dwWord == 8 || dwWord == 4 )
        {
            float diff = (dwWord==4? 1 : -1) * 0.5f;
            float MAX_DEFL = 15;
            //float deflection_l = _left_tail_wing.controlSurface0()->getDeflection();
            //if( deflection_l < -MAX_DEFL )	deflection_l = -MAX_DEFL;
            //if( deflection_l > MAX_DEFL )	deflection_l = MAX_DEFL;
            //hydraulics().setDeflection(_left_tail_wing.controlSurface0(), deflection_l+diff );

            //float deflection_r = _right_tail_wing.controlSurface0()->getDeflection();
            //if( deflection_r < -MAX_DEFL )	deflection_r = -MAX_DEFL;
            //if( deflection_r > MAX_DEFL )	deflection_r  = MAX_DEFL;
            //hydraulics().setDeflection( _right_tail_wing.controlSurface0(), deflection_r+diff );
        }

        if( dwWord & 2 ) //Regular Braking.
        {
            if( _front_wheel.spring().getWheelSpeed()  > 0 )
                _front_wheel.spring().incrWheelSpeed(- std::min( max_decel * dt, _front_wheel.spring().getWheelSpeed() ) );
            else
                _front_wheel.spring().incrWheelSpeed( std::min( max_decel * dt, (-_front_wheel.spring().getWheelSpeed()) ));

            if( _left_front_wheel.spring().getWheelSpeed()  > 0 )
                _left_front_wheel.spring().incrWheelSpeed(- std::min( max_decel * dt, _left_front_wheel.spring().getWheelSpeed() ));
            else
                _left_front_wheel.spring().incrWheelSpeed( std::min( max_decel * dt, (-_left_front_wheel.spring().getWheelSpeed()) ) );

            if (_left_back_wheel.spring().getWheelSpeed()  > 0)
                _left_back_wheel.spring().incrWheelSpeed(- std::min(max_decel * dt, _left_back_wheel.spring().getWheelSpeed()));
            else
                _left_back_wheel.spring().incrWheelSpeed( std::min(max_decel * dt, (-_left_back_wheel.spring().getWheelSpeed())) );

            if( _right_front_wheel.spring().getWheelSpeed()  > 0 )
                _right_front_wheel.spring().incrWheelSpeed( - std::min( max_decel * dt, _right_front_wheel.spring().getWheelSpeed() ) );
            else
                _right_front_wheel.spring().incrWheelSpeed( std::min( max_decel * dt, (-_right_front_wheel.spring().getWheelSpeed()) ) );

            if (_right_back_wheel.spring().getWheelSpeed()  > 0)
                _right_back_wheel.spring().incrWheelSpeed( - std::min(max_decel * dt, _right_back_wheel.spring().getWheelSpeed()) );
            else
                _right_back_wheel.spring().incrWheelSpeed( std::min(max_decel * dt, (-_right_back_wheel.spring().getWheelSpeed())) );
        }
    }

    if( GetAsyncKeyState( '3' ) < 0 )
    {
        if( GetAsyncKeyState( VK_CONTROL ) == 0 )
        {
            //float deflection_l = _left_tail_wing.element(0)->controlSurfaceN(0)->getDeflection();
            //deflection_l -= dt * 30;
            //if( deflection_l < -25 )
                //deflection_l = -25;
            //_left_tail_wing.element(0)->controlSurfaceN(0)->setDeflection( deflection_l);

            //float deflection_r = _right_tail_wing.element(0)->controlSurfaceN(0)->getDeflection();
            //deflection_r -= dt * 30;
            //if( deflection_r < -25 )
                //deflection_r = -25;
            //_right_tail_wing.element(0)->controlSurfaceN(0)->setDeflection( deflection_r);
        }
        else
        {
            float MAX_DEFL = 10;
            //float deflection_l = _left_tail_wing.controlSurface0()->getDeflection();
            //deflection_l -= dt * 30;
            //if( deflection_l < -MAX_DEFL )
                //deflection_l = -MAX_DEFL;
            //_left_tail_wing.controlSurface0()->setDeflection( deflection_l);

            //float deflection_r = _right_tail_wing.controlSurface0()->getDeflection();
            //deflection_r -= dt * 30;
            //if( deflection_r < -MAX_DEFL )
                //deflection_r = -MAX_DEFL;
            //_right_tail_wing.controlSurface0()->setDeflection( deflection_r);
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
            /*float MAX_DEFL = 10;
            float deflection_l = _left_tail_wing.controlSurface0()->getDeflection();
            deflection_l += dt * 30;
            if( deflection_l > MAX_DEFL )
                deflection_l = MAX_DEFL;
            _left_tail_wing.controlSurface0()->setDeflection( deflection_l);

            float deflection_r = _right_tail_wing.controlSurface0()->getDeflection();
            deflection_r += dt * 30;
            if( deflection_r > MAX_DEFL )
                deflection_r = MAX_DEFL;
            _right_tail_wing.controlSurface0()->setDeflection( deflection_r);*/
        }
    }

    if( GetAsyncKeyState( '5' ) < 0 )
    {
        float deflection_l = _left_wing.element(0)->controlSurfaceN(0)->getDeflection();
        deflection_l -= dt * 30;
        if( deflection_l < -25 )
            deflection_l = -25;
        _left_wing.element(0)->controlSurfaceN(0)->setDeflection(deflection_l);

        float deflection_r = _right_wing.element(0)->controlSurfaceN(0)->getDeflection();
        deflection_r += dt * 30;
        if( deflection_r > 25 )
            deflection_r = 25;
        _right_wing.element(0)->controlSurfaceN(0)->setDeflection(deflection_r);
    }

    if( GetAsyncKeyState( '6' ) < 0 )
    {
        float deflection_l = _left_wing.element(0)->controlSurfaceN(0)->getDeflection();
        deflection_l += dt * 30;
        if( deflection_l > 25 )
            deflection_l = 25;
        _left_wing.element(0)->controlSurfaceN(0)->setDeflection(deflection_l);

        float deflection_r = _right_wing.element(0)->controlSurfaceN(0)->getDeflection();
        deflection_r -= dt * 30;
        if( deflection_r < -25 )
            deflection_r = -25;
        _right_wing.element(0)->controlSurfaceN(0)->setDeflection(deflection_r);
    }

    if( GetAsyncKeyState( '7') < 0 )
    {
        _front_wheel.spring().incrSteeringAngle(- dt*30);
        float deflection = _front_wheel.spring().getSteeringAngle() ;
        if( deflection < -25 )	deflection = -25;
        if( deflection > 25 )	deflection = 25;
        _vertical_tail.element(0)->controlSurfaceN(0)->setDeflection(deflection);

    }

    if( GetAsyncKeyState( '8') < 0 )
    {
        _front_wheel.spring().incrSteeringAngle(dt*30);
        float deflection = _front_wheel.spring().getSteeringAngle() ;
        if( deflection < -25 )	deflection = -25;
        if( deflection > 25 )	deflection = 25;
        _vertical_tail.element(0)->controlSurfaceN(0)->setDeflection(deflection);
    }

    if( GetAsyncKeyState( '0' ) < 0 )
    {
        _front_wheel.spring().setSteeringAngle(0.0f);

        hydraulics().setDeflection( _left_wing.element(0)->controlSurfaceN(0), 0.0f );
        hydraulics().setDeflection( _right_wing.element(0)->controlSurfaceN(0), 0.0f );
        hydraulics().setDeflection( _left_tail_wing.element(0)->controlSurfaceN(0), 0.0f );
        hydraulics().setDeflection( _right_tail_wing.element(0)->controlSurfaceN(0), 0.0f );
        hydraulics().setDeflection( _vertical_tail.element(0)->controlSurfaceN(0), 0.0f );
        //hydraulics().setDeflection( _left_tail_wing.controlSurface0(), 0.0f );
        //hydraulics().setDeflection( _right_tail_wing.controlSurface0(), 0.0f );
    }

    if( GetAsyncKeyState( '9' ) < 0 )
    {
        _front_wheel.spring().setWheelSpeed(-std::min(fabs(_front_wheel.spring().getWheelSpeed()-max_accel* dt),100.0) );

        _left_front_wheel.spring().setWheelSpeed(-std::min(fabs(_left_front_wheel.spring().getWheelSpeed()-max_accel* dt),100.0));
        _left_back_wheel.spring().setWheelSpeed(-std::min(fabs(_left_back_wheel.spring().getWheelSpeed() - max_accel* dt), 100.0));

        _right_front_wheel.spring().setWheelSpeed(-std::min(fabs(_right_front_wheel.spring().getWheelSpeed()-max_accel* dt),100.0));
        _right_back_wheel.spring().setWheelSpeed( -std::min(fabs(_right_back_wheel.spring().getWheelSpeed() - max_accel* dt), 100.0));
    }

    if( GetAsyncKeyState( VK_NEXT ) < 0 )
    {
        for (int i = 0; i < 4; ++i)
            _engine[i]._thrust_percent -= dt * 20;
    }

    if( GetAsyncKeyState( VK_PRIOR ) < 0 )
    {
        for (int i = 0; i < 4; ++i)
            _engine[i]._thrust_percent += dt * 20;
    }

    if( _front_wheel.spring().getSteeringAngle() < -75 )
        _front_wheel.spring().setSteeringAngle(-75);

    if( _front_wheel.spring().getSteeringAngle() > 75 )
        _front_wheel.spring().setSteeringAngle(75);

    if( GetAsyncKeyState( 'B' ) < 0 )
    {
        if( GetAsyncKeyState( VK_SHIFT ) < 0 )
        {
            _left_front_wheel.spring().setDrivingState( SpringModel::DrivingState::BRAKE );
            _left_back_wheel.spring().setDrivingState( SpringModel::DrivingState::BRAKE );

            _right_front_wheel.spring().setDrivingState( SpringModel::DrivingState::BRAKE );
            _right_back_wheel.spring().setDrivingState( SpringModel::DrivingState::BRAKE );
        }
        else
        {
            _front_wheel.spring().setDrivingState( SpringModel::DrivingState::NEUTRAL );

            _left_front_wheel.spring().setDrivingState( SpringModel::DrivingState::NEUTRAL );
            _left_back_wheel.spring().setDrivingState( SpringModel::DrivingState::NEUTRAL );

            _right_front_wheel.spring().setDrivingState( SpringModel::DrivingState::NEUTRAL );
            _right_back_wheel.spring().setDrivingState( SpringModel::DrivingState::NEUTRAL );

            if( _left_front_wheel.spring().getWheelSpeed()  > 0 )
                _left_front_wheel.spring().incrWheelSpeed(- std::min( max_decel * dt, _left_front_wheel.spring().getWheelSpeed() ));
            else
                _left_front_wheel.spring().incrWheelSpeed( std::min( max_decel * dt, (-_left_front_wheel.spring().getWheelSpeed()) ));

            if (_left_back_wheel.spring().getWheelSpeed()  > 0)
                _left_back_wheel.spring().incrWheelSpeed( -std::min(max_decel * dt, _left_back_wheel.spring().getWheelSpeed()));
            else
                _left_front_wheel.spring().incrWheelSpeed( std::min(max_decel * dt, (-_left_back_wheel.spring().getWheelSpeed())));

            if( _right_front_wheel.spring().getWheelSpeed()  > 0 )
                _right_front_wheel.spring().incrWheelSpeed( - std::min( max_decel * dt, _right_front_wheel.spring().getWheelSpeed() ));
            else
                _right_front_wheel.spring().incrWheelSpeed(std::min( max_decel * dt, (-_right_front_wheel.spring().getWheelSpeed()) ));

            if (_right_back_wheel.spring().getWheelSpeed()  > 0)
                _right_back_wheel.spring().incrWheelSpeed(- std::min(max_decel * dt, _right_back_wheel.spring().getWheelSpeed()));
            else
                _right_back_wheel.spring().incrWheelSpeed( std::min(max_decel * dt, (-_right_back_wheel.spring().getWheelSpeed())));
        }
    }

    for (int i = 0; i < 4; ++i)
    {
        if (_engine[i]._thrust_percent < 0.0)
            _engine[i]._thrust_percent = 0.0;

        if (_engine[i]._thrust_percent > 100.0)
            _engine[i]._thrust_percent = 100.0;
    }

    if( isUsingMouse() )
    {
        int x, y;
        scrn->GetMousePos(x, y);

        int width, height;
        scrn->GetScreenDims(width, height);

        int mx = width / 2;
        int my = height / 2;

        float dx = (x - mx) / 8.0f;
        float dy = (y - my) / 8.0f;

        if( dx < -25 ) dx = -25;
        if( dx > 25 ) dx = 25;
        if( dy < -35 ) dy = -35;
        if( dy > 35 ) dy = 35;

        //hydraulics().setDeflection( _left_wing.element(0)->controlSurfaceN(2), 0);
        //hydraulics().setDeflection( _right_wing.element(0)->controlSurfaceN(2), 0);

        if( Height() < 2.0f )
        {
            hydraulics().setDeflection( _left_wing.element(0)->controlSurfaceN(0), dx/8.0f );
            hydraulics().setDeflection( _right_wing.element(0)->controlSurfaceN(0), -dx/8.0f );
        }
        else
        {
            hydraulics().setDeflection( _left_wing.element(0)->controlSurfaceN(0), dx );
            hydraulics().setDeflection( _right_wing.element(0)->controlSurfaceN(0), -dx );
        }

        hydraulics().setDeflection( _left_tail_wing.element(0)->controlSurfaceN(0), -dy );
        hydraulics().setDeflection( _right_tail_wing.element(0)->controlSurfaceN(0), -dy );

        dx = (x - mx) / 10.0f;

        _front_wheel.spring().setSteeringAngle(dx);

        dx /= 2.0f;

        if( dx < -25 ) dx = -25;
        if( dx > 25 ) dx = 25;

        hydraulics().setDeflection( _vertical_tail.element(0)->controlSurfaceN(0), dx );
    }

    return true;
}

void BAAirbus380JSONRigidBody::updateCameraView()
{
    JSONRigidBody::updateCameraView();

    switch((ViewPosition)getCameraProvider()->curViewIdx())
    {
    case CockpitForwards :
        getCameraProvider()->getCameraView()->setPosition(toNonLocalTranslateFrame(Vector3D(-0.8, 7.6, -34.0)));
        break;
    case PassengerLeftMiddle :
        break;
    case RightGearWheel :
        break;
    }
}

bool BAAirbus380JSONRigidBody::onMouseWheel(int wheelDelta)
{
	for (int i = 0; i < 4; ++i)
        _engine[i]._thrust_percent += wheelDelta;
	return true;
}

float BAAirbus380JSONRigidBody::getPower(int engine)
{
	if (engine < 0 || engine > 3)
		return 0.0f;

	return _engine[engine]._thrust_percent;
}

float BAAirbus380JSONRigidBody::getPowerOutput(int engine)
{
	if (engine < 0 || engine > 3)
		return 0.0f;

	return _engine[engine]._actual_thrust_percent;

}

void BAAirbus380JSONRigidBody::setPower(float value)
{
    for( size_t i=0; i < 4; ++i)
        _engine[i]._thrust_percent = value;
}

bool BAAirbus380JSONRigidBody::onSyncKeyPress()
{
    if( GetKeyState(VK_F5) < 0)
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

	return false;
}

void BAAirbus380JSONRigidBody::initSpeedAndPos()
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

	setPosition(GPSLocation(51.477681, -0.433258, 0) + Vector3F(-50, 0, -20));
	setVelocity(0,0,0);
	setEuler( 5,90,-0.1);
	
#if defined LOCATED_AT_GIBRALTER
	setEuler(0,270,0);
	setPosition( GPSLocation(36.151473, -5.339665,5) + Vector3F(1609*3,91 * 3,0));
	setVelocity(-82, 0, 0);

	//setPosition(GPSLocation(36.151473, -5.339665, 5));
	//setVelocity(0, 0, 0);

#else
	setPosition( GPSLocation(51.464951, -0.434115,22));
#endif
}

void BAAirbus380JSONRigidBody::addForceGenerators()
{
    std::string path = getPath();

	if( !_wingAoaData.load( path + "a380.afl" ) )
        std::cout << "Failed to load WingData  :  a380.afl";

	if( !_rudderAoaData.load( path + "NACA 0009 (symmetrical).afl" ) )
        std::cout << "Failed to load NACA 0009 (symmetrical).afl  : NACA 0009 (symmetrical)";

	if( !_elevatorAoaData.load( path + "NACA 0009 (symmetrical).afl" ) )
        std::cout << "Failed to load NACA 0009 (symmetrical).afl : NACA 0009 (symmetrical)";

	addForceGenerator( "gravityFG", &_gravityFG, 15 );

	for (int i = 0; i < 4; ++i)
	{
		char name[16];
		sprintf(name, "engine_%d", i + 1);
		addForceGenerator(name, &_engine[i], 15);
	}

	addForceGenerator( "vertical_tail", &_vertical_tail, 15 );

	addForceGenerator( "left_wing", &_left_wing, 15 );
	addForceGenerator( "right_wing", &_right_wing, 15 );

	addForceGenerator( "left_tail_wing", &_left_tail_wing, 15 );
	addForceGenerator( "right_tail_wing", &_right_tail_wing, 15 );

	addForceGenerator("custom_fg", &_custom_fg);

	int fps = 150;

	for (int i = 0; i < 4; ++i)
	{
		char name[64];
		sprintf(name, "engine_under_pod_%d", i + 1);
		addForceGenerator(name, &_under_engine_pod[i], fps);
	}

	addForceGenerator( "left_wing_horiz", &_left_wing_horiz,fps );
	addForceGenerator( "right_wing_horiz", &_right_wing_horiz,fps );

	addForceGenerator( "left_tail_horiz", &_left_tail_horiz,fps );
	addForceGenerator( "right_tail_horiz", &_right_tail_horiz,fps );

	addForceGenerator( "above_nose", &_above_nose,fps);
	addForceGenerator( "front_nose", &_front_nose,fps);
	addForceGenerator( "rear_body", &_rear_body,fps);
	addForceGenerator( "above_vtail", &_above_vtail,fps);
	addForceGenerator( "lower_rear_body", &_lower_rear_body, fps );

	addForceGenerator( "front_wheel", &_front_wheel, fps );

	addForceGenerator( "left_front_wheel", &_left_front_wheel, fps );
	addForceGenerator("left_back_wheel", &_left_back_wheel, fps);

	addForceGenerator( "right_front_wheel", &_right_front_wheel, fps );
	addForceGenerator("right_back_wheel", &_right_back_wheel, fps);

	//MOJ_JEB
	//WheelConstrainedSpring<0,BAAirbus380JSONRigidBody>::Attach( this, fps);

	JSONRigidBodyBuilder body(this);

	body.build(path + "BAAirbusA380.body");

	for (int i = 0; i < 4; ++i)
		_engine[i].setMaxSpd(0.82f);
	
	double cl, cd, cm;


	float area(0.0f);
	// Total Area : test.
	{

		for (size_t i = 0; i < _left_wing.size(); i++)
			area += _left_wing.element(i)->getArea();

		for (size_t i = 0; i < _right_wing.size(); i++)
			area += _right_wing.element(i)->getArea();

		_wingAoaData.ClCdCm(0, cl, cd, cm);

		for (int i = 0; i < 4; ++i)
			_engine[i].incrDrag(cd, area);
	}

	float area2(0.0f);
	// Total Area : test.
	{

		for (size_t i = 0; i < _left_tail_wing.size(); i++)
			area2 += _left_tail_wing.element(i)->getArea();

		for (size_t i = 0; i < _right_tail_wing.size(); i++)
			area2 += _right_tail_wing.element(i)->getArea();

		_elevatorAoaData.ClCdCm(0, cl, cd, cm);
		for (int i = 0; i < 4; ++i)
			_engine[i].incrDrag(cd, area2);
	}

	float area3(0.0f);
	// Total Area : test.
	{

		for (size_t i = 0; i < _vertical_tail.size(); i++)
			area3 += _vertical_tail.element(i)->getArea();

		for (size_t i = 0; i < _vertical_tail.size(); i++)
			area3 += _vertical_tail.element(i)->getArea();

		_rudderAoaData.ClCdCm(0, cl, cd, cm);
		for (int i = 0; i < 4; ++i)
			_engine[i].incrDrag(cd, area3);
	}

	float totArea = area + area2 + area3;

	float fResponseRate = 80.0f;
	hydraulics().setResponseRate( _vertical_tail.element(0)->controlSurfaceN(0), fResponseRate );

	hydraulics().setResponseRate( _left_wing.element(0)->controlSurfaceN(0), fResponseRate);
	//hydraulics().setResponseRate( _left_wing.element(0)->controlSurfaceN(2), 40 );
	//hydraulics().setResponseRate( _left_wing.element(1)->controlSurfaceN(1), 80 );

	hydraulics().setResponseRate( _right_wing.element(0)->controlSurfaceN(0), fResponseRate );
	//hydraulics().setResponseRate( _right_wing.element(0)->controlSurfaceN(2), 40 );
	//hydraulics().setResponseRate( _right_wing.element(1)->controlSurfaceN(1), 80 );

	hydraulics().setResponseRate( _left_tail_wing.controlSurface0(), 1.0f );
	hydraulics().setResponseRate( _left_tail_wing.element(0)->controlSurfaceN(0), fResponseRate );

	hydraulics().setResponseRate( _right_tail_wing.controlSurface0(), 1.0f );
	hydraulics().setResponseRate( _right_tail_wing.element(0)->controlSurfaceN(0), fResponseRate );

}



/////////////////////////////////////////////////////////////////////////////////////////////////

