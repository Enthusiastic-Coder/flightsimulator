#include "AeroWheelSpringForceGenerator.h"
#include "GSRigidBody.h"
#include "AirProperties.h"

void AeroWheelSpringForceGenerator::onApplyForce(Particle *p, double dt)
{
	GSRigidBody* pBody = static_cast<GSRigidBody*>(p);
	WheelSpringForceGenerator::onApplyForce(p, dt);

	Vector3D cp = getContactPoint();
	Vector3D velocityTotalLocal = pBody->getTotalBodyVelocity(cp);
	velocityTotalLocal -= pBody->toLocalGroundFrame(getWeather()->getWindFromPosition(cp));

	Vector3D dragUnit = -velocityTotalLocal.Unit();

	double fVelocity = velocityTotalLocal.Magnitude();
	double fDensity = AirProperties::Density(pBody->Height());
	double fDrag = 0.05 * fDensity * fVelocity * fVelocity;

	pBody->setBodyFrame();
	applyForce(p, fDrag * dragUnit);
}
