#include "stdafx.h"
#include <limits>
#include <jibbs/boundary/BoundaryHelperT.h>
#include "FlightRecorder.h"
#include "AeroSectionSubElementForceGenerator.h"
#include "AeroSectionForceGenerator.h"
#include "AeroControlSurface.h"
#include "AeroSectionElementForceGenerator.h"

AeroControlSurface::AeroControlSurface( Vector3F vRotAxis, float dist_per_defl) :
    _parent( nullptr ),
    _deflection( 0.0 ),
    _vRotAxis( vRotAxis.Unit() ),
    _distance_per_deflection(dist_per_defl)
{
	_qDeflectionRotAxis = onDeflectionChange(0.0f, 1.0f );
}

void AeroControlSurface::setParent( AeroControlSurface *parent )
{
	_parent = parent;
}

inline AeroControlSurface* AeroControlSurface::parent()
{
	return _parent;
}

void AeroControlSurface::add( AeroSectionSubElementForceGenerator* s )
{
	_subElementForceGeneratorList.push_back( s );
}

void AeroControlSurface::setDeflection(float deflection)
{
	_deflection = deflection;
	onDeflectionChange();
}

float AeroControlSurface::getDeflection() const
{
	return _deflection;
}

const Vector3F& AeroControlSurface::getShiftTranslation()
{
	return _vDistDeflection;
}

const Vector3F& AeroControlSurface::getRotAxis() const
{
	return _vRotAxis;
}

void AeroControlSurface::setRecorderHook(FlightRecorder& a)
{
	a.addDataRef(_deflection);
}

void AeroControlSurface::onDeflectionChange()
{
	bool bIsFlap = fabs(_distance_per_deflection) > std::numeric_limits<float>::epsilon();

	if( parent() == nullptr && !bIsFlap)
		_qDeflectionRotAxis = onDeflectionChange(0.0f, 1.0f );

	if(bIsFlap)
		_vDistDeflection = _distance_per_deflection * _deflection * Vector3F(0,0,-1);

	for( size_t i=0; i < _subElementForceGeneratorList.size(); ++i )
	{
		AeroSectionSubElementForceGenerator * sefg = _subElementForceGeneratorList[i];

		if (bIsFlap)
		{
			// FLAP adjust Cl ratio else rotate normal
			sefg->setClAdjustRatio(1.0f + _deflection / 10.0f);
		}
		else
		{
			sefg->setNormal(sefg->parent()->getNormal());
			Vector3F& normal = const_cast<Vector3F&>(sefg->getNormal());
			sefg->rotateNormal(normal);
			rotateNormal(normal);
		}		
	}
}

QuarternionF AeroControlSurface::onDeflectionChange(float front, float back)
{
    //return MathSupport<float>::MakeQ(-_deflection * getRotAxis());
    float deflection = DegreesToRadians(-_deflection );
    float dBack = back + _distance_per_deflection * _deflection;
    float phi = atan2f( dBack * sin(deflection), front + dBack * cos(deflection) );
    return MathSupport<float>::MakeQOrientation( RadiansToDegrees( phi ) * getRotAxis());
}

void AeroControlSurface::rotateNormal( Vector3F & v )
{
	if( parent() )
		parent()->rotateNormal( v );
	else
		rotateNormal( _qDeflectionRotAxis, v );
}

void AeroControlSurface::rotateNormal( QuarternionF& qRotAxis, Vector3F & v )
{
	v = QVRotate( qRotAxis, v );
}

