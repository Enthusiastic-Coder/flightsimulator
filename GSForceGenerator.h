#pragma once

enum ForceGeneratorType
{
	Type_None,
	Type_SpringNormal,
	Type_WheelSpringNormal,
	Type_AeroWheelSpringNormal,
	Type_Gravity,
	Type_AircraftEngine,
	Type_Aero,
	Type_AeroSection,
	Type_AeroSectionElement,
	Type_AeroSectionSubElement,
	Type_SpringConstrained,
	Type_WheelSpringConstrained,
	Type_Custom
};

#define FORCEGENERATOR_TYPE( type_to_return ) \
	ForceGeneratorType Type() override\
	{\
		return ForceGeneratorType::type_to_return;\
	}

#define FG_TYPE( type ) ForceGeneratorType::type

#define FG_TYPE_IS_SPRINGNORMAL( type )				(FG_TYPE(Type_SpringNormal) == type)
#define FG_TYPE_IS_WHEELSPRINGNORMAL( type )		(FG_TYPE(Type_WheelSpringNormal) == type)
#define FG_TYPE_IS_AEROWHEELSPRINGNORMAL( type )	(FG_TYPE(Type_AeroWheelSpringNormal) == type)
#define FG_TYPE_IS_GRAVITY( type )					(FG_TYPE(Type_Gravity) == type)
#define FG_TYPE_IS_AIRCRAFTENGINE( type )			(FG_TYPE(Type_AircraftEngine) == type)
#define FG_TYPE_IS_AERO( type )						(FG_TYPE(Type_Aero) == type)
#define FG_TYPE_IS_SPRINGCONSTRAINED( type )		(FG_TYPE(Type_SpringConstrained) == type)
#define FG_TYPE_IS_WHEELSPRINGCONSTRAINED( type )	(FG_TYPE(Type_WheelSpringConstrained) == type)
#define FG_TYPE_IS_CUSTOM( type )					(FG_TYPE(Type_Custom) == type)

class JSONRigidBody;
class WorldSystem;
class GSRigidBody;

#include "ForceGenerator.h"
#include "FlightRecorder.h"
#include "interfaces.h"

class GSForceGenerator :
	public ForceGenerator,
	public IForceGeneratorDrawable,
	public IDataRecordable<FlightRecorder>
{
public:
    GSForceGenerator(WorldSystem* pWorldSystem = 0, float dForceLineLength = 100.0f);

	virtual void onInitialise(WorldSystem* pWorldSystem);
	virtual bool onCheckActive(Particle *p);

    virtual void onApplyForce(Particle *p, double dt) override;
	virtual void setRecorderHook(FlightRecorder& a) override;

    void drawForceGenerator(GSRigidBody* pBody, Renderer* r) override;

	virtual ForceGeneratorType Type() = 0;

	ISceneryEnvironment * getEnv();
	IWeather * getWeather();
	IJoystick * getJoystick();
	WorldSystem* getWorld();

	class PivotObject
	{
	public:
		PivotObject();
		PivotObject(PivotObject* pParent);

		PivotObject* getParent();
		virtual void applyTransform(unsigned int camID) = 0;

		void setPivot(Vector3F t, Vector3F r);
		const Vector3F& getT() const;
		const Vector3F& getR() const;

	private:
		PivotObject* _pParent;
		Vector3F _TRANSLATE;
		Vector3F _ROTATION;
	};

	void addPivotObject(PivotObject* po);
	PivotObject* getPivotObject(size_t idx);
	size_t pivotObjectCount();
	void setLengthForceLines(float scale);

private:
	WorldSystem* _pWorldSystem;
	float _dForceLinesLength;
    std::vector<PivotObject*> _pivotObjects;
    float _dynCol[4];
};
