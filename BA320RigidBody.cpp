#include "stdafx.h"
#include "BA320RigidBody.h"
#include "JSONRigidBodyBuilder.h"
#include "WorldSystem.h"

#include <iostream>
BAAirbus320CustomForceGenerator::BAAirbus320CustomForceGenerator() {}

void BAAirbus320CustomForceGenerator::onApplyForce(Particle *p, double dt)
{
#ifdef WIN32
    float goodForce = 200000.0f;
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
        body.applyForce( pFW->getContactPointCG(body.cg()), Vector3D(0,goodForce/2,0));

    if( GetAsyncKeyState( '1' ) < 0 )
        body.applyForce( pLW->getContactPointCG(body.cg()), Vector3D(0,goodForce,0));

    if( GetAsyncKeyState( '2' ) < 0 )
        body.applyForce( pRW->getContactPointCG(body.cg()), Vector3D(0,goodForce,0));

    if( GetAsyncKeyState( 'J' ) < 0 )
        body.applyForce( Vector3D(0,8.74f,18.25f), Vector3D(goodForce, 0 ,0)/4.0);

    if( GetAsyncKeyState( 'K' ) < 0 )
        body.applyForce( Vector3D(0,8.74f,18.25f), Vector3D(-goodForce, 0 ,0)/4.0);

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
#endif
}


