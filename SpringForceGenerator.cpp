#include "stdafx.h"
#include "SpringForceGenerator.h"
#include "WorldSystem.h"


SpringModel& SpringForceGenerator::spring()
{
	return _spring;
}

void SpringForceGenerator::reset()
{
	GSForceGenerator::reset();
	_spring.reset();
}

double SpringForceGenerator::Height()
{
	return spring().getHeightData().Height();
}


void SpringForceGenerator::setRecorderHook(FlightRecorder& a)
{
	GSForceGenerator::setRecorderHook(a);
	_spring.setRecorderHook(a);
}

/*if( springTopPt._height < -5 ) //NEED A WAY TO REPORT BACK AIRCRAFT CRASH
{
_pRigidBody->setAngularVelocity(0,0,0);
_pRigidBody->setVelocity(0,0,0);
_pRigidBody->setOrientation(0,0,0);
GPSLocation gpsLoc = _pRigidBody->getGPSLocation();
_pRigidBody->setPosition( GPSLocation(gpsLoc._lat, gpsLoc._lng, gpsLoc._height + 5 ) );
return;
}*/

void SpringForceGenerator::onApplyForce(Particle *p, double dt)
{
	GSRigidBody* pBody = static_cast<GSRigidBody*>(p);
	if(getWorld())
		getWorld()->getHeightFromPosition(pBody->toNonLocalTranslateFrame(spring().separatorPosition(getContactPoint())), spring().getHeightData());

	if (spring().getHeightData().Height() > 10)// turn into configurable setting.
		return;

	spring().setVelocityBody(pBody->getTotalBodyVelocity(getContactPoint()) + pBody->toLocalGroundFrame(spring().getHeightData().Velocity()));
	spring().setBodyOrientation(pBody->getGroundOrientation());
	spring().setGravity(pBody->gravity());
	spring().update(dt);

	pBody->setBodyFrame();
	applyForce(pBody, spring().getBodyForce());
}