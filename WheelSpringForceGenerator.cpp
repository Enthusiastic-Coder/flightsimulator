#include "stdafx.h"
#include "WheelSpringForceGenerator.h"
#include "SpringModel.h"
#include "GSRigidBody.h"


WheelSpringForceGenerator::WheelSpringForceGenerator()
{
	spring().setHasWheels(true);
}

void WheelSpringForceGenerator::onApplyForce(Particle *p, double dt)
{
	GSRigidBody* pBody = static_cast<GSRigidBody*>(p);
	QuarternionD qSteering = MathSupport<double>::MakeQHeading(spring().getLocalSteeringDirection() - pBody->getEuler().y);
	spring().setSteeringOrientation(qSteering);
	SpringForceGenerator::onApplyForce(p, dt);
}

////////////////////////////

WheelSpringForceGenerator::WheelSpringPivotObject::WheelSpringPivotObject(SpringModel& spring, bool bIsSuspensionOnly, bool bNoRotation) :
	SpringPivotObject(spring),
	_bIsSuspensionOnly(bIsSuspensionOnly),
	_bNoRotation(bNoRotation)
{ }

void WheelSpringForceGenerator::WheelSpringPivotObject::applyTransform(unsigned int camID)
{
	if (_bIsSuspensionOnly)
		SpringPivotObject::applyTransform(camID);
	else
	{
		Matrix4x4D& mv = OpenGLPipeline::Get(camID).GetModel().Top();
		mv.Translate(getT());
		mv.Translate(0, _spring.getWheelRadius() - _spring.displacementDistance(), 0);

		bool hasR = !(getR() == Vector3F::Null());

		if (hasR)
		{
			mv.Translate(-getT());
			mv.Translate(getR());
		}

		mv.RotateModel(0, _spring.getSteeringAngle(), 0);

		if (hasR)
		{
			mv.Translate(-getR());
			mv.Translate(getT());
		}

		if (!_bNoRotation)
			mv.RotateModel(-_spring.wheelAnglePosition(), 0, 0);

		mv.Translate(0, -_spring.getWheelRadius(), 0);
		mv.Translate(-getT());
	}
}