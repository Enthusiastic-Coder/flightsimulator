#pragma once

#include "JSONRigidBodyCollection.h"

#include <QGuiApplication>

template<class T>
class TowBarRigidBody;

template<class T>
class MyTowBarOwnCustomForceGenerator : public GSForceGenerator
{
public:
	FORCEGENERATOR_TYPE(Type_Custom)

	MyTowBarOwnCustomForceGenerator()
		:_hasFocus(false)
	{
	}

	bool _hasFocus;

	void onApplyForce(Particle *p, double dt) override
	{		
		if( !_hasFocus)
			return;

		TowBarRigidBody<T> *pBody = (TowBarRigidBody<T> *)p;

		float max_decel = 6.0f;
		float max_accel = 6.0f;
		float goodForce = 10000.0f;
		pBody->setBodyFrame();

		if( GetAsyncKeyState( 'L' ) < 0 )
			pBody->applyForce( Vector3D(0,0,0), pBody->toLocalGroundFrame(Vector3D(0, goodForce*3 ,0)));

		pBody->setBodyFrame();

		if( GetAsyncKeyState( 223 ) < 0 )
			pBody->applyForce( Vector3D(0, 0.5f, 2.65f),  pBody->toLocalGroundFrame(Vector3D(0, goodForce ,0)));

		if( GetAsyncKeyState( '1' ) < 0 )
			pBody->applyForce( Vector3D(0.0f,0.54f, -3.26f),  pBody->toLocalGroundFrame(Vector3D(0, goodForce ,0)));

		if( GetAsyncKeyState( 'J' ) < 0 )
			pBody->applyForce( Vector3D(0,1,2),  Vector3D(goodForce/2, 0 ,0));

		if( GetAsyncKeyState( 'K' ) < 0 )
			pBody->applyForce( Vector3D(0,1,2),  Vector3D(-goodForce/2, 0 ,0));

		if( GetAsyncKeyState( 'I') < 0 )
		{
			pBody->_leftWheel.spring().getWheelSpeed() -= max_accel * dt;
			pBody->_rightWheel.spring().getWheelSpeed() -= max_accel * dt;
		}

		if( GetAsyncKeyState( 'M' ) < 0 )
		{
			pBody->applyForce( Vector3D(-1.74f,0,0.32f), Vector3D(0, 0, goodForce));
			pBody->applyForce( Vector3D(1.74f,0,0.32f), Vector3D(0, 0, goodForce));
		}

		if( GetAsyncKeyState( 'B' ) < 0 )
		{
			if( pBody->_leftWheel.spring().getWheelSpeed()  > 0 )
				pBody->_leftWheel.spring().getWheelSpeed() -= min( max_decel * dt, pBody->_leftWheel.spring().getWheelSpeed() );
			else
				pBody->_leftWheel.spring().getWheelSpeed() += min( max_decel * dt, (-pBody->_leftWheel.spring().getWheelSpeed()) );

			if( pBody->_rightWheel.spring().getWheelSpeed()  > 0 )
				pBody->_rightWheel.spring().getWheelSpeed() -= min( max_decel * dt, pBody->_rightWheel.spring().getWheelSpeed() );
			else
				pBody->_rightWheel.spring().getWheelSpeed() += min( max_decel * dt, (-pBody->_rightWheel.spring().getWheelSpeed()) );
		}
	}
};


