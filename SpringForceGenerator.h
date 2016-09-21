#pragma once

#include "GSForceGenerator.h"
#include "SpringModel.h"
#include "OpenGLPipeline.h"

class SpringForceGenerator : public GSForceGenerator
{
public:
	FORCEGENERATOR_TYPE(Type_SpringNormal)

		void reset() override;
	void setRecorderHook(FlightRecorder& a) override;
	void onApplyForce(Particle *p, double dt) override;

	double Height();
	SpringModel& spring();

	class SpringPivotObject : public PivotObject
	{
	public:
		SpringPivotObject(SpringModel& spring) : _spring(spring) { }

		void applyTransform(unsigned int camID) override
		{
			OpenGLMatrixStack& mv = OpenGLPipeline::Get(camID).GetModel();
			mv.Top().Translate(0, -_spring.displacementDistance(), 0);
		}

	protected:
		SpringModel& _spring;
	};

private:
	SpringModel _spring;
};
