#pragma once

#include <QGuiApplication>

template<class T>
class PiccadillyTrainARigidbody;

template<class T>
class PiccadillyLineTrainACustomForceGenerator : public GSForceGenerator
{
public:
	FORCEGENERATOR_TYPE(Type_Custom)

	PiccadillyLineTrainACustomForceGenerator ()
		:_hasFocus(false),_bDrivingByMouse(false)
	{
	}

	bool _hasFocus;

	void onApplyForce(Particle *p, double dt) override
	{		
		if( !_hasFocus)
			return;

		PiccadillyTrainARigidbody<T> *pBody = static_cast<PiccadillyTrainARigidbody<T>*>(p);

		/*float max_decel = 6.0f;
		float max_accel = 6.0f;
		float goodForce = 10000.0f;
		pBody->setBodyFrame();

		if( GetAsyncKeyState( 'L' ) < 0 )
			pBody->applyForce( VectorD(0,0,0),pBody->toLocalGroundFrame(VectorD(0, goodForce*3 ,0)));

		if( GetAsyncKeyState( 'J' ) < 0 )
			pBody->applyForce( VectorD(0,1,2), VectorD(goodForce, 0 ,0));

		if( GetAsyncKeyState( 'K' ) < 0 )
			pBody->applyForce( VectorD(0,1,2), VectorD(-goodForce, 0 ,0));

		if( _hasFocus && _bDrivingByMouse )
		{
			POINT cursorPt;
			GetCursorPos(&cursorPt);
			float dMouseY = (cursorPt.y - GetSystemMetrics(SM_CYSCREEN)/2)/20.0f;
			max_accel = -(dMouseY - pBody->_leftFrontWheel._wheel_linear_velocity);

			if( fabs( dMouseY ) < 5 )
			{
				pBody->_leftFrontWheel.spring().setDrivingState( SpringModel::DrivingState::NEUTRAL );
				pBody->_rightFrontWheel.spring().setDrivingState( SpringModel::DrivingState::NEUTRAL );
				pBody->_leftRearWheel.spring().setDrivingState( SpringModel::DrivingState::NEUTRAL );
				pBody->_rightRearWheel.spring().setDrivingState( SpringModel::DrivingState::NEUTRAL );
			}
			else
			{
				pBody->_leftFrontWheel.spring().getDrivingState() = SpringModel::DrivingState::POWERED;
				pBody->_rightFrontWheel.spring().getDrivingState() = SpringModel::DrivingState::POWERED;
				pBody->_leftRearWheel.spring().getDrivingState() = SpringModel::DrivingState::POWERED;
				pBody->_rightRearWheel.spring().getDrivingState() = SpringModel::DrivingState::POWERED;
			}

			if( max_accel > 0 )
				max_accel = min(18,max_accel);
			else
				max_accel = max(-18, max_accel);


			 float towAngle = (cursorPt.x- GetSystemMetrics(SM_CXSCREEN)/2)/16;
			if( towAngle < -25 ) towAngle = -25;
			if( towAngle > 25 ) towAngle = 25;
			pBody->_leftFrontWheel.spring().getSteeringAngle() = towAngle;
			pBody->_rightFrontWheel.spring().getSteeringAngle() = towAngle;
		}

		if( GetAsyncKeyState( 'I') < 0 || (_hasFocus && _bDrivingByMouse))
		{
			pBody->_leftFrontWheel._wheel_linear_velocity -= max_accel * dt;
			pBody->_rightFrontWheel._wheel_linear_velocity -= max_accel * dt;

			pBody->_leftRearWheel._wheel_linear_velocity -= max_accel * dt;
			pBody->_rightRearWheel._wheel_linear_velocity -= max_accel * dt;
		}

		if( GetAsyncKeyState( 'M' ) < 0 )
		{
			pBody->_leftFrontWheel._wheel_linear_velocity += max_accel * dt;
			pBody->_rightFrontWheel._wheel_linear_velocity += max_accel * dt;

			pBody->_leftRearWheel._wheel_linear_velocity += max_accel * dt;
			pBody->_rightRearWheel._wheel_linear_velocity += max_accel * dt;
		}

		if( GetAsyncKeyState( 'B' ) < 0 && GetAsyncKeyState( VK_SHIFT ) < 0 )
		{
			pBody->_leftFrontWheel.spring().setDrivingState( SpringModel::DrivingState::BRAKE );
			pBody->_rightFrontWheel.spring().setDrivingState( SpringModel::DrivingState::BRAKE );
			pBody->_leftRearWheel.spring().setDrivingState( SpringModel::DrivingState::BRAKE );
			pBody->_rightRearWheel.spring().setDrivingState( SpringModel::DrivingState::BRAKE );

		}

		if( GetAsyncKeyState( 'B' ) < 0 && GetAsyncKeyState( VK_SHIFT ) >= 0 )
		{
			if( pBody->_leftFrontWheel._wheel_linear_velocity  > 0 )
				pBody->_leftFrontWheel._wheel_linear_velocity -= min( max_decel * dt, pBody->_leftFrontWheel._wheel_linear_velocity );
			else
				pBody->_leftFrontWheel._wheel_linear_velocity += min( max_decel * dt, (-pBody->_leftFrontWheel._wheel_linear_velocity) );

			if( pBody->_rightFrontWheel._wheel_linear_velocity  > 0 )
				pBody->_rightFrontWheel._wheel_linear_velocity -= min( max_decel * dt, pBody->_rightFrontWheel._wheel_linear_velocity );
			else
				pBody->_rightFrontWheel._wheel_linear_velocity += min( max_decel * dt, (-pBody->_rightFrontWheel._wheel_linear_velocity) );


			if( pBody->_leftRearWheel._wheel_linear_velocity  > 0 )
				pBody->_leftRearWheel._wheel_linear_velocity -= min( max_decel * dt, pBody->_leftRearWheel._wheel_linear_velocity );
			else
				pBody->_leftRearWheel._wheel_linear_velocity += min( max_decel * dt, (-pBody->_leftRearWheel._wheel_linear_velocity) );

			if( pBody->_rightRearWheel._wheel_linear_velocity  > 0 )
				pBody->_rightRearWheel._wheel_linear_velocity -= min( max_decel * dt, pBody->_rightRearWheel._wheel_linear_velocity );
			else
				pBody->_rightRearWheel._wheel_linear_velocity += min( max_decel * dt, (-pBody->_rightRearWheel._wheel_linear_velocity) );

		}*/
	}
};