BAAirbus320JSONRigidBody::BAAirbus320JSONRigidBody(std::string name) :
    JSONRigidBody(name),
    //_windTunnel(_left_wing,_right_wing),
    _vertical_tail(&_rudderAoaData),
    _left_wing(&_wingAoaData,true),
    _right_wing(&_wingAoaData, true),
    _left_tail_wing(&_elevatorAoaData),
    _right_tail_wing(&_elevatorAoaData)
{
    setMass( 42800 );

#pragma region CONTACTS CONFIGURATION
    _front_wheel.spring().setChannelMass( 12891.0f);
    _front_wheel.setContactPoint( Vector3D(0, 0.66f, -12.33f ));

    _front_wheel.spring().setMinimumDistance( 0.36f );
    _front_wheel.spring().setEquilibriumDistance( 0.56f );
    _front_wheel.spring().setLength( 0.77f);
    _front_wheel.spring().setRestitution( 0.1f );

    _front_wheel.spring().setSpringFriction( 35000.0f );
    _front_wheel.spring().setSpringMass( 300.0f );
    _front_wheel.spring().setContactFrictionCoefficient(15.0);
    _front_wheel.spring().calculateSpringConstant();
    _front_wheel.spring().setWheelRadius( 0.4f );
    _front_wheel.spring().setWheelRadius( 0.518347f );

    _left_wheel.spring().setChannelMass( 14954.5f );
    _left_wheel.setContactPoint( Vector3D(-3.66f, 0.12f+ 0.66f, 0.44f ));

    _left_wheel.spring().setMinimumDistance( 0.56f );
    _left_wheel.spring().setEquilibriumDistance( 0.76f );
    _left_wheel.spring().setLength( 0.97f );
    _left_wheel.spring().setRestitution( 0.1f );
    _left_wheel.spring().setSpringFriction( 35000.0f );
    _left_wheel.spring().setSpringMass( 300.0f );
    _left_wheel.spring().setContactFrictionCoefficient(15.0);

    _left_wheel.spring().setRestitution( 0.8f );

    _left_wheel.spring().calculateSpringConstant();
    _left_wheel.spring().setWheelRadius( 0.6363f );

    _right_wheel.spring().setChannelMass( 14954.5f );
    _right_wheel.setContactPoint( Vector3D(3.66f, 0.12f+ 0.66f, 0.44f ));
    _right_wheel.spring().setMinimumDistance( 0.56f );
    _right_wheel.spring().setEquilibriumDistance( 0.76f );
    _right_wheel.spring().setLength( 0.97f );
    _right_wheel.spring().setRestitution( 0.1f );
    _right_wheel.spring().setSpringFriction( 35000.0f );
    _right_wheel.spring().setSpringMass( 300.0f );
    _right_wheel.spring().setContactFrictionCoefficient(15.0);
    _right_wheel.spring().setRestitution( 0.8f );

    _right_wheel.spring().calculateSpringConstant();
    _right_wheel.spring().setWheelRadius( 0.6363f );

    //////////////////////////////////////////////
    _left_engine_under_pod.spring().setChannelMass(getMass()/2);
    _left_engine_under_pod.setContactPoint( Vector3D(-5.83f,0.6f,-4.13f) );
    _left_engine_under_pod.spring().setMinimumDistance( 0.2f );
    _left_engine_under_pod.spring().setEquilibriumDistance( 0.4f );
    _left_engine_under_pod.spring().setLength( 0.6f );

    _left_engine_under_pod.spring().setMinimumDistance( 0.1 );
    _left_engine_under_pod.spring().setEquilibriumDistance( 0.15 );
    _left_engine_under_pod.spring().setLength( 0.2 );

    _left_engine_under_pod.spring().setSpringMass( 300.0f );
    _left_engine_under_pod.spring().setContactFrictionCoefficient( 20.0f );
    _left_engine_under_pod.spring().calculateSpringConstant();

    // Right Engine
    _right_engine_under_pod.spring().setChannelMass(getMass() / 2);
    _right_engine_under_pod.setContactPoint( Vector3D(5.83f,0.6f,-4.13f) );
    _right_engine_under_pod.spring().setMinimumDistance( 0.2f );
    _right_engine_under_pod.spring().setEquilibriumDistance( 0.4f );
    _right_engine_under_pod.spring().setLength( 0.6f );

    _right_engine_under_pod.spring().setMinimumDistance( 0.1 );
    _right_engine_under_pod.spring().setEquilibriumDistance( 0.15 );
    _right_engine_under_pod.spring().setLength( 0.2 );

    _right_engine_under_pod.spring().setSpringMass( 300.0f );
    _right_engine_under_pod.spring().setContactFrictionCoefficient( 20.0f );
    _right_engine_under_pod.spring().calculateSpringConstant();

    // Rear Body
    _lower_rear_body.spring().setChannelMass(getMass() / 4);
    _lower_rear_body.setContactPoint( Vector3D(0.0f,2.5f, 13.36f) );
    _lower_rear_body.spring().setMinimumDistance( 0.2f );
    _lower_rear_body.spring().setEquilibriumDistance( 0.4f );
    _lower_rear_body.spring().setLength( 0.8f );
    _lower_rear_body.spring().setSpringMass( 300.0f );
    _lower_rear_body.spring().setContactFrictionCoefficient( 20.0f );
    _lower_rear_body.spring().calculateSpringConstant();

    //////////////////////////////////////////
    float m = getMass() / 20;
    _left_wing_horiz.spring().setChannelMass(m);
    _left_wing_horiz.setContactPoint( Vector3D(-17.7f,4.1f,4.94f) );
    _left_wing_horiz.spring().setMinimumDistance( 0.1f );
    _left_wing_horiz.spring().setEquilibriumDistance( 0.2f );
    _left_wing_horiz.spring().setLength( 0.8f);
    _left_wing_horiz.spring().setSpringMass( 300.0f );
    _left_wing_horiz.spring().setContactFrictionCoefficient( 20.0f );
    _left_wing_horiz.spring().calculateSpringConstant();

    _right_wing_horiz.spring().setChannelMass(m);
    _right_wing_horiz.setContactPoint( Vector3D(17.7f,4.1f,4.94f) );
    _right_wing_horiz.spring().setMinimumDistance( 0.1f );
    _right_wing_horiz.spring().setEquilibriumDistance( 0.2f );
    _right_wing_horiz.spring().setLength( 0.8f);
    _right_wing_horiz.spring().setSpringMass( 300.0f );
    _right_wing_horiz.spring().setContactFrictionCoefficient( 20.0f );
    _right_wing_horiz.spring().calculateSpringConstant();

    //////////////////////////////////////////

    //////////////////////////////////////////
    _left_tail_horiz.spring().setChannelMass(getMass()/8);
    _left_tail_horiz.setContactPoint( Vector3D(-6.43f,5.23f,19.46f) );
    _left_tail_horiz.spring().setMinimumDistance( 0.5f );
    _left_tail_horiz.spring().setEquilibriumDistance( 0.6f );
    _left_tail_horiz.spring().setLength( 0.8f);
    _left_tail_horiz.spring().setSpringMass( 300.0f );
    _left_tail_horiz.spring().setContactFrictionCoefficient( 20.0f );
    _left_tail_horiz.spring().calculateSpringConstant();

    _right_tail_horiz.spring().setChannelMass(getMass() / 8);
    _right_tail_horiz.setContactPoint( Vector3D(6.43f,5.23f,19.46f) );
    _right_tail_horiz.spring().setMinimumDistance( 0.5f );
    _right_tail_horiz.spring().setEquilibriumDistance( 0.6f );
    _right_tail_horiz.spring().setLength( 0.8f);
    _right_tail_horiz.spring().setSpringMass( 300.0f );
    _right_tail_horiz.spring().setContactFrictionCoefficient( 20.0f );
    _right_tail_horiz.spring().calculateSpringConstant();

    //////////////////////////////////////////

    _above_nose.spring().setChannelMass(getMass()/4.0f);
    _above_nose.setContactPoint( Vector3D(0.0f,6.00f,-12.12f) );
    _above_nose.spring().setMinimumDistance( 0.2f );
    _above_nose.spring().setEquilibriumDistance( 0.6f );
    _above_nose.spring().setLength( 0.8f);
    _above_nose.spring().setSpringMass( 300.0f );
    _above_nose.spring().setContactFrictionCoefficient( 20.0f );
    _above_nose.spring().calculateSpringConstant();

    _front_nose.spring().setChannelMass(getMass() / 8);
    _front_nose.setContactPoint( Vector3D(0.0f,3.31f,-17.38f) );
    _front_nose.spring().setMinimumDistance( 0.2f );
    _front_nose.spring().setEquilibriumDistance( 0.6f );
    _front_nose.spring().setLength( 0.8f);
    _front_nose.spring().setSpringMass( 300.0f );
    _front_nose.spring().setContactFrictionCoefficient( 20.0f );
    _front_nose.spring().calculateSpringConstant();

    _rear_body.spring().setChannelMass(getMass() / 8);
    _rear_body.setContactPoint( Vector3D(0.0f,5.07f,20.30f) );
    _rear_body.spring().setMinimumDistance( 0.2f );
    _rear_body.spring().setEquilibriumDistance( 0.6f );
    _rear_body.spring().setLength( 0.8f);
    _rear_body.spring().setSpringMass( 300.0f );
    _rear_body.spring().setContactFrictionCoefficient( 20.0f );
    _rear_body.spring().calculateSpringConstant();

    _above_vtail.spring().setChannelMass(getMass()/6.0f);
    _above_vtail.setContactPoint( Vector3D(0.0f,12.10f,19.0f) );
    _above_vtail.spring().setMinimumDistance( 0.2f );
    _above_vtail.spring().setEquilibriumDistance( 0.4f );
    _above_vtail.spring().setLength( 0.6f);
    _above_vtail.spring().setSpringMass( 300 );
    _above_vtail.spring().setContactFrictionCoefficient( 20.0f );
    _above_vtail.spring().calculateSpringConstant();

    _left_engine.setContactPoint( Vector3D(-5.82f, 1.71f, -5.07f ) );
    _right_engine.setContactPoint( Vector3D(5.82f, 1.71f, -5.07f ) );
    _front_wheel.spring().setDrivingState( SpringModel::DrivingState::NEUTRAL );
#pragma endregion

    addForceGenerators();
    initSpeedAndPos();
}

