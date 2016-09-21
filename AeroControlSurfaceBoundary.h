#pragma once

#include "BoundaryHelperT.h"

class AeroSectionSubElementForceGenerator;
class AeroControlSurface;
class GSRigidBody;

class AeroControlSurfaceBoundary : public AeroSectionBoundary<AeroControlSurfaceBoundary>
{
public:
	AeroControlSurfaceBoundary(AeroSectionSubElementForceGenerator *pSefg, AeroControlSurface *pControlSurface );
	
	AeroSectionSubElementForceGenerator* parent();
	
	void setFront(float v);
	void setBack(float v);
    void rotateNormal( Vector3F & v );
    void drawNormalBoundaryLines(GSRigidBody* b, Renderer* r);

private:
	AeroSectionSubElementForceGenerator *_pSubElementForceGenerator;
	AeroControlSurface *_pControlSurface;
	float _front, _back;
};
