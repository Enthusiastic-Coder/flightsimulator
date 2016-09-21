#pragma once

#include "GSForceGenerator.h"

class AircraftEngineForceGenerator : public GSForceGenerator
{
public:
	FORCEGENERATOR_TYPE(Type_AircraftEngine)

	class AircraftEnginePivotObject : public PivotObject
	{
	public:
		AircraftEnginePivotObject(AircraftEngineForceGenerator* pEngine);
		void applyTransform(unsigned int camID) override;

	private:
		AircraftEngineForceGenerator* _pEngine;
	};

	AircraftEngineForceGenerator();

	void setRecorderHook(FlightRecorder& a) override;
	void setMaxThrust(float value);
	void setMaxSpd(float maxMach);
	void incrDrag(float cd, float area, float n1=0.92f);
	void setEngineDir(Vector3F value);
	Vector3F getEngineDir();

	float _angle_position;
	float _angle_velocity;
	float _thrust_percent;
	float _actual_thrust_percent;

protected:
	void onApplyForce(Particle *p, double dt) override;
	float getMaxThrustAvailable(float spd, float fHeight);
	float getCurrentThrust(Particle* p);
	void updateRotorVelPos(double dt);

private:
	float _max_thrust;
	float _max_drag;
	float _vInvFrac;
	float _density0;
	float _max_spd;
	Vector3F _engine_direction;
};

