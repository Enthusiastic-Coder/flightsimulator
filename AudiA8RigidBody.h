#pragma once

#include "JSONRigidBodyBuilder.h"
#include <QGuiApplication>
#include <QPoint>
#include <QCursor>
#include <QWindow>
#include <QGuiApplication>
#include <QSize>

template<class T>
class AudiA8RigidBody;

template<class T>
class AudiA8CustomForceGenerator : public GSForceGenerator
{
public:
	FORCEGENERATOR_TYPE(Type_Custom)

	AudiA8CustomForceGenerator ()
		:_hasFocus(false),_bDrivingByMouse(false)
	{
	}

	bool _hasFocus;

	void onApplyForce(Particle *p, double dt) override
	{		
		if( !_hasFocus)
			return;

		AudiA8RigidBody<T> *pBody = static_cast<AudiA8RigidBody<T>*>(p);

		float max_decel = 6.0f;
		float max_accel = 6.0f;
		float goodForce = 10000.0f;
		pBody->setBodyFrame();

		if( GetAsyncKeyState( 'L' ) < 0 )
			pBody->applyForce( Vector3D(0,0,0),pBody->toLocalGroundFrame(Vector3D(0, goodForce*3 ,0)));

		if( GetAsyncKeyState( 'J' ) < 0 )
			pBody->applyForce( Vector3D(0,1,2), Vector3D(goodForce, 0 ,0));

		if( GetAsyncKeyState( 'K' ) < 0 )
			pBody->applyForce( Vector3D(0,1,2), Vector3D(-goodForce, 0 ,0));

		if( _hasFocus && _bDrivingByMouse )
		{
            //POINT cursorPt;
            //GetCursorPos(&cursorPt);

            QPoint cursorPt = QCursor::pos();
            QSize sz = qApp->primaryScreen()->size();

            float dMouseY = cursorPt.y() - sz.height()/2.0f;
			max_accel = -dMouseY/40;

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


			 float towAngle = (cursorPt.x- GetSystemMetrics(SM_CXSCREEN)/2)/16.0f/2.0f;
			if( towAngle < -25 ) towAngle = -25;
			if( towAngle > 25 ) towAngle = 25;
			pBody->_leftFrontWheel.spring().getSteeringAngle() = towAngle;
			pBody->_rightFrontWheel.spring().getSteeringAngle() = towAngle;

			/*max_accel = 0.0;
			pBody->_leftFrontWheel.spring().getWheelSpeed() = dMouseY;
			pBody->_rightFrontWheel.spring().getWheelSpeed() = dMouseY;
			pBody->_leftRearWheel.spring().getWheelSpeed() = dMouseY;
			pBody->_rightRearWheel.spring().getWheelSpeed() =dMouseY;*/
		}

		if( GetAsyncKeyState( 'I') < 0 || (_hasFocus && _bDrivingByMouse))
		{
			pBody->_leftFrontWheel.spring().getWheelSpeed() -= max_accel * dt;
			pBody->_rightFrontWheel.spring().getWheelSpeed() -= max_accel * dt;

			pBody->_leftRearWheel.spring().getWheelSpeed() -= max_accel * dt;
			pBody->_rightRearWheel.spring().getWheelSpeed() -= max_accel * dt;
		}

		if( GetAsyncKeyState( 'M' ) < 0 )
		{
			pBody->_leftFrontWheel.spring().getWheelSpeed() += max_accel * dt;
			pBody->_rightFrontWheel.spring().getWheelSpeed() += max_accel * dt;

			pBody->_leftRearWheel.spring().getWheelSpeed() += max_accel * dt;
			pBody->_rightRearWheel.spring().getWheelSpeed() += max_accel * dt;
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
			if( pBody->_leftFrontWheel.spring().getWheelSpeed()  > 0 )
				pBody->_leftFrontWheel.spring().getWheelSpeed() -= min( max_decel * dt, pBody->_leftFrontWheel.spring().getWheelSpeed() );
			else
				pBody->_leftFrontWheel.spring().getWheelSpeed() += min( max_decel * dt, (-pBody->_leftFrontWheel.spring().getWheelSpeed()) );

			if( pBody->_rightFrontWheel.spring().getWheelSpeed()  > 0 )
				pBody->_rightFrontWheel.spring().getWheelSpeed() -= min( max_decel * dt, pBody->_rightFrontWheel.spring().getWheelSpeed() );
			else
				pBody->_rightFrontWheel.spring().getWheelSpeed() += min( max_decel * dt, (-pBody->_rightFrontWheel.spring().getWheelSpeed()) );


			if( pBody->_leftRearWheel.spring().getWheelSpeed()  > 0 )
				pBody->_leftRearWheel.spring().getWheelSpeed() -= min( max_decel * dt, pBody->_leftRearWheel.spring().getWheelSpeed() );
			else
				pBody->_leftRearWheel.spring().getWheelSpeed() += min( max_decel * dt, (-pBody->_leftRearWheel.spring().getWheelSpeed()) );

			if( pBody->_rightRearWheel.spring().getWheelSpeed()  > 0 )
				pBody->_rightRearWheel.spring().getWheelSpeed() -= min( max_decel * dt, pBody->_rightRearWheel.spring().getWheelSpeed() );
			else
				pBody->_rightRearWheel.spring().getWheelSpeed() += min( max_decel * dt, (-pBody->_rightRearWheel.spring().getWheelSpeed()) );

		}
	}
};