bool BAAirbus320JSONRigidBody::onMouseWheel(int wheelDelta)
{
    //void incrPower(int engine) from UI

    //engine =-1 all engines

    _left_engine._thrust_percent += wheelDelta;
    _right_engine._thrust_percent += wheelDelta;
    return true;
}

float BAAirbus320JSONRigidBody::getPower(int engine)
{
    if (engine < 0 || engine > 1)
        return 0.0f;

    if (engine == 1)
        return _left_engine._thrust_percent;

    return _right_engine._thrust_percent;
}

float BAAirbus320JSONRigidBody::getPowerOutput(int engine)
{
    if (engine < 0 || engine > 1)
        return 0.0f;

    if (engine == 1)
        return _left_engine._actual_thrust_percent;

    return _right_engine._actual_thrust_percent;
}

void BAAirbus320JSONRigidBody::setPower(float value)
{
    _left_engine._thrust_percent = value;
    _right_engine._thrust_percent = value;
}

void BAAirbus320JSONRigidBody::applyBrakes(float dt, bool bShift)
{
    double max_decel = 6.0f;
    double max_accel = 6.0f;
   // max_decel = 12.0f;

    max_decel = 3.0f;

    if( bShift )
    {
        _left_wheel.spring().setDrivingState( SpringModel::DrivingState::BRAKE );
        _right_wheel.spring().setDrivingState( SpringModel::DrivingState::BRAKE );
    }
    else
    {
        _left_wheel.spring().setDrivingState( SpringModel::DrivingState::NEUTRAL );
        _right_wheel.spring().setDrivingState( SpringModel::DrivingState::NEUTRAL );

        if( _left_wheel.spring().getWheelSpeed()  > 0 )
            _left_wheel.spring().incrWheelSpeed( - std::min( max_decel * dt, _left_wheel.spring().getWheelSpeed() ));
        else
            _left_wheel.spring().incrWheelSpeed( std::min( max_decel * dt, (-_left_wheel.spring().getWheelSpeed()) ));

        if( _right_wheel.spring().getWheelSpeed()  > 0 )
            _right_wheel.spring().incrWheelSpeed( - std::min( max_decel * dt, _right_wheel.spring().getWheelSpeed() ));
        else
            _right_wheel.spring().incrWheelSpeed(std::min( max_decel * dt, (-_right_wheel.spring().getWheelSpeed()) ) );
    }
}