template<class T>
class PiccadillyTrainARigidbody : 
	public JSONRigidBody
{
public:
	PiccadillyTrainARigidbody();

	PiccadillyLineTrainACustomForceGenerator<T> _custom_fg;

	GravityForceGenerator _gravityFG;
	
	WheelSpringForceGenerator _leftFrontWheel;
	WheelSpringForceGenerator _rightFrontWheel;
	WheelSpringForceGenerator _leftRearWheel;
	WheelSpringForceGenerator _rightRearWheel;

	float _RightDoorCSubASlider;
	bool _RightDoorCSubASliderClosing;
	DWORD _RightKeyTimePress;


	float _LeftDoorCSubASlider;
	bool _LeftDoorCSubASliderClosing;
	DWORD _LeftKeyTimePress;

	void update(double dt)
	{
		JSONRigidBody::update(dt);

		if( GetAsyncKeyState( '1' ) < 0 && (GetTickCount() - _RightKeyTimePress)> 200 )
		{
			_RightDoorCSubASliderClosing = !_RightDoorCSubASliderClosing;
			_RightKeyTimePress = GetTickCount();
		}

		if( _RightDoorCSubASliderClosing == false && fabs(_RightDoorCSubASlider - 1) < FLT_EPSILON )
		{
			
		}
		else if( _RightDoorCSubASliderClosing == true && fabs(_RightDoorCSubASlider ) < FLT_EPSILON )
		{
		}
		else
		{
			if( _RightDoorCSubASliderClosing )
				_RightDoorCSubASlider -= dt / 1.8;
			else
				_RightDoorCSubASlider += dt / 1.8;

			if( _RightDoorCSubASlider < 0 )
				_RightDoorCSubASlider = 0;
			if( _RightDoorCSubASlider > 1 ) 
				_RightDoorCSubASlider = 1;
		}

		if( GetAsyncKeyState( '2' ) < 0 && (GetTickCount() - _LeftKeyTimePress)> 200 )
		{
			_LeftDoorCSubASliderClosing = !_LeftDoorCSubASliderClosing;
			_LeftKeyTimePress = GetTickCount();
		}

		if( _LeftDoorCSubASliderClosing == false && fabs(_LeftDoorCSubASlider - 1) < FLT_EPSILON )
		{
		}
		else if( _LeftDoorCSubASliderClosing == true && fabs(_LeftDoorCSubASlider ) < FLT_EPSILON )
		{
			
		}
		else
		{
			if( _LeftDoorCSubASliderClosing )
				_LeftDoorCSubASlider -= dt / 1.8;
			else
				_LeftDoorCSubASlider += dt / 1.8;

			if( _LeftDoorCSubASlider < 0 )
				_LeftDoorCSubASlider = 0;
			if( _LeftDoorCSubASlider > 1 ) 
				_LeftDoorCSubASlider = 1;
		}
		
	}

	void draw(float fLightFraction)
	{
		T* t = static_cast<T*>(this);
		t->renderMesh(fLightFraction,false);
	}

	void drawShadow(float fLightFraction)
	{
		if( fLightFraction < 0.5f )
			return;

		T* t = static_cast<T*>(this);
		t->_bshadow = true;
		t->renderMesh(fLightFraction,true);
		t->_bshadow = false;
	}

	void drawWindTunnel(double dt)
	{
	}

	void setFocus(bool value)
	{
		_custom_fg._hasFocus = value;
	}

	bool hasFocus()
	{
		return _custom_fg._hasFocus;
	}

	void initSpeedAndPos()
	{
		setVelocity(0,0,0);
		setAngularVelocity(0,0,0);
		setOrientation(0,0,0);
	}

	virtual void setRecorderHook(FlightRecorder& a) override
	{
		JSONRigidBody::setRecorderHook(a);
		a.addDataRef( _LeftDoorCSubASlider );
		a.addDataRef( _RightDoorCSubASlider );
	}

	void reset() override
	{
		JSONRigidBody::reset();
		_pJsonBody->reset();
	}

	void Attach( JSONRigidBodyCollection *pParent )
	{
		//_pJsonBody = pParent->createJSONBody( this, VectorD(0,0,0), JSONRigidBody::TYPE_HAS_NO_SHADOW );
		/*_pJsonBody->add( &_gravityFG, 15 );

		float fps = 150;

		_pJsonBody->add( &_custom_fg );
		_pJsonBody->add( &_leftFrontWheel, fps );
		_pJsonBody->add( &_rightFrontWheel, fps );
		_pJsonBody->add( &_leftRearWheel, fps );
		_pJsonBody->add( &_rightRearWheel, fps );*/

		/*pJSONBody->add( &_frontMiddle , fps );
		pJSONBody->add( &_leftMiddle, fps );
		pJSONBody->add( &_rightMiddle, fps );
		pJSONBody->add( &_rearMiddle, fps );

		pJSONBody->add( &_topLeftFront, fps );
		pJSONBody->add( &_topRightFront, fps );
		pJSONBody->add( &_topLeftRear, fps );
		pJSONBody->add( &_topRightRear, fps );*/
	}

	JSONRigidBody *_pJsonBody;
};

