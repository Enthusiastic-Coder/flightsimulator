#include "stdafx.h"
#include "AircraftCarrierRigidBody.h"
#include "JSONRigidBodyBuilder.h"
#include "OpenGLRenderer.h"
#include "WorldSystem.h"

AircraftCarrierCustomForceGenerator::AircraftCarrierCustomForceGenerator() {}

void AircraftCarrierCustomForceGenerator::onApplyForce(Particle *p, double dt)
{	
	float goodForce = 50000.0f;
	AircraftCarrierJSONRigidBody* pBody = reinterpret_cast<AircraftCarrierJSONRigidBody*>(p);

    if (getWorld()->focusedRigidBody() != p)
        return;

	pBody->setBodyFrame();

    float dFactor = 1.0f;

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
		pBody->_angularVelocity = Vector3F();
		pBody->_heightRate = 0.0;

		if (GetAsyncKeyState(VK_SHIFT) < 0)
			pBody->_orientation.Reset();

		pBody->UpdatePlane();
	}
}


AircraftCarrierJSONRigidBody::AircraftCarrierJSONRigidBody(std::string name) :
	JSONRigidBody(name, JSONRigidBody::Type::PLANE)
{
	setMass( 8000 );

	addForceGenerator("fg", &_custom_fg);
	initSpeedAndPos();
}

void AircraftCarrierJSONRigidBody::update(double dt)
{
	JSONRigidBody::update(dt);
	UpdatePlane();

	IJoystick *joy = _custom_fg.getJoystick();
}

bool AircraftCarrierJSONRigidBody::onMouseWheel(int wheelDelta)
{
	return false;
}

float AircraftCarrierJSONRigidBody::getPower(int engine)
{
	return __super::getPower(engine);
}

float AircraftCarrierJSONRigidBody::getPowerOutput(int engine)
{
	return __super::getPowerOutput(engine);
}

bool AircraftCarrierJSONRigidBody::onSyncKeyPress()
{
    if( GetKeyState(VK_F5) < 0 ) //Reset
    {
		reset();
		initSpeedAndPos();
		return true;
	}

	return false;
}

void AircraftCarrierJSONRigidBody::initSpeedAndPos()
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
	setEuler( 5,90,0.0);
	
#if defined LOCATED_AT_GIBRALTER
	setPosition( GPSLocation(36.151473, -5.339665,-20)+Vector3F(1000,0,0));
	setEuler(0, 220, 0);
#else
	setPosition( GPSLocation(51.464951, -0.434115,22));
#endif
	_heightRate = 0.0;
	UpdatePlane();
} 

bool AircraftCarrierJSONRigidBody::getHeightFromPosition(const GPSLocation & position, HeightData &heightData) const 
{
	Vector3F pos = getGPSLocation().offSetTo(position);

	for (int i = 0; i < 3; ++i)
	{
		if (!_Planes[i].PointIsContained(pos))
			continue;

		heightData.setData(pos, _Planes[i].getPlane(), (_angularVelocity / 180.0f*float(M_PI) ^ pos).toDouble());
		return true;
	}

	return false;
}

void AircraftCarrierJSONRigidBody::UpdatePlane()
{
	QuarternionF q = getGroundOrientation().toFloat();// MathSupport<float>::MakeQ(getOrientation().toFloat());

    float dModelHeight = 31.88f;
	
	Vector3F CG = cg().toFloat();

	_Planes[0][0] = QVRotate(q, Vector3F(-12, dModelHeight, 174)-CG)  + CG;
	_Planes[0][1] = QVRotate(q, Vector3F(12, dModelHeight, 174) - CG) + CG;
	_Planes[0][2] = QVRotate(q, Vector3F(18, dModelHeight, 80) - CG) + CG;
	_Planes[0][3] = QVRotate(q, Vector3F(-21, dModelHeight, 80) - CG) + CG;
	_Planes[0].calcPlane();

	_Planes[1][0] = QVRotate(q, Vector3F(-32, dModelHeight, 80) - CG) + CG;
	_Planes[1][1] = QVRotate(q, Vector3F(36, dModelHeight, 80) - CG) + CG;
	_Planes[1][2] = QVRotate(q, Vector3F(33, dModelHeight, -108) - CG) + CG;
	_Planes[1][3] = QVRotate(q, Vector3F(-30, dModelHeight, -108) - CG)  + CG;
	_Planes[1].calcPlane();

	_Planes[2][0] = QVRotate(q, Vector3F(-30, dModelHeight, -108) - CG) + CG;
	_Planes[2][1] = QVRotate(q, Vector3F(33, dModelHeight, -108) - CG) + CG;
	_Planes[2][2] = QVRotate(q, Vector3F(13, dModelHeight, -150) - CG) + CG;
	_Planes[2][3] = QVRotate(q, Vector3F(-24, dModelHeight, -143) - CG) + CG;
	_Planes[2].calcPlane();
}

void AircraftCarrierJSONRigidBody::renderMesh(Renderer* r, unsigned int shadowMapCount)
{
    __super::renderMesh(r, shadowMapCount);
//	return;
//	// Test Plane Cover Area
//	glUseProgram(0);
//	OpenGLPipeline& mat = OpenGLPipeline::Get(0);
//	mat.Push();
//	mat.GetModel().LoadIdentity();
//	mat.GetModel().TransRotateGPS(getGPSLocation());
//	mat.GetModel().Translate(0, 0.1, 0);
//	mat.Apply(0);
		
//	glDisable(GL_TEXTURE_2D);
//	glEnable(GL_BLEND);
//	//glDisable(GL_DEPTH_TEST);
//	glBegin(GL_QUADS);
//		glColor4f(1, 0, 0, 0.5);
//		glVertex3fv(&_Planes[0][0].x);
//		glVertex3fv(&_Planes[0][1].x);
//		glVertex3fv(&_Planes[0][2].x);
//		glVertex3fv(&_Planes[0][3].x);

//		glColor4f(1, 0, 1, 0.5);
//		glVertex3fv(&_Planes[1][0].x);
//		glVertex3fv(&_Planes[1][1].x);
//		glVertex3fv(&_Planes[1][2].x);
//		glVertex3fv(&_Planes[1][3].x);

//		glColor4f(1, 1, 0, 0.5);
//		glVertex3fv(&_Planes[2][0].x);
//		glVertex3fv(&_Planes[2][1].x);
//		glVertex3fv(&_Planes[2][2].x);
//		glVertex3fv(&_Planes[2][3].x);

//	glEnd();

//	glEnable(GL_TEXTURE_2D);
//	glDisable(GL_BLEND);
//	//glEnable(GL_DEPTH_TEST);
//	mat.Pop();
//	glUseProgram(progID);

    //r->useProgram();
}

/////////////////////////////////////////////////////////////////////////////////////////////////

