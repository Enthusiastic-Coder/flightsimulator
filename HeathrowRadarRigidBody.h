#pragma once

#include "JSONRigidBodyCollection.h"

template<class T>
class HeathrowRadarRigidBody : 
	public JSONRigidBody
{
public:
	HeathrowRadarRigidBody(float fMass, const Matrix3x3D& inertia, JSONRigidBodyCollection *pParent);

	void update(double dt)
	{
		_rotation -= 360 * dt/ 4.0f;
		// Empty Implementation as this is static body.
	}

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
		setPosition( 150, 0, -100 );
	}

	void Attach( JSONRigidBodyCollection *pParent )
	{
		JSONRigidBody *pJSONBody = pParent->createStaticJSONBody( this );
	}

	virtual void setRecorderHook(FlightRecorder& a) override
	{
		a.addDataRef( _rotation );
	}

	float _rotation;
	Vector3D _rotationPt;
};

template<class T> 
HeathrowRadarRigidBody<T>::HeathrowRadarRigidBody(float fMass, const Matrix3x3D& inertia, JSONRigidBodyCollection *pParent) : 
	_rotation(0.0f), 
	_rotationPt(Vector3D(0,37.45f,0) ), 
	JSONRigidBody("HeathrowRadar", pParent)
{
	setMass( fMass );
	setInertiaMatrix( inertia );
	setCG(Vector3D() );
	
	initSpeedAndPos();
}


//////////////////////////////////////////////////////////////////////////////////////////////

#include "HeathrowRadarOpengl.h"

class HeathrowRadar : 
	public HeathrowRadarRigidBody<HeathrowRadar>, 
	public HeathrowRadarOpenGLModel<HeathrowRadar>
{
public:
	HeathrowRadar()
		: HeathrowRadarRigidBody(100, Matrix3x3D(1,0,0,1,0,0,1,0,0),nullptr) { }

	void persistRead(FILE* fPersistFile )
	{
	}

	void persistWrite(FILE* fPersistFile )
	{
	}

};
