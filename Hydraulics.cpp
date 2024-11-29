#include "Hydraulics.h"
#include "AeroControlSurface.h"

float Hydraulics::getDeflection( AeroControlSurface* pAfg ) const
{
	std::map<AeroControlSurface*,float>::const_iterator it = _controlSurfaces.find(pAfg);
	if( it == _controlSurfaces.end())
		return 0.0f;

	return it->second;
}

void Hydraulics::setResponseRate( AeroControlSurface* pAfg, float fRate)
{
	_rate[pAfg] = fRate;
}

void Hydraulics::setDeflection(AeroControlSurface* pAfg, float fDeflection)
{		
	_controlSurfaces[pAfg] = fDeflection;
}

void Hydraulics::update( float dt )
{
    for( std::pair<AeroControlSurface*,float> it : _controlSurfaces )
	{
		if (it.first == 0)
			continue;

		float fCurrentDeflection = it.first->getDeflection();
		float fRequestedDeflection = _controlSurfaces[it.first];

		float delta = fRequestedDeflection - fCurrentDeflection;
			
		if( fabs( delta ) > 0.1f )
		{
			float fRate = _rate[it.first];
			float dValue = fRate == 0 ? 40*dt : fRate * dt;

			if( fabs(delta) <  fabs(dValue) )
				it.first->setDeflection( fRequestedDeflection );
			else
				it.first->setDeflection( fCurrentDeflection + (delta > 0 ? dValue : -dValue) );
		}
	}
}