template<class T> 
PiccadillyTrainARigidbody <T>::PiccadillyTrainARigidbody () : 
	_pJsonBody (NULL), JSONRigidBody("Piccadilly Line", nullptr)
{
	setMass( 1833.5 );

	setInertiaMatrix( Matrix3x3D( 3612.6822, 0, 0*26.1463047,
					0, 4784.15374, 0,
					0*26.1463047, 0, 1171.47155 ) );
	setCG(VectorD(0.12*0, 0.6, -0.4+0.11) );

	_RightDoorCSubASlider = 0.0;
	_RightDoorCSubASliderClosing = true;
	_RightKeyTimePress = 0;

	_LeftDoorCSubASlider = 0.0;
	_LeftDoorCSubASliderClosing = true;
	_LeftKeyTimePress = 0;

	//Tension
	//Front wheels : 28000 Newtons
	//Rear Wheels : 46000 Newtons

	/*double dLength = 0.26;
	double dEquib = 0.20;//0.15;
	double dMin = 0.1;
	double dSpringFriction = 8000.0;
	double dWheelRadius = 0.35577;//0.36;
	double dRest = 0.3;
	double dContactFriction = 15.0;

	_leftFrontWheel.spring().setChannelMass( 510 );
	_leftFrontWheel.setContactPoint( VectorD(-0.80184, 0, -1.489 ));
	_leftFrontWheel.spring().setMinimumDistance( dMin );
	_leftFrontWheel.spring().setEquilibriumDistance( dEquib );
	_leftFrontWheel.spring().setLength( dLength );
	_leftFrontWheel.spring().setRestitution( dRest );

	_leftFrontWheel.spring().setSpringFriction( dSpringFriction*2 );
	_leftFrontWheel.spring().setSpringMass( 318 );
	_leftFrontWheel.spring().setContactFrictionCoefficient( dContactFriction );
	_leftFrontWheel.spring().calculateSpringConstant();
	_leftFrontWheel.spring().setWheelRadius( dWheelRadius;

	//////////////////////////////////////////////////////

	_rightFrontWheel.spring().setChannelMass( 565 );
	_rightFrontWheel.setContactPoint( VectorD(0.80184, 0, -1.489 ));
	_rightFrontWheel.spring().setMinimumDistance( dMin );
	_rightFrontWheel.spring().setEquilibriumDistance( dEquib );
	_rightFrontWheel.spring().setLength( dLength );
	_rightFrontWheel.spring().setRestitution( dRest );

	_rightFrontWheel.spring().setSpringFriction( dSpringFriction*2 );
	_rightFrontWheel.spring().setSpringMass( 353 );
	_rightFrontWheel.spring().setContactFrictionCoefficient( dContactFriction );
	_rightFrontWheel.spring().calculateSpringConstant();
	_rightFrontWheel.spring().setWheelRadius( dWheelRadius;

	//////////////////////////////////////////////////////

	_leftRearWheel.spring().setChannelMass( 370 );
	_leftRearWheel.setContactPoint( VectorD(-0.80184, 0, 1.36414 ));
	_leftRearWheel.spring().setMinimumDistance( dMin );
	_leftRearWheel.spring().setEquilibriumDistance( dEquib );
	_leftRearWheel.spring().setLength( dLength );
	_leftRearWheel.spring().setRestitution( dRest );

	_leftRearWheel.spring().setSpringFriction( dSpringFriction );
	_leftRearWheel.spring().setSpringMass( 231 );
	_leftRearWheel.spring().setContactFrictionCoefficient( dContactFriction );
	_leftRearWheel.spring().calculateSpringConstant();
	_leftRearWheel.spring().setWheelRadius( dWheelRadius;


	//////////////////////////////////////////////////////

	_rightRearWheel.spring().setChannelMass( 389 );
	_rightRearWheel.setContactPoint( VectorD(0.80184, 0, 1.36414 ));
	_rightRearWheel.spring().setMinimumDistance( dMin );
	_rightRearWheel.spring().setEquilibriumDistance( dEquib );
	_rightRearWheel.spring().setLength( dLength );
	_rightRearWheel.spring().setRestitution( dRest );

	_rightRearWheel.spring().setSpringFriction( dSpringFriction );
	_rightRearWheel.spring().setSpringMass( 243 );
	_rightRearWheel.spring().setContactFrictionCoefficient( dContactFriction );
	_rightRearWheel.spring().calculateSpringConstant();
	_rightRearWheel.spring().setWheelRadius( dWheelRadius;*/

}

//////////////////////////////////////////////////////////////////////////////////////////////

class PiccadillyLineTrainA : public PiccadillyTrainARigidbody<PiccadillyLineTrainA>
{
public:
	PiccadillyLineTrainA()
	{ 

	}

};
