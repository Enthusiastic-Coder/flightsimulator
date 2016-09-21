#pragma once

#include "RigidBody.h"

class ForceGenerator
{
public:
	ForceGenerator();

	virtual void reset();
	virtual void onApplyForce( Particle *p, double dt ) = 0;
	virtual void onBeforeUpdate( Particle *p, double dt);
	virtual void onAfterUpdate(Particle* p, double dt);
	virtual void applyForce(Particle* particle, const Vector3D& force );
	void setContactPoint(const Vector3D &contactPoint );
	const Vector3D& getContactPoint() const;
	Vector3D getContactPointCG(const Vector3D& cg) const;
	const Vector3D& getAppliedForce() const;
	bool isActive(Particle* p) const;
	void setIsActive(bool bIsActive);

protected:
	Vector3D _contactPoint; // Model-Frame
	Vector3D _appliedForce; // Body Frame (same as Model as no rotation)
	bool _bIsActive;
};