bool BAAirbus320JSONRigidBody::onAsyncKeyPress(IScreenMouseInfo *scrn, float dt)
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

    if(
        #ifdef ANDROID
            isUsingMouse() &&
        #endif
            joy && joy->isAvailable() )
    {
        float fAileron(0.0f);
        float fPitch(0.0f);
        float fYaw(0.0f);
        float fThrust(0.0f);

        fAileron = joy->joyGetX();
        fPitch = joy->joyGetY();

#ifdef ANDROID
        HeightData hd;
        _custom_fg.getWorld()->getHeightFromPosition(getGPSLocation(), hd);

        fAileron *= 2.0f;
        if( hd.Height() < 5.0)
        	fYaw = fAileron;

        fPitch -= sin( 40/180.0f * M_PI);
        fPitch *= 2.0f;
#else
        fYaw = joy->joyGetZ() * 2.0f;

        fThrust = std::fabs(joy->joyGetV()) > 0.2f ? joy->joyGetV() : 0.0f;
#endif
        _right_engine._thrust_percent -= fThrust;

        fThrust = _right_engine._thrust_percent;

        _front_wheel.spring().setSteeringAngle(fYaw * 55.0f);
        hydraulics().setDeflection(_vertical_tail.element(0)->controlSurfaceN(0), fYaw * 15.0f );
        _right_engine._thrust_percent = fThrust;

        hydraulics().setDeflection( _left_wing.element(0)->controlSurfaceN(0), fAileron * 15.0f );
        hydraulics().setDeflection( _right_wing.element(0)->controlSurfaceN(0), -fAileron * 15.0f );

        hydraulics().setDeflection( _left_tail_wing.element(0)->controlSurfaceN(0), -fPitch * 20.0f );
        hydraulics().setDeflection( _right_tail_wing.element(0)->controlSurfaceN(0), -fPitch * 20.0f );

        float deflection_l = _left_tail_wing.controlSurface0()->getDeflection();

#ifdef ANDROID
        if( hd.Height() < 1.5)
        	deflection_l = 0;
        else
#endif
        	deflection_l -= dt * fPitch * 4;

        float MAX_DEFL = 10;
        if( deflection_l < -MAX_DEFL )
            deflection_l = -MAX_DEFL;
        if( deflection_l > MAX_DEFL )
            deflection_l = MAX_DEFL;

        _left_tail_wing.controlSurface0()->setDeflection(deflection_l);
        _right_tail_wing.controlSurface0()->setDeflection(deflection_l);

        if( _right_engine._thrust_percent > 100 ) _right_engine._thrust_percent  = 100;
        if( _right_engine._thrust_percent < 0 ) _right_engine._thrust_percent = 0;

        _left_engine._thrust_percent = _right_engine._thrust_percent;

        if( joy->numberofButtonsPressed())
        {
            int dwWord = joy->buttonFlagPressed();

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

            if( dwWord & 2) //Regular Braking.
                applyBrakes(dt,false);
        }
    }

