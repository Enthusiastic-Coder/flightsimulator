#pragma once

#include "JSONRigidBodyCollection.h"

#include <QGuiApplication>

//template<class T>
class FlatLifterRigidBody;

//template<class T>
class MyLifterOwnCustomForceGenerator : public GSForceGenerator
{
public:
	FORCEGENERATOR_TYPE(Type_Custom)

	MyLifterOwnCustomForceGenerator ()
		:_hasFocus(false)
	{
	}

	bool _hasFocus;

	void onApplyForce(Particle *p, double dt) override;
};




//template<class T>
class FlatLifterRigidBody : public JSONRigidBody
{
public:
	FlatLifterRigidBody(float fMass, const Matrix3x3D& inertia, JSONRigidBodyCollection *pParent=nullptr);

	MyLifterOwnCustomForceGenerator _custom_fg;

	void update(double dt)
	{
		// Empty Implementation as this is static body.

		if( _angularVelocity.Magnitude() > DBL_EPSILON || fabs(_heightRate) > DBL_EPSILON )
		{
			_orientation += float(dt) * _angularVelocity.toFloat();
			_height += dt * _heightRate;

			if( _height < -DBL_EPSILON )
			{
				_height = 0.0;
				_heightRate = 0.0;
			}

			double dLimits = 89;

			if( _orientation.x < -dLimits )
			{
				_orientation.x = -dLimits;
				_angularVelocity.x = 0;
			}

			if( _orientation.x > dLimits )
			{
				_orientation.x = dLimits;
				_angularVelocity.x = 0;
			}

			if( _orientation.z < -dLimits )
			{
				_orientation.z = -dLimits;
				_angularVelocity.z = 0;
			}

			if( _orientation.z > dLimits )
			{
				_orientation.z = dLimits;
				_angularVelocity.z = 0;
			}
			
			UpdatePlane();
		}
	}

	void initSpeedAndPos()
	{
		setVelocity(0,0,0);
		setAngularVelocity(0,0,0);
		setEuler(0,0,0);
		_dims = 50;
		_height = 0.0;
		_heightRate = 0.0;
		UpdatePlane();
	}

	void setFocus(bool value)
	{
		_custom_fg._hasFocus = value;
	}

	bool hasFocus()
	{
		return _custom_fg._hasFocus;
	}

	virtual void setRecorderHook(FlightRecorder& a) override
	{
		a.addDataRef( _height );
		a.addDataRef( _boundary );
	}

	bool getHeightFromPosition( const GPSLocation & position, HeightData &heightData ) const override
	{
		Vector3F pos = getGPSLocation().offSetTo( position );

		if(  !_boundary.PointIsContained( pos ) )
			return false;

		double dPlaneHeight = _boundary.getPlane().Height(pos);

		if( dPlaneHeight < 0.0 )
			heightData.setData( position._height );
		else
			heightData.setData( pos, _boundary.getPlane(), _angularVelocity/180.0*M_PI ^ pos.toDouble() );

		return true;
	}

	void UpdatePlane()
	{
		QuarternionF q = MathSupport<float>::MakeQOrientation( _orientation );

		Vector3F heightV(0, _height, 0);

		_boundary[0] = QVRotate( q, Vector3F( -_dims, 0, _dims ) ) + heightV;
		_boundary[1] = QVRotate( q, Vector3F( _dims, 0, _dims ) ) + heightV;
		_boundary[2] = QVRotate( q, Vector3F( _dims, 0, -_dims ) ) + heightV;
		_boundary[3] = QVRotate( q, Vector3F( -_dims, 0, -_dims ) ) + heightV;

		_boundary.calcPlane();
	}

	float _dims;
	float _height;
	float _heightRate;

	Vector3F _orientation;
	Vector3D _angularVelocity;
	
	QuadPlaneBoundaryT _boundary;
};

FlatLifterRigidBody::FlatLifterRigidBody(float fMass, const Matrix3x3D& inertia, JSONRigidBodyCollection *pParent)
	: JSONRigidBody( "FlatLifter", pParent )
{
	setMass( fMass );
	setInertiaMatrix( inertia );
	setCG(Vector3D() );
	
	initSpeedAndPos();
}


//////////////////////////////////////////////////////////////////////////////////////////////

template<class T>
class FlatLifterpenGLModel : public IMeshModel
{
public:
	FlatLifterpenGLModel() : _bshadow(false) {}