template<class T>
class AudiA8RigidBody : public JSONRigidBody
{
public:
	AudiA8RigidBody(JSONRigidBodyCollection *pParent);

	AudiA8CustomForceGenerator<T> _custom_fg;

	GravityForceGenerator _gravityFG;
	
	WheelSpringForceGenerator _leftFrontWheel;
	WheelSpringForceGenerator _rightFrontWheel;
	WheelSpringForceGenerator _leftRearWheel;
	WheelSpringForceGenerator _rightRearWheel;

	SpringForceGenerator _frontLeft, _frontRight;
	SpringForceGenerator _frontLeftCeiling, _frontRightCeiling;
	SpringForceGenerator _rearLeftCeiling, _rearRightCeiling;
	SpringForceGenerator _rearLeft, _rearRight;

	void update(double dt)
	{
		JSONRigidBody::update(dt);
	}

	void Render(float fLightFraction)
	{
		T* t = static_cast<T*>(this);
		t->renderMesh(fLightFraction, false);
	}

	void drawShadow(float fLightFraction)
	{
		if( fLightFraction < 0.5f )
			return;

		T* t = static_cast<T*>(this);
		t->_bshadow = true;
		t->renderMesh(fLightFraction, true);
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
	}

	void reset() override
	{
		JSONRigidBody::reset();
		_pJsonBody->reset();
	}

	void Attach( JSONRigidBodyCollection *pParent )
	{
		/*_pJsonBody = pParent->createJSONBody( this, VectorD(0,-0.15,0) );
		_pJsonBody->add( &_gravityFG, 15 );

		float fps = 150;

		_pJsonBody->add( &_custom_fg );
		_pJsonBody->add( &_leftFrontWheel, fps );
		_pJsonBody->add( &_rightFrontWheel, fps );
		_pJsonBody->add( &_leftRearWheel, fps );
		_pJsonBody->add( &_rightRearWheel, fps );


		_pJsonBody->add( &_frontLeft, fps );
		_pJsonBody->add( &_frontRight, fps );
		_pJsonBody->add( &_frontLeftCeiling, fps);
		_pJsonBody->add( &_frontRightCeiling, fps );
		_pJsonBody->add( &_rearLeftCeiling, fps );
		_pJsonBody->add( &_rearRightCeiling, fps );
		_pJsonBody->add( &_rearLeft, fps );
		_pJsonBody->add( &_rearRight, fps );*/
	}

	JSONRigidBody *_pJsonBody;
};