#ifdef WIN32
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
        _front_wheel.spring().incrSteeringAngle( -dt*30 );
        float deflection = _front_wheel.spring().getSteeringAngle() ;
        if( deflection < -25 )	deflection = -25;
        if( deflection > 25 )	deflection = 25;
        _vertical_tail.element(0)->controlSurfaceN(0)->setDeflection(deflection);

    }

    if( GetAsyncKeyState( '8') < 0 )
    {
        _front_wheel.spring().incrSteeringAngle( dt*30 );
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
        hydraulics().setDeflection( _left_tail_wing.controlSurface0(), 0.0f );
        hydraulics().setDeflection( _right_tail_wing.controlSurface0(), 0.0f );
    }

    if( GetAsyncKeyState( '9' ) < 0 )
    {
        _front_wheel.spring().setWheelSpeed(-std::min(fabs(_front_wheel.spring().getWheelSpeed()-max_accel* dt),100.0));
        _left_wheel.spring().setWheelSpeed(-std::min(fabs(_left_wheel.spring().getWheelSpeed()-max_accel* dt),100.0));
        _right_wheel.spring().setWheelSpeed(-std::min(fabs(_right_wheel.spring().getWheelSpeed()-max_accel* dt),100.0));
    }

    if( GetAsyncKeyState( VK_NEXT ) < 0 )
    {
        _left_engine._thrust_percent -= dt*20;
        _right_engine._thrust_percent -= dt*20;
    }

    if( GetAsyncKeyState( VK_PRIOR ) < 0 )
    {
        _left_engine._thrust_percent += dt*20;
        _right_engine._thrust_percent += dt*20;
    }

    if( GetAsyncKeyState( 'B' ) < 0 )
        applyBrakes(dt, GetAsyncKeyState( VK_SHIFT ) < 0);

    if( isUsingMouse() )
    {
        //IMouse* mouse = InputHandler::getMouse();

        int x, y;
        scrn->GetMousePos(x, y);

        int width, height;
        scrn->GetScreenDims(width, height);

        int mx = width /2 ;
        int my = height /2 ;

        float dx = (x - mx) / 20.0f;
        float dy = (y - my) / 20.0f;

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

            /*if( velocity().Magnitude() < 180 )
            {
                if( dx < -10 )
                    hydraulics().setDeflection( _left_wing.element(0)->controlSurfaceN(2), (dx+10)/4.0f);

                if( dx > 10 )
                    hydraulics().setDeflection( _right_wing.element(0)->controlSurfaceN(2), -(dx+10)/4.0f);
            }*/
        }

        hydraulics().setDeflection( _left_tail_wing.element(0)->controlSurfaceN(0), -dy );
        hydraulics().setDeflection( _right_tail_wing.element(0)->controlSurfaceN(0), -dy );

        float deflection_l = _left_tail_wing.controlSurface0()->getDeflection();
        deflection_l -= dt * dy/20;

        float MAX_DEFL = 10;
        if( deflection_l < -MAX_DEFL )
            deflection_l = -MAX_DEFL;

        _left_tail_wing.controlSurface0()->setDeflection(deflection_l);
        _right_tail_wing.controlSurface0()->setDeflection(deflection_l);



        dx = (x - mx) / 10.0f /2.0f;

        _front_wheel.spring().setSteeringAngle(dx);

        dx /= 2.0f *4.0f;

        if( dx < -25 ) dx = -25;
        if( dx > 25 ) dx = 25;

        hydraulics().setDeflection( _vertical_tail.element(0)->controlSurfaceN(0), dx );
    }
