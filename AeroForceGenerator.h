#pragma once

#include "AoaClData.h"
#include "CompositeListForceGenerator.h"
#include "AeroSectionForceGenerator.h"

class AeroForceGenerator : public CompositeListForceGenerator<AeroForceGenerator,AeroSectionForceGenerator,AeroForceGenerator>
{
public:
	FORCEGENERATOR_TYPE( Type_Aero )
	AeroForceGenerator(AoaClData *, bool isWashing=false);
	virtual ~AeroForceGenerator();
	
    void drawForceGenerator(GSRigidBody *p, Renderer *r) override;
	void setRecorderHook(FlightRecorder& a) override;
	
	void clearControlSurface0();
	void setControlSurface0(AeroControlSurface *controlSurface0);
	AeroControlSurface* controlSurface0();
	AoaClData *getClCdCm();
	void calcAspectRatio();
	double getAspectRatio();
	bool isWashing();

private:
	AoaClData *_pAoaClData;
	AeroControlSurface* _controlSurface0;
	double _fAspectRatio;
	bool _bIsWashing;
};

