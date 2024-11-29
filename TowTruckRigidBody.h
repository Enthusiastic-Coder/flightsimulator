#pragma once

#include <QGuiApplication>
#include <QCursor>
#include <QSize>
#include <QWindow>
#include <QPoint>
#include "GSForceGenerator.h"
#include "SpringForceGenerator.h"

template<class T>
class TowTruckRigidBody;

template<class T>
class MyTowTruckOwnCustomForceGenerator : public GSForceGenerator
{
public:
	FORCEGENERATOR_TYPE( Type_Custom )

	MyTowTruckOwnCustomForceGenerator()
		:_hasFocus(false),_bDrivingByMouse(false)
	{
	}

	bool _hasFocus;

	void onApplyForce(Particle *p, double dt) override
	{		
		if( !_hasFocus)
			return;

		TowTruckRigidBody<T> *pTowTruck = static_cast<TowTruckRigidBody<T>*>(p);

		float max_decel = 12.0f;
		float max_accel = 12.0f;
		float goodForce = 100000.0f/3.0f;
		pTowTruck->setBodyFrame();

		if( GetAsyncKeyState( 'L' ) < 0 )
			pTowTruck->applyForce( VectorD(0,0,0),pTowTruck->toLocalGroundFrame(VectorD(0, goodForce*3 ,0)));

		if( GetAsyncKeyState( 'J' ) < 0 )
			pTowTruck->applyForce( VectorD(0,1,2), VectorD(goodForce, 0 ,0));

		if( GetAsyncKeyState( 'K' ) < 0 )
			pTowTruck->applyForce( VectorD(0,1,2), VectorD(-goodForce, 0 ,0));

		if( _hasFocus && _bDrivingByMouse )
		{
            QPoint cursorPt = QCursor::pos();
            QSize sz = qApp->primaryScreen()->size();
            //POINT cursorPt;
            //GetCursorPos(&cursorPt);

            float dMouseY = (cursorPt.y() - sz.height()/2)/10.0f;
			max_accel = -(dMouseY - pTowTruck->_leftFrontWheel.spring().getWheelSpeed());

			if( fabs( dMouseY ) < 5 )
			{
				pTowTruck->_leftFrontWheel.spring().setDrivingState( SpringModel::DrivingState::NEUTRAL );
				pTowTruck->_rightFrontWheel.spring().setDrivingState( SpringModel::DrivingState::NEUTRAL );
				pTowTruck->_leftRearWheel.spring().setDrivingState( SpringModel::DrivingState::NEUTRAL );
				pTowTruck->_rightRearWheel.spring().setDrivingState( SpringModel::DrivingState::NEUTRAL );
			}
			else
			{
				pTowTruck->_leftFrontWheel.spring().getDrivingState() = SpringModel::DrivingState::POWERED;
				pTowTruck->_rightFrontWheel.spring().getDrivingState() = SpringModel::DrivingState::POWERED;
				pTowTruck->_leftRearWheel.spring().getDrivingState() = SpringModel::DrivingState::POWERED;
				pTowTruck->_rightRearWheel.spring().getDrivingState() = SpringModel::DrivingState::POWERED;
			}

			if( max_accel > 0 )
				max_accel = min(16,max_accel);
			else
				max_accel = max(-16, max_accel);
			
			 float towAngle = (cursorPt.x- GetSystemMetrics(SM_CXSCREEN)/2)/16;
			if( towAngle < -25 ) towAngle = -25;
			if( towAngle > 25 ) towAngle = 25;
			pTowTruck->_leftFrontWheel.spring().getSteeringAngle() = towAngle;
			pTowTruck->_rightFrontWheel.spring().getSteeringAngle() = towAngle;
		}

		if( GetAsyncKeyState( 'I') < 0 || (_hasFocus && _bDrivingByMouse))
		{
			pTowTruck->_leftFrontWheel.spring().getWheelSpeed() -= max_accel * dt;
			pTowTruck->_rightFrontWheel.spring().getWheelSpeed() -= max_accel * dt;

			pTowTruck->_leftRearWheel.spring().getWheelSpeed() -= max_accel * dt;
			pTowTruck->_rightRearWheel.spring().getWheelSpeed() -= max_accel * dt;
		}

		if( GetAsyncKeyState( 'M' ) < 0 )
		{
			pTowTruck->_leftFrontWheel.spring().getWheelSpeed() += max_accel * dt;
			pTowTruck->_rightFrontWheel.spring().getWheelSpeed() += max_accel * dt;

			pTowTruck->_leftRearWheel.spring().getWheelSpeed() += max_accel * dt;
			pTowTruck->_rightRearWheel.spring().getWheelSpeed() += max_accel * dt;
		}


		if( GetAsyncKeyState( 'B' ) < 0 && GetAsyncKeyState( VK_SHIFT ) < 0 )
		{
			pTowTruck->_leftFrontWheel.spring().setDrivingState( SpringModel::DrivingState::BRAKE );
			pTowTruck->_rightFrontWheel.spring().setDrivingState( SpringModel::DrivingState::BRAKE );
			pTowTruck->_leftRearWheel.spring().setDrivingState( SpringModel::DrivingState::BRAKE );
			pTowTruck->_rightRearWheel.spring().setDrivingState( SpringModel::DrivingState::BRAKE );

		}

		if( GetAsyncKeyState( 'B' ) < 0 && GetAsyncKeyState( VK_SHIFT ) >= 0 )
		{
			if( pTowTruck->_leftFrontWheel.spring().getWheelSpeed()  > 0 )
				pTowTruck->_leftFrontWheel.spring().getWheelSpeed() -= min( max_decel * dt, pTowTruck->_leftFrontWheel.spring().getWheelSpeed() );
			else
				pTowTruck->_leftFrontWheel.spring().getWheelSpeed() += min( max_decel * dt, (-pTowTruck->_leftFrontWheel.spring().getWheelSpeed()) );

			if( pTowTruck->_rightFrontWheel.spring().getWheelSpeed()  > 0 )
				pTowTruck->_rightFrontWheel.spring().getWheelSpeed() -= min( max_decel * dt, pTowTruck->_rightFrontWheel.spring().getWheelSpeed() );
			else
				pTowTruck->_rightFrontWheel.spring().getWheelSpeed() += min( max_decel * dt, (-pTowTruck->_rightFrontWheel.spring().getWheelSpeed()) );


			if( pTowTruck->_leftRearWheel.spring().getWheelSpeed()  > 0 )
				pTowTruck->_leftRearWheel.spring().getWheelSpeed() -= min( max_decel * dt, pTowTruck->_leftRearWheel.spring().getWheelSpeed() );
			else
				pTowTruck->_leftRearWheel.spring().getWheelSpeed() += min( max_decel * dt, (-pTowTruck->_leftRearWheel.spring().getWheelSpeed()) );

			if( pTowTruck->_rightRearWheel.spring().getWheelSpeed()  > 0 )
				pTowTruck->_rightRearWheel.spring().getWheelSpeed() -= min( max_decel * dt, pTowTruck->_rightRearWheel.spring().getWheelSpeed() );
			else
				pTowTruck->_rightRearWheel.spring().getWheelSpeed() += min( max_decel * dt, (-pTowTruck->_rightRearWheel.spring().getWheelSpeed()) );

		}
	}
};