#endif


    if( _front_wheel.spring().getSteeringAngle() < -75 )	_front_wheel.spring().setSteeringAngle( -75);
    if( _front_wheel.spring().getSteeringAngle() > 75 )	_front_wheel.spring().setSteeringAngle(75);

    if( _left_engine._thrust_percent <0 )		_left_engine._thrust_percent = 0.0f;
    if( _right_engine._thrust_percent <0 )		_right_engine._thrust_percent = 0.0f;
    if( _left_engine._thrust_percent >100 )		_left_engine._thrust_percent = 100.0f;
    if( _right_engine._thrust_percent >100 )		_right_engine._thrust_percent = 100.0f;


    return true;
}

void BAAirbus320JSONRigidBody::updateCameraView()
{
    JSONRigidBody::updateCameraView();
    CameraView* view = getCameraProvider()->getCameraView();

    switch((ViewPosition)getCameraProvider()->curViewIdx())
    {
    case CockpitForwards :
        view->setPosition(toNonLocalTranslateFrame(Vector3D(-0.45, 4.80, -15.0)));
        break;
    case PassengerLeftMiddle :
    {
        QuarternionD q = MathSupport<double>::MakeQHeading(90);
        Vector3D e = -MathSupport<double>::MakeEuler(getGroundOrientation() * q);
        view->setPosition(toNonLocalTranslateFrame(Vector3D(1.7, 4.5, 3.4)));
        view->setOrientation(e.toFloat());
        view->setDescription("LeftSeat");
    }
        break;
    case RightGearWheel :
        break;
    }
}

void BAAirbus320JSONRigidBody::airResetPos()
{
    reset();
    initSpeedAndPos();
}

void BAAirbus320JSONRigidBody::airResetApproachPos()
{
    double fMilesOut = 3.5f;
    reset();

    setPosition(GPSLocation(51.4648,-0.4719087,fMilesOut * 350/3.2808) + Vector3D( -fMilesOut  * 5280 /3.2808f, 0, rand()%500-250));
    setEuler( 0, 90, 0);

    double dSpeed = 160 * 1.15f * 1.609334f / 3600 * 1000 ;
    setVelocity(toNonLocalFrame( Vector3D(0, 0, -dSpeed) ) );
    setAngularVelocity( 0, 0, 0 );
}

void BAAirbus320JSONRigidBody::airSpoilerToggle(bool bLeft)
{
    if( bLeft)
    {
        AeroControlSurface *pML = _left_wing.element(0)->controlSurfaceN(2);
        AeroControlSurface *pInnerML = _left_wing.element(1)->controlSurfaceN(1);

        if( hydraulics().getDeflection( pML ) == 0 )
        {
            hydraulics().setDeflection( pML, -50.0f );
            hydraulics().setDeflection( pInnerML, -50.0f );
        }
        else
        {
            hydraulics().setDeflection( pML, 0.0f );
            hydraulics().setDeflection( pInnerML, 0.0f );
        }
    }
    else
    {
        AeroControlSurface *pMR = _right_wing.element(0)->controlSurfaceN(2);
        AeroControlSurface *pInnerML = _right_wing.element(1)->controlSurfaceN(1);

        if( hydraulics().getDeflection( pMR ) == 0 )
        {
            hydraulics().setDeflection( pMR, -50.0f );
            hydraulics().setDeflection( pInnerML, -50.0f );
        }
        else
        {
            hydraulics().setDeflection( pMR, 0.0f );
            hydraulics().setDeflection( pInnerML, 0.0f );
        }
    }
}

