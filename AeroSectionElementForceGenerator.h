#pragma once

#include "CompositeListForceGenerator.h"

class AeroSectionForceGenerator;

class AeroSectionElementForceGenerator : 
	public CompositeListForceGenerator<AeroSectionElementForceGenerator,AeroSectionSubElementForceGenerator,AeroSectionForceGenerator>,
	public AeroSectionBoundary<AeroSectionElementForceGenerator>
{
public:
	FORCEGENERATOR_TYPE( Type_AeroSectionElement )
	AeroSectionElementForceGenerator(AeroSectionForceGenerator *p);
    void drawForceGenerator(GSRigidBody *b, Renderer *r) override;
};

