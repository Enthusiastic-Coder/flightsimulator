#pragma once

#include "AeroControlSurface.h"
#include "GSForceGenerator.h"


class Hydraulics
{
public:
	float getDeflection(AeroControlSurface* pAfg) const;
	void setResponseRate(AeroControlSurface* pAfg, float fRate);
	void setDeflection(AeroControlSurface* pAfg, float fDeflection);
	void update(float dt);

private:
	std::map<AeroControlSurface*,float> _controlSurfaces;
	std::map<AeroControlSurface*,float> _rate;
};