void BAAirbus320JSONRigidBody::airFlapIncr(int incr)
{
    AeroControlSurface* pML = _left_wing.element(0)->controlSurfaceN(1);
    AeroControlSurface* pMR = _right_wing.element(0)->controlSurfaceN(1);

    AeroControlSurface* pInnerML = _left_wing.element(1)->controlSurfaceN(0);
    AeroControlSurface* pInnerMR = _right_wing.element(1)->controlSurfaceN(0);

    float fDeflection = hydraulics().getDeflection(pML);

    if(incr > 0)
    {
        if( fDeflection == 0.0f )
            fDeflection = 1.0f;
        else if( fDeflection == 1.0f )
            fDeflection = 5.0f;
        else if( fDeflection == 5.0f )
            fDeflection = 10.0f;
        else if( fDeflection == 10.0f )
            fDeflection = 20.0f;
        else
            fDeflection = 40.0f;
    }
    else
    {
        if( fDeflection == 40.0f )
            fDeflection = 20.0f;
        else if( fDeflection == 20.0f )
            fDeflection = 10.0f;
        else if( fDeflection == 10.0f )
            fDeflection = 5.0f;
        else if( fDeflection == 5.0f )
            fDeflection = 1.0f;
        else
            fDeflection = 0.0f;
    }

    float fFlapFactor = 4.0f;
    hydraulics().setDeflection( pML, fDeflection /fFlapFactor  );
    hydraulics().setDeflection( pMR, fDeflection /fFlapFactor );
    hydraulics().setDeflection( pInnerML, fDeflection/fFlapFactor  );
    hydraulics().setDeflection( pInnerMR, fDeflection /fFlapFactor );
}

void BAAirbus320JSONRigidBody::applyBrakes(bool bApply)
{
    if( bApply)
    {
        _left_wheel.spring().setDrivingState( SpringModel::DrivingState::BRAKE );
        _right_wheel.spring().setDrivingState( SpringModel::DrivingState::BRAKE );
    }
    else
    {
        _left_wheel.spring().setDrivingState( SpringModel::DrivingState::NEUTRAL );
        _right_wheel.spring().setDrivingState( SpringModel::DrivingState::NEUTRAL );
    }
}

bool BAAirbus320JSONRigidBody::onSyncKeyPress()
{
    return false;
}

void BAAirbus320JSONRigidBody::initSpeedAndPos()
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
    setEuler( 5,270,-0.1);

#if defined LOCATED_AT_GIBRALTER
    setEuler(0, 270, 0);
    setPosition( GPSLocation(36.151473, -5.339665,5));
#else
    setPosition( GPSLocation(51.464951, -0.434115,22));
#endif
}