	void renderMesh(float fLightingFraction, unsigned int camID, unsigned int shadowMapCount) override
	{
		if( _bshadow )
			return;

		T *pThis = static_cast<T*>(this);

		const Vector3F& p0 = pThis->_boundary[0];
		const Vector3F& p1 = pThis->_boundary[1];
		const Vector3F& p2 = pThis->_boundary[2];
		const Vector3F& p3 = pThis->_boundary[3];

		glEnable(GL_BLEND);
		glDepthMask(GL_FALSE);
		
		glBegin(GL_QUADS);
			glColor4f(0.0,0.0,0.3,0.4);

			//FRONT
			glNormal3f(0,0,1);
			glVertex3d( p0.x, 0, p0.z);
			glVertex3d( p1.x, 0, p1.z );
			glVertex3d( p1.x, p1.y, p1.z );
			glVertex3d( p0.x, p0.y, p0.z );

			//RIGHT
			glNormal3f(1,0,0);
			glVertex3d( p1.x, 0, p1.z );
			glVertex3d( p2.x, 0, p2.z );
			glVertex3d( p2.x, p2.y, p2.z );
			glVertex3d( p1.x, p1.y, p1.z );

			//BACK
			glNormal3f(0,0,-1);
			glVertex3d( p2.x, 0, p2.z );
			glVertex3d( p3.x, 0, p3.z );
			glVertex3d( p3.x, p3.y, p3.z );
			glVertex3d( p2.x, p2.y, p2.z );

			//LEFT
			glNormal3f(-1,0,0);
			glVertex3d( p3.x, 0, p3.z );
			glVertex3d( p0.x, 0, p0.z );
			glVertex3d( p0.x, p0.y, p0.z );
			glVertex3d( p3.x, p3.y, p3.z );

			//TOP
			glNormal3f(0,1,0);
			glColor4f(0.0,0.3,0.3,0.8);
			glVertex3d( p0.x, p0.y, p0.z );
			glVertex3d( p1.x, p1.y, p1.z );
			glVertex3d( p2.x, p2.y, p2.z );
			glVertex3d( p3.x, p3.y, p3.z );

		glEnd();
	

		glBegin(GL_LINES);
			glNormal3fv( &pThis->_boundary.getPlane().N.x );
			glColor3f( 1,1,1 );

			glVertex3d( 0, pThis->_height, 0 );
			glVertex3d( p0.x, p0.y, p0.z ); 

			glVertex3d( 0, pThis->_height, 0 );
			glVertex3d( p1.x, p1.y, p1.z ); 

			glVertex3d( 0, pThis->_height, 0 );
			glVertex3d( p2.x, p2.y, p2.z ); 

			glVertex3d( 0, pThis->_height, 0 );
			glVertex3d( p3.x, p3.y, p3.z ); 

			glVertex3d( p0.x, p0.y, p0.z ); 
			glVertex3d( p0.x, 0, p0.z ); 

			glVertex3d( p1.x, p1.y, p1.z ); 
			glVertex3d( p1.x, 0, p1.z ); 

			glVertex3d( p2.x, p2.y, p2.z ); 
			glVertex3d( p2.x, 0, p2.z ); 

			glVertex3d( p3.x, p3.y, p3.z ); 
			glVertex3d( p3.x, 0, p3.z ); 

			glVertex3d( p0.x, p0.y, p0.z ); 
			glVertex3d( p1.x, p1.y, p1.z ); 

			glVertex3d( p1.x, p1.y, p1.z ); 
			glVertex3d( p2.x, p2.y, p2.z ); 

			glVertex3d( p2.x, p2.y, p2.z ); 
			glVertex3d( p3.x, p3.y, p3.z ); 

			glVertex3d( p3.x, p3.y, p3.z ); 
			glVertex3d( p0.x, p0.y, p0.z ); 

		glEnd();


		glDepthMask(GL_TRUE);
		glDisable(GL_BLEND);
	}

	bool _bshadow;
	float _fLightingFraction;
};


void MyLifterOwnCustomForceGenerator::onApplyForce(Particle *p, double dt) 
{
	if (!_hasFocus)
		return;

	FlatLifterRigidBody *pBody = (FlatLifterRigidBody *)p;

	double dFactor = 1.0;

	if (GetAsyncKeyState(VK_SHIFT) < 0)
		dFactor = 2.0;

	if (GetAsyncKeyState('1') < 0)
		pBody->_heightRate += dt * dFactor;

	if (GetAsyncKeyState('2') < 0)
		pBody->_heightRate -= dt * dFactor;

	dFactor *= 2;

	if (GetAsyncKeyState('3'))
		pBody->_angularVelocity.x += dFactor * dt;

	if (GetAsyncKeyState('4'))
		pBody->_angularVelocity.x -= dFactor * dt;

	if (GetAsyncKeyState('5'))
		pBody->_angularVelocity.y += dFactor * dt;

	if (GetAsyncKeyState('6'))
		pBody->_angularVelocity.y -= dFactor * dt;

	if (GetAsyncKeyState('7'))
		pBody->_angularVelocity.z += dFactor * dt;

	if (GetAsyncKeyState('8'))
		pBody->_angularVelocity.z -= dFactor * dt;

	if (GetAsyncKeyState('0'))
	{
		pBody->_angularVelocity = Vector3D();
		pBody->_heightRate = 0.0;

		if (GetAsyncKeyState(VK_SHIFT) < 0)
			pBody->_orientation.Reset();

		pBody->UpdatePlane();
	}
}
//////////////////////////////////////////////////////////////////////////////////////////////

class FlatLifter : 
	public FlatLifterRigidBody, 
	public FlatLifterpenGLModel<FlatLifter>
{
public:
	FlatLifter()
		: FlatLifterRigidBody(100, Matrix3x3D(1,0,0,1,0,0,1,0,0)) { }

};