template<class T> 
AudiA8RigidBody <T>::AudiA8RigidBody (JSONRigidBodyCollection *pParent)
	: _pJsonBody (NULL), JSONRigidBody( "AudiA8", pParent )
{
	setMass( 1833.5 );

	setInertiaMatrix( Matrix3x3D( 3612.6822, 0, 0*26.1463047,
					0, 4784.15374, 0,
					0*26.1463047, 0, 1171.47155 ) );
	setCG(Vector3D(0.12*0, 0.6, -0.4+0.11) );

	//Tension
	//Front wheels : 28000 Newtons
	//Rear Wheels : 46000 Newtons

	double dLength = 0.26;
	double dEquib = 0.20;//0.15;
	double dMin = 0.1;
	double dSpringFriction = 8000.0;
	double dWheelRadius = 0.35577;//0.36;
	double dRest = 0.3;
	double dContactFriction = 15.0;

	_leftFrontWheel.spring().setChannelMass( 510 );
	_leftFrontWheel.setContactPoint( Vector3D(-0.80184, 0, -1.489 ));
	_leftFrontWheel.spring().setMinimumDistance( dMin );
	_leftFrontWheel.spring().setEquilibriumDistance( dEquib );
	_leftFrontWheel.spring().setLength( dLength );
	_leftFrontWheel.spring().setRestitution( dRest );

	_leftFrontWheel.spring().setSpringFriction( dSpringFriction*2 );
	_leftFrontWheel.spring().setSpringMass( 358 );
	_leftFrontWheel.spring().setContactFrictionCoefficient( dContactFriction );
	_leftFrontWheel.spring().calculateSpringConstant();
	_leftFrontWheel.spring().setWheelRadius( dWheelRadius );

	//////////////////////////////////////////////////////

	_rightFrontWheel.spring().setChannelMass( 565 );
	_rightFrontWheel.setContactPoint( Vector3D(0.80184, 0, -1.489 ));
	_rightFrontWheel.spring().setMinimumDistance( dMin );
	_rightFrontWheel.spring().setEquilibriumDistance( dEquib );
	_rightFrontWheel.spring().setLength( dLength );
	_rightFrontWheel.spring().setRestitution( dRest );

	_rightFrontWheel.spring().setSpringFriction( dSpringFriction*2 );
	_rightFrontWheel.spring().setSpringMass( 358 );
	_rightFrontWheel.spring().setContactFrictionCoefficient( dContactFriction );
	_rightFrontWheel.spring().calculateSpringConstant();
	_rightFrontWheel.spring().setWheelRadius( dWheelRadius );

	//////////////////////////////////////////////////////

	_leftRearWheel.spring().setChannelMass( 370 );
	_leftRearWheel.setContactPoint( Vector3D(-0.80184, 0, 1.36414 ));
	_leftRearWheel.spring().setMinimumDistance( dMin );
	_leftRearWheel.spring().setEquilibriumDistance( dEquib );
	_leftRearWheel.spring().setLength( dLength );
	_leftRearWheel.spring().setRestitution( dRest );

	_leftRearWheel.spring().setSpringFriction( dSpringFriction );
	_leftRearWheel.spring().setSpringMass( 231 );
	_leftRearWheel.spring().setContactFrictionCoefficient( dContactFriction );
	_leftRearWheel.spring().calculateSpringConstant();
	_leftRearWheel.spring().setWheelRadius( dWheelRadius );


	//////////////////////////////////////////////////////

	_rightRearWheel.spring().setChannelMass( 389 );
	_rightRearWheel.setContactPoint( Vector3D(0.80184, 0, 1.36414 ));
	_rightRearWheel.spring().setMinimumDistance( dMin );
	_rightRearWheel.spring().setEquilibriumDistance( dEquib );
	_rightRearWheel.spring().setLength( dLength );
	_rightRearWheel.spring().setRestitution( dRest );

	_rightRearWheel.spring().setSpringFriction( dSpringFriction );
	_rightRearWheel.spring().setSpringMass( 243 );
	_rightRearWheel.spring().setContactFrictionCoefficient( dContactFriction );
	_rightRearWheel.spring().calculateSpringConstant();
	_rightRearWheel.spring().setWheelRadius( dWheelRadius );


	///////////////////////////////////////////////////////////////////

	float fMass = 100;
	float fSpringMass = 50.0;
	float fRst = 0.05f;
	float fSpringFriction = 18000.0;

	_frontLeft.spring().setChannelMass(fMass);
	_frontLeft.setContactPoint( Vector3D(-0.63, 0.53, -2.41 ));
	_frontLeft.spring().setMinimumDistance( dMin );
	_frontLeft.spring().setEquilibriumDistance( dRest );
	_frontLeft.spring().setLength(dLength);
	_frontLeft.spring().setRestitution( fRst );
	_frontLeft.spring().setSpringFriction( fSpringFriction );
	_frontLeft.spring().setSpringMass( fSpringMass );
	_frontLeft.spring().setContactFrictionCoefficient( 30.0f );
	_frontLeft.spring().calculateSpringConstant();

	_frontRight.spring().setChannelMass(fMass);
	_frontRight.setContactPoint( Vector3D(0.63, 0.53, -2.41 ));
	_frontRight.spring().setMinimumDistance( dMin );
	_frontRight.spring().setEquilibriumDistance( dRest );
	_frontRight.spring().setLength(0.40f);
	_frontRight.spring().setRestitution( fRst );
	_frontRight.spring().setSpringFriction( fSpringFriction );
	_frontRight.spring().setSpringMass( fSpringMass );
	_frontRight.spring().setContactFrictionCoefficient( 30.0f );
	_frontRight.spring().calculateSpringConstant();

	_frontLeftCeiling.spring().setChannelMass(fMass);
	_frontLeftCeiling.setContactPoint( Vector3D(-0.68, 1.36, -0.4172 ));
	_frontLeftCeiling.spring().setMinimumDistance( dMin );
	_frontLeftCeiling.spring().setEquilibriumDistance( dRest );
	_frontLeftCeiling.spring().setLength(dLength);
	_frontLeftCeiling.spring().setRestitution( fRst );
	_frontLeftCeiling.spring().setSpringFriction( fSpringFriction );
	_frontLeftCeiling.spring().setSpringMass( fSpringMass );
	_frontLeftCeiling.spring().setContactFrictionCoefficient( 30.0f );
	_frontLeftCeiling.spring().calculateSpringConstant();
	
	_frontRightCeiling.spring().setChannelMass(fMass);
	_frontRightCeiling.setContactPoint( Vector3D(0.68, 1.36, -0.4172 ));
	_frontRightCeiling.spring().setMinimumDistance( dMin );
	_frontRightCeiling.spring().setEquilibriumDistance( dRest );
	_frontRightCeiling.spring().setLength(dLength);
	_frontRightCeiling.spring().setRestitution( 0.1f );
	_frontRightCeiling.spring().setSpringFriction( fSpringFriction );
	_frontRightCeiling.spring().setSpringMass( fSpringMass );
	_frontRightCeiling.spring().setContactFrictionCoefficient( 30.0f );
	_frontRightCeiling.spring().calculateSpringConstant();

	_rearLeftCeiling.spring().setChannelMass(fMass);
	_rearLeftCeiling.setContactPoint( Vector3D(-0.62, 1.4, 0.86 ));
	_rearLeftCeiling.spring().setMinimumDistance( dMin );
	_rearLeftCeiling.spring().setEquilibriumDistance( dRest );
	_rearLeftCeiling.spring().setLength(dLength);
	_rearLeftCeiling.spring().setRestitution( fRst );
	_rearLeftCeiling.spring().setSpringFriction( fSpringFriction );
	_rearLeftCeiling.spring().setSpringMass( fSpringMass );
	_rearLeftCeiling.spring().setContactFrictionCoefficient( 30.0f );
	_rearLeftCeiling.spring().calculateSpringConstant();
		
	_rearRightCeiling.spring().setChannelMass(fMass);
	_rearRightCeiling.setContactPoint( Vector3D(0.62, 1.4, 0.86 ));
	_rearRightCeiling.spring().setMinimumDistance( dMin );
	_rearRightCeiling.spring().setEquilibriumDistance( dRest );
	_rearRightCeiling.spring().setLength(dLength);
	_rearRightCeiling.spring().setRestitution( fRst );
	_rearRightCeiling.spring().setSpringFriction( fSpringFriction );
	_rearRightCeiling.spring().setSpringMass( fSpringMass );
	_rearRightCeiling.spring().setContactFrictionCoefficient( 30.0f );
	_rearRightCeiling.spring().calculateSpringConstant();

	_rearLeft.spring().setChannelMass(fMass);
	_rearLeft.setContactPoint( Vector3D(-0.74, 0.56, 2.44 ));
	_rearLeft.spring().setMinimumDistance( dMin );
	_rearLeft.spring().setEquilibriumDistance( dRest );
	_rearLeft.spring().setLength(dLength);
	_rearLeft.spring().setRestitution( fRst );
	_rearLeft.spring().setSpringFriction( fSpringFriction );
	_rearLeft.spring().setSpringMass( fSpringMass );
	_rearLeft.spring().setContactFrictionCoefficient( 30.0f );
	_rearLeft.spring().calculateSpringConstant();

	_rearRight.spring().setChannelMass(fMass);
	_rearRight.setContactPoint( Vector3D(0.74, 0.56, 2.44 ));
	_rearRight.spring().setMinimumDistance( dMin );
	_rearRight.spring().setEquilibriumDistance( dRest );
	_rearRight.spring().setLength(dLength);
	_rearRight.spring().setRestitution( fRst );
	_rearRight.spring().setSpringFriction( fSpringFriction );
	_rearRight.spring().setSpringMass( fSpringMass );
	_rearRight.spring().setContactFrictionCoefficient( 30.0f );
	_rearRight.spring().calculateSpringConstant();
}

//////////////////////////////////////////////////////////////////////////////////////////////

class AudiA8 : 
	public AudiA8RigidBody<AudiA8>
///	public AudiA8OpenGLModel<AudiA8>
//	public PersistRigidBody<AudiA8>
{
public:
	AudiA8()
		:AudiA8RigidBody(nullptr)
	{ 

	}

};
