#include "stdafx.h"
#include "GravityForceGenerator.h"
#include "WorldSystem.h"


void GravityForceGenerator::onBeforeUpdate(Particle *pParticle, double dt)
{
	GSForceGenerator::onBeforeUpdate(pParticle, dt);
	//Need to set orientation so that gravity acts towards center of earth.

	if (pParticle->isParticle())
		_dGravity = getWorld()->GetGravityFromPosition(pParticle->position());
	else
		_dGravity = static_cast<GSRigidBody*>(pParticle)->gravity().y;
}

void GravityForceGenerator::onApplyForce(Particle* pParticle, double dt)
{
	if (!pParticle->isParticle())
	{
		GSRigidBody* pBody = static_cast<GSRigidBody*>(pParticle);
		pBody->setWorldFrame();
		setContactPoint(pBody->cg());
	}

	applyForce(pParticle, -pParticle->getMass() * _dGravity * pParticle->position().Unit());
}