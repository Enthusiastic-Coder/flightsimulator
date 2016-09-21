#include "StdAfx.h"
#include "ForceGenerator.h"

ForceGenerator::ForceGenerator() 
: _bIsActive(true) 
{
}

void ForceGenerator::onBeforeUpdate( Particle *p, double dt)
{
	_appliedForce = Vector3D();
}

void ForceGenerator::onAfterUpdate(Particle* p, double dt)
{
}

void ForceGenerator::applyForce(Particle* particle, const Vector3D& force )
{
	double MAX_FORCE = 8e8;
	
	Vector3D & f = (*const_cast<Vector3D*>(&force));
			
	if( fabs(f.x) > MAX_FORCE || fabs(f.y) > MAX_FORCE || fabs(f.z) > MAX_FORCE)
		f = f.Unit() * MAX_FORCE;

	if( particle->isParticle() )
		particle->applyForce( getContactPoint(), f );
	else
	{
		RigidBody *pRigidBody = static_cast<RigidBody*>(particle);
		pRigidBody->applyForce( getContactPointCG( pRigidBody->cg()), f );

		if( !pRigidBody->isLocalFrame() )
			f = pRigidBody->toLocalFrame(force);
	}

	_appliedForce += f;

	//if( fabs(_appliedForce.x) > MAX_FORCE || fabs(_appliedForce.y) > MAX_FORCE || fabs(_appliedForce.z) > MAX_FORCE)
		//_appliedForce = _appliedForce.Unit() * MAX_FORCE;
}

void ForceGenerator::setContactPoint(const Vector3D &contactPoint ) 
{ 
	_contactPoint = contactPoint; 
}

const Vector3D& ForceGenerator::getContactPoint() const
{ 
	return _contactPoint; 
}

Vector3D ForceGenerator::getContactPointCG(const Vector3D& cg) const
{ 
	return _contactPoint-cg; 
}

const Vector3D& ForceGenerator::getAppliedForce()  const
{ 
	return _appliedForce; 
}

bool ForceGenerator::isActive(Particle* p) const
{ 
	return _bIsActive; 
}
	
void ForceGenerator::setIsActive(bool bIsActive)	
{ 
	_bIsActive = bIsActive; 
}

void ForceGenerator::reset()
{
	_appliedForce.Reset();
}