template<class T>
class TowTruckRigidBody : 
	public JSONRigidBody, 
	public ConstrainedSpring<0,TowTruckRigidBody<T>>,
	public ConstrainedSpring<1,TowTruckRigidBody<T>>
{
public:
	TowTruckRigidBody(float fMass, const Matrix3x3D& inertia, JSONRigidBodyCollection *pParent);

	MyTowTruckOwnCustomForceGenerator<T> _custom_fg;
	GravityForceGenerator _gravityFG;
	WheelSpringForceGenerator _leftFrontWheel, _rightFrontWheel;
	WheelSpringForceGenerator _leftRearWheel, _rightRearWheel;

	SpringForceGenerator _frontMiddle;
	SpringForceGenerator _leftMiddle;
	SpringForceGenerator _rightMiddle;
	SpringForceGenerator _rearMiddle;
	
	SpringForceGenerator _topLeftFront;
	SpringForceGenerator _topRightFront;
	SpringForceGenerator _topLeftRear;
	SpringForceGenerator _topRightRear;

	void draw(float fLightFraction)
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

	void initSpeedAndPos()
	{
		setVelocity(0,0,0);
		setPosition( GPSLocation() );
		setAngularVelocity(0,0,0);
	}

	void Attach( JSONRigidBodyCollection *pParent )
	{
		/*JSONRigidBody *pJSONBody = pParent->createJSONBody( this, VectorD(0, -0.2f, 0) );

		pJSONBody->add( &_gravityFG, 15 );

		float fps = 150;

		pJSONBody->add( &_custom_fg );
		pJSONBody->add( &_leftFrontWheel, fps );
		pJSONBody->add( &_rightFrontWheel, fps );
		pJSONBody->add( &_leftRearWheel, fps );
		pJSONBody->add( &_rightRearWheel, fps );

		pJSONBody->add( &_frontMiddle , fps );
		pJSONBody->add( &_leftMiddle, fps );
		pJSONBody->add( &_rightMiddle, fps );
		pJSONBody->add( &_rearMiddle, fps );

		pJSONBody->add( &_topLeftFront, fps );
		pJSONBody->add( &_topRightFront, fps );
		pJSONBody->add( &_topLeftRear, fps );
		pJSONBody->add( &_topRightRear, fps );

		ConstrainedSpring<0,TowTruckRigidBody<T>>::Attach(pJSONBody,fps);
		ConstrainedSpring<1,TowTruckRigidBody<T>>::Attach(pJSONBody,fps);*/
	}

	void setFocus(bool value)
	{
		_custom_fg._hasFocus = value;
	}

	bool hasFocus()
	{
		return _custom_fg._hasFocus;
	}

private:
	void SpringContactCommmonDistanceTopSetup( SpringForceGenerator& scfg )
	{
		scfg.spring().setMinimumDistance( 0.05f );
		scfg.spring().setEquilibriumDistance( 0.20f );
		scfg.spring().setLength(0.40f);
		scfg.spring().setRestitution( 0.05f );
	}

	void SpringContactCommmonDistanceSetup( SpringForceGenerator& scfg )
	{
		scfg.spring().setMinimumDistance( 0.05f );
		scfg.spring().setEquilibriumDistance( 0.20f );
		scfg.spring().setLength(0.40f);
		scfg.spring().setRestitution( 0.3f );
	}

	void SpringContactCommonSetup( SpringForceGenerator& scfg )
	{
		scfg.spring().setSpringFriction( 30000.0f );
		scfg.spring().setSpringMass( 300.0f );
		scfg.spring().setContactFrictionCoefficient( 20.0f );
		SpringContactCommmonDistanceSetup(scfg);
		scfg.spring().calculateSpringConstant();
	}
};

