#pragma once

#include "GSForceGenerator.h"


class GravityForceGenerator : public GSForceGenerator
{
public:
	FORCEGENERATOR_TYPE(Type_Gravity)

		GravityForceGenerator() : _dGravity(9.80665) {}

	void onBeforeUpdate(Particle *pParticle, double dt) override;
	void onApplyForce(Particle* pParticle, double dt) override;

private:
	double _dGravity;
};