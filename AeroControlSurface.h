#pragma once

#include "FlightRecorder.h"
#include <jibbs/boundary/BoundaryHelperT.h>

class AeroSectionSubElementForceGenerator;
class AeroSectionForceGenerator;

class AeroControlSurface :	
	public AeroSectionBoundary<AeroControlSurface>,
	public IDataRecordable<FlightRecorder>
{
	friend class AeroControlSurfaceBoundary;

public:
    AeroControlSurface( Vector3F vRotAxis, float dist_per_defl = 0.0 );

	void add( AeroSectionSubElementForceGenerator* s );
	void setDeflection(float deflection);
	float getDeflection() const;
    const Vector3F& getShiftTranslation();
    const Vector3F& getRotAxis() const;
	void setRecorderHook(FlightRecorder& a) override;

private:
	void setParent( AeroControlSurface *parent );
	AeroControlSurface* parent();
	QuarternionF onDeflectionChange(float front, float back);
	void onDeflectionChange();
    void rotateNormal( Vector3F & v );
    void rotateNormal( QuarternionF& qRotAxis, Vector3F & v );
	
private:
	AeroControlSurface *_parent;
	std::vector<AeroSectionSubElementForceGenerator*> _subElementForceGeneratorList;
	float _deflection;
    Vector3F _vRotAxis;
    Vector3F _vDistDeflection;
	QuarternionF _qDeflectionRotAxis;
	float _distance_per_deflection;
};

/////////////////////////////////////////////////////////////////////////