void BAAirbus320JSONRigidBody::addForceGenerators()
{
    std::string path = getPath();

    if( !_wingAoaData.load( path + "a320.afl" ) )
        std::cout << "Failed to load WingData : a320.afl";

    if( !_rudderAoaData.load( path + "NACA 0009 (symmetrical).afl" ) )
        std::cout << "Failed to load NACA 0009 (symmetrical).afl : NACA 0009 (symmetrical)";

    if( !_elevatorAoaData.load( path + "NACA 0009 (symmetrical).afl" ) )
        std::cout << "Failed to load NACA 0009 (symmetrical).afl : NACA 0009 (symmetrical)";

    addForceGenerator( "gravityFG", &_gravityFG, 15 );

    addForceGenerator( "vertical_tail", &_vertical_tail, 15 );

    addForceGenerator( "left_wing", &_left_wing, 15 );
    addForceGenerator( "right_wing", &_right_wing, 15 );

    addForceGenerator( "left_tail_wing", &_left_tail_wing, 15 );
    addForceGenerator( "right_tail_wing", &_right_tail_wing, 15 );

    addForceGenerator( "left_engine", &_left_engine, 15 );
    addForceGenerator( "right_engine", &_right_engine, 15 );


    int fps = 150;

    addForceGenerator( "custom_fg", &_custom_fg );

    addForceGenerator( "left_engine_under_pod", &_left_engine_under_pod, fps );
    addForceGenerator( "right_engine_under_pod", &_right_engine_under_pod, fps );

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
    addForceGenerator( "left_wheel", &_left_wheel, fps );
    addForceGenerator( "right_wheel", &_right_wheel, fps );

    //MOJ_JEB
    //WheelConstrainedSpring<0,BAAirbus320JSONRigidBody>::Attach( this, fps);

    JSONRigidBodyBuilder body(this);

    _left_engine.setMaxSpd(0.82f);
    _right_engine.setMaxSpd(0.82f);
    double cl, cd, cm;

    try
    {
        body.build(path + "BAAirbusA320.body");

        float area(0.0f);
        // Total Area : test.
        {

            for( size_t i=0; i < _left_wing.size(); i++ )
                area += _left_wing.element(i)->getArea();

            for( size_t i=0; i < _right_wing.size(); i++ )
                area += _right_wing.element(i)->getArea();

            _wingAoaData.ClCdCm(0, cl, cd, cm);
            _left_engine.incrDrag(cd, area);
            _right_engine.incrDrag(cd, area);

        }

        float area2(0.0f);
        // Total Area : test.
        {

            for (size_t i = 0; i < _left_tail_wing.size(); i++)
                area2 += _left_tail_wing.element(i)->getArea();

            for (size_t i = 0; i < _right_tail_wing.size(); i++)
                area2 += _right_tail_wing.element(i)->getArea();

            _elevatorAoaData.ClCdCm(0, cl, cd, cm);
            _left_engine.incrDrag(cd, area2);
            _right_engine.incrDrag(cd, area2);
        }

        float area3(0.0f);
        // Total Area : test.
        {

            for (size_t i = 0; i < _vertical_tail.size(); i++)
                area3 += _vertical_tail.element(i)->getArea();

            for (size_t i = 0; i < _vertical_tail.size(); i++)
                area3 += _vertical_tail.element(i)->getArea();

            _rudderAoaData.ClCdCm(0, cl, cd, cm);
            _left_engine.incrDrag(cd, area3);
            _right_engine.incrDrag(cd, area3);

        }



        float totArea = area + area2 + area3;

        float fResponseRate = 80.0f;
        hydraulics().setResponseRate( _vertical_tail.element(0)->controlSurfaceN(0), fResponseRate );

        hydraulics().setResponseRate( _left_wing.element(0)->controlSurfaceN(0), fResponseRate);
        hydraulics().setResponseRate( _left_wing.element(0)->controlSurfaceN(2), 40 );
        hydraulics().setResponseRate( _left_wing.element(1)->controlSurfaceN(1), 80 );

        hydraulics().setResponseRate( _right_wing.element(0)->controlSurfaceN(0), fResponseRate );
        hydraulics().setResponseRate( _right_wing.element(0)->controlSurfaceN(2), 40 );
        hydraulics().setResponseRate( _right_wing.element(1)->controlSurfaceN(1), 80 );

        hydraulics().setResponseRate( _left_tail_wing.controlSurface0(), 1.0f );
        hydraulics().setResponseRate( _left_tail_wing.element(0)->controlSurfaceN(0), fResponseRate );

        hydraulics().setResponseRate( _right_tail_wing.controlSurface0(), 1.0f );
        hydraulics().setResponseRate( _right_tail_wing.element(0)->controlSurfaceN(0), fResponseRate );

    }
    catch( const std::string& str )
    {
        std::cout << str << " : JSON config parse failed";
    }
}

float BAAirbus320JSONRigidBody::airGetAileron() const
{
	return _left_wing.element(0)->controlSurfaceN(0)->getDeflection();
}

float BAAirbus320JSONRigidBody::airGetPitch() const
{
	return _left_tail_wing.element(0)->controlSurfaceN(0)->getDeflection();
}


/////////////////////////////////////////////////////////////////////////////////////////////////

