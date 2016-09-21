#pragma once
#include "datarecorder.h"

class FlightRecorder : public DataRecorder
{
public:
	FlightRecorder();
    void hookEuler( const Vector3D & euler);
	virtual void Extrapolate(DataRecord *start,DataRecord *end);

protected:
	void ExtrapolateEuler(DataRecord *start,DataRecord *end, int iRecordOffset);

private:	
	int _iHdgRecordOffset;
	int _iBankRecordOffset;	
	int _iPitchRecordOffset;
};