template<class T> 
TowTruckRigidBody<T>::TowTruckRigidBody(float fMass, const Matrix3x3D& inertia, JSONRigidBodyCollection *pParent)
	:JSONRigidBody( "TowTruck", pParent )
//:_gravityFG(VectorD(0, -1.0f, 0 ) )
{
	setMass( fMass );
	setInertiaMatrix( inertia );
	setCG(VectorD(0,1.03f, -0.28f));

	_leftFrontWheel.spring().setChannelMass( 2056.37 );
	_leftFrontWheel.setContactPoint( VectorD(-1.23f, 0, -1.34f ));
	SpringContactCommonSetup(_leftFrontWheel);
	_leftFrontWheel.spring().setWheelRadius( 0.72f );

	_rightFrontWheel.spring().setChannelMass( 2056.37 );
	_rightFrontWheel.setContactPoint( VectorD(1.23f, 0, -1.34f ));
	SpringContactCommonSetup( _rightFrontWheel );
	_rightFrontWheel.spring().setWheelRadius( 0.72f );
//----------------------------------------------------------------------
	_leftRearWheel.spring().setChannelMass( 1443.63f );
	_leftRearWheel.setContactPoint( VectorD(-1.23f, 0, 1.95f));
	SpringContactCommonSetup( _leftRearWheel );
	_leftRearWheel.spring().setWheelRadius( 0.7f );

	_rightRearWheel.spring().setChannelMass( 1443.63f );
	_rightRearWheel.setContactPoint( VectorD(1.23f, 0, 1.95f ));
	SpringContactCommonSetup(_rightRearWheel);
	_rightRearWheel.spring().setWheelRadius( 0.7f );
//------------------------------------------------------------------

	_frontMiddle.spring().setChannelMass(fMass/4);
	_frontMiddle.setContactPoint( VectorD(0, 0.95, -3.56 ));
	_frontMiddle.spring().setSpringFriction( 25000.0f );
	_frontMiddle.spring().setSpringMass( 300.0f );
	_frontMiddle.spring().setContactFrictionCoefficient( 30.0f );
	SpringContactCommmonDistanceTopSetup(_frontMiddle);
	_frontMiddle.spring().calculateSpringConstant();

	_leftMiddle.spring().setChannelMass(fMass/4);
	_leftMiddle.setContactPoint( VectorD(-1.53, 0.95, -0.16 ));
	SpringContactCommmonDistanceTopSetup(_leftMiddle);
	_leftMiddle.spring().setSpringFriction( 25000.0f );
	_leftMiddle.spring().setSpringMass( 300.0f );
	_leftMiddle.spring().setContactFrictionCoefficient( 30.0f );
	_leftMiddle.spring().calculateSpringConstant();

	_rightMiddle.spring().setChannelMass(fMass/4);
	_rightMiddle.setContactPoint( VectorD(1.53, 0.95, -0.16 ));
	SpringContactCommmonDistanceTopSetup(_rightMiddle);
	_rightMiddle.spring().setSpringFriction( 25000.0f );
	_rightMiddle.spring().setSpringMass( 300.0f );
	_rightMiddle.spring().setContactFrictionCoefficient( 30.0f );
	_rightMiddle.spring().calculateSpringConstant();

	_rearMiddle.spring().setChannelMass(fMass/4);
	_rearMiddle.setContactPoint( VectorD(0, 0.53, 3.74 ));
	SpringContactCommmonDistanceTopSetup(_rearMiddle);
	_rearMiddle.spring().setSpringFriction( 25000.0f );
	_rearMiddle.spring().setSpringMass( 300.0f );
	_rearMiddle.spring().setContactFrictionCoefficient( 30.0f );
	_rearMiddle.spring().calculateSpringConstant();
//-------------------------------------------------------------------
	_topLeftFront.spring().setChannelMass(1809.54);
	_topLeftFront.setContactPoint( VectorD(-1.29, 1.68, -3.5 ));
	SpringContactCommmonDistanceTopSetup(_topLeftFront);
	_topLeftFront.spring().setSpringFriction( 35000.0f );
	_topLeftFront.spring().setSpringMass( 300.0f );
	_topLeftFront.spring().setContactFrictionCoefficient( 30.0f );
	_topLeftFront.spring().calculateSpringConstant();

	_topRightFront.spring().setChannelMass(1809.54);
	_topRightFront.setContactPoint( VectorD(1.29, 1.68, -3.5 ));
	SpringContactCommmonDistanceTopSetup(_topRightFront);
	_topRightFront.spring().setSpringFriction( 35000.0f );
	_topRightFront.spring().setSpringMass( 300.0f );
	_topRightFront.spring().setContactFrictionCoefficient( 30.0f );
	_topRightFront.spring().calculateSpringConstant();

	_topLeftRear.spring().setChannelMass(1690.46);
	_topLeftRear.setContactPoint( VectorD(-1.53, 1.58, 3.13 ));
	SpringContactCommmonDistanceTopSetup(_topLeftRear);
	_topLeftRear.spring().setSpringFriction( 35000.0f );
	_topLeftRear.spring().setSpringMass( 300.0f );
	_topLeftRear.spring().setContactFrictionCoefficient( 30.0f );
	_topLeftRear.spring().calculateSpringConstant();

	_topRightRear.spring().setChannelMass(1690.46);
	_topRightRear.setContactPoint( VectorD(1.53, 1.58, 3.13 ));
	SpringContactCommmonDistanceTopSetup(_topRightRear);
	_topRightRear.spring().setSpringFriction( 35000.0f );
	_topRightRear.spring().setSpringMass( 300.0f );
	_topRightRear.spring().setContactFrictionCoefficient( 30.0f );
	_topRightRear.spring().calculateSpringConstant();
}


//////////////////////////////////////////////////////////////////////////////////////////////

#include "TowTruckOpengl.h"

class TowTruck : 
	public TowTruckRigidBody<TowTruck>, 
	public TowTruckOpenGLModel<TowTruck>
//	public PersistRigidBody<TowTruck>
{
public:
	TowTruck(float fMass, const Matrix3x3D& inertia, JSONRigidBodyCollection* pParent)
		: TowTruckRigidBody(fMass, inertia,pParent) { }
};
