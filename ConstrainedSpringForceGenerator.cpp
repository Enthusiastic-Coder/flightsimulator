#include "stdafx.h"
#include "ConstrainedSpringForceGenerator.h"
#include "GSRigidBody.h"


ConstrainedSpringForceGenerator::ConstrainedSpringForceGenerator() :
	_pPartnerBody(nullptr), 
	_pSpring(nullptr),
	_bMaster(false),
	_distance(100.0f)
{
}

bool ConstrainedSpringForceGenerator::isMaster()
{
	return _bMaster;
}

void ConstrainedSpringForceGenerator::onInitialise(WorldSystem* pWorldSystem)
{
	if( _pSpring != nullptr )
		_pSpring->calculate();
}

void ConstrainedSpringForceGenerator::onBeforeUpdate( Particle *p, double dt)
{
	GSForceGenerator::onBeforeUpdate(p, dt);
	
	if( _pSpring == nullptr || _pPartnerBody == nullptr )
		return;

	_partnerTotalVelocity = _pPartnerBody->toNonLocalFrame(_pPartnerBody->getTotalBodyVelocity(_pPartnerConnectionPt));
	_partnerPosition = _pPartnerBody->toNonLocalTranslateFrame(_pPartnerConnectionPt);
}

void ConstrainedSpringForceGenerator::onApplyForce( Particle *p, double dt )
{
	if( _pSpring == nullptr || _pPartnerBody == nullptr )
		return;

	GSRigidBody *pBody = static_cast<GSRigidBody*>(p);

	Vector3D thisPosition = pBody->toNonLocalTranslateFrame(getContactPoint());
	Vector3D dPosition = pBody->toLocalFrame(_partnerPosition - thisPosition);

	_distance = dPosition.Magnitude();

	if( _distance > _pSpring->maxLen())
		return;

	Vector3D totalVel = pBody->toNonLocalFrame(pBody->getTotalBodyVelocity(getContactPoint()));
	Vector3D dVelocityWorld = pBody->toLocalFrame(_partnerTotalVelocity - totalVel);

	pBody->setBodyFrame();

	applyForce(p, _pSpring->K() * _distance * dPosition.Unit() );

	double fVelocityWorldMagnitude = dVelocityWorld.Magnitude();

	double fFriction = _pSpring->MU() * fVelocityWorldMagnitude;
	double fMaxFriction = p->getMass() * fVelocityWorldMagnitude / dt;
		
    applyForce(p, std::min(fFriction,fMaxFriction) * dVelocityWorld.Unit() );
}


//////////////////////////////////////////////////////