template<class T>
class TowBarRigidBody : 
	public JSONRigidBody, 
	public ConstrainedSpring<0,TowBarRigidBody<T>>,
	public ConstrainedSpring<1,TowBarRigidBody<T>>,
	public ConstrainedSpring<2,TowBarRigidBody<T>>
{
public:
	TowBarRigidBody(float fMass, const Matrix3x3D& inertia, JSONRigidBodyCollection *pParent);

	MyTowBarOwnCustomForceGenerator<T> _custom_fg;
	GravityForceGenerator _gravityFG;
	WheelSpringForceGenerator _leftWheel, _rightWheel;
	
	SpringForceGenerator _front;
	SpringForceGenerator _back;
	SpringForceGenerator _top;


	void Render(float fLightFraction)
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

	void initSpeedAndPos()
	{
		setVelocity(0,0,0);
		setPosition( GPSLocation() );
		setAngularVelocity(0,0,0);
		setOrientation(0,0,0);
	}

	void Attach( JSONRigidBodyCollection *pParent )
	{
		/*JSONRigidBody *pJSONBody = pParent->createJSONBody( this, VectorD(0, -0.06f, 0) );

		pJSONBody->add( &_gravityFG, 15 );

		float fps = 150;

		pJSONBody->add( &_custom_fg );
		pJSONBody->add( &_leftWheel, fps );
		pJSONBody->add( &_rightWheel, fps );

		pJSONBody->add( &_front, fps );
		pJSONBody->add( &_back, fps );
		pJSONBody->add( &_top , fps );

		ConstrainedSpring<0,TowBarRigidBody<T>>::Attach(pJSONBody,fps);
		ConstrainedSpring<1,TowBarRigidBody<T>>::Attach(pJSONBody,fps);
		ConstrainedSpring<2,TowBarRigidBody<T>>::Attach(pJSONBody,fps);*/
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
		scfg.spring().setEquilibriumDistance( 0.06f );
		scfg.spring().setLength(0.2f);
		scfg.spring().setRestitution( 0.01f );
	}

	void SpringContactCommmonDistanceSetup( SpringForceGenerator& scfg )
	{
		scfg.spring().setMinimumDistance( 0.05f );
		scfg.spring().setEquilibriumDistance( 0.1f );
		scfg.spring().setLength(0.2f);
		scfg.spring().setRestitution( 0.1f );
	}

	void SpringContactCommonSetup( SpringForceGenerator& scfg )
	{
		scfg.spring().setSpringFriction( 30000.0f );
		scfg.spring().setSpringMass( 300.0f );
		scfg.spring().setContactFrictionCoefficient( 30.0f );
		SpringContactCommmonDistanceSetup(scfg);
		scfg.spring().calculateSpringConstant();
	}
};

template<class T> 
TowBarRigidBody<T>::TowBarRigidBody(float fMass, const Matrix3x3D& inertia, JSONRigidBodyCollection *pParent)
	:JSONRigidBody("TowBar", pParent)
//:_gravityFG(VectorD(0, -1.0f, 0 ) )
{
	setMass( fMass );
	setInertiaMatrix( inertia );
	setCG(Vector3D(0, 0.53f, -0.18f ) );
	
	//initSpeedAndPos();

	_leftWheel.spring().setChannelMass( getMass()/2.5f );
	_leftWheel.setContactPoint( Vector3D(-0.6f, 0, 0.65f ));
	SpringContactCommonSetup(_leftWheel);
	_leftWheel.spring().setWheelRadius( 0.27f );
	_leftWheel.spring().setDrivingState( SpringModel::DrivingState::NEUTRAL );


	_rightWheel.spring().setChannelMass( getMass()/2.5f );
	_rightWheel.setContactPoint( Vector3D(0.6f, 0, 0.65f ));
	SpringContactCommonSetup( _rightWheel );
	_rightWheel.spring().setWheelRadius( 0.27f );
	_rightWheel.spring().setDrivingState( SpringModel::DrivingState::NEUTRAL );
//------------------------------------------------------------------

	_front.spring().setChannelMass(getMass()/3);
	_front.setContactPoint( Vector3D(0, 0.54, -3.26 ));
	_front.spring().setSpringFriction( 30000.0f );
	_front.spring().setSpringMass( 30.0f );
	_front.spring().setContactFrictionCoefficient( 30.0f );
	SpringContactCommmonDistanceTopSetup(_front);
	_front.spring().calculateSpringConstant();

	_back.spring().setChannelMass(getMass()/3);
	_back.setContactPoint( Vector3D(0, 0.5f, 2.65 ));
	SpringContactCommmonDistanceTopSetup(_back);
	_back.spring().setSpringFriction( 30000.0f );
	_back.spring().setSpringMass( 30.0f );
	_back.spring().setContactFrictionCoefficient( 30.0f );
	_back.spring().calculateSpringConstant();

//-------------------------------------------------------------------
	_top.spring().setChannelMass(getMass()/2);
	_top.setContactPoint( Vector3D(0.0, 1.25f, -1.96f ));
	SpringContactCommmonDistanceTopSetup(_top);
	_top.spring().setSpringFriction( 35000.0f );
	_top.spring().setSpringMass( 30.0f );
	_top.spring().setContactFrictionCoefficient( 30.0f );
	_top.spring().calculateSpringConstant();
}


//////////////////////////////////////////////////////////////////////////////////////////////

#include "TowBarOpengl.h"

class TowBar : 
	public TowBarRigidBody<TowBar>, 
	public TowBarOpenGLModel<TowBar>
{
public:
	TowBar(float fMass, const Matrix3x3D& inertia)
		: TowBarRigidBody(fMass, inertia,nullptr) { }

};
