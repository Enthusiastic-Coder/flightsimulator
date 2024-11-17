#pragma once

#include <jibbs/boundary/BoundaryHelperT.h>
#include "GSForceGenerator.h"
#include "AeroSectionSubElementForceGenerator.h"

class AeroSectionElementForceGenerator;
class AeroControlSurfaceBoundary;
class AoaClData;
class AeroControlSurface;

class AeroSectionSubElementForceGenerator : 
	public GSForceGenerator, 
	public AeroSectionBoundary<AeroSectionSubElementForceGenerator>
{
public:
	FORCEGENERATOR_TYPE( Type_AeroSectionSubElement )

	AeroSectionSubElementForceGenerator(AeroSectionElementForceGenerator *parent);
	AeroSectionElementForceGenerator * parent();
	void addControlSurfaceReference(AeroControlSurface *pControlSurfaceRef);
	AoaClData *getAoaClData();

	void onApplyForce( Particle *p, double dt ) override;
    void drawForceGenerator(GSRigidBody *b, Renderer *r) override;
	void setRecorderHook(FlightRecorder& a) override;

	void rotateNormal( Vector3F& normal );
	void setClAdjustRatio(float ratio);
	const Vector3D& getDownWash() const;

private:
	AeroSectionElementForceGenerator* _parent;
    std::vector<AeroControlSurfaceBoundary*> _controlSurfaceBoundaries;
	Vector3D _downWash;
	float _clAdjustRatio;
};
