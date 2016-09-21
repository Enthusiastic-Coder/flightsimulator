#include "StdAfx.h"
#include "FlightRecorder.h"

FlightRecorder::FlightRecorder() :
	_iHdgRecordOffset(-1),
	_iBankRecordOffset(-1),
	_iPitchRecordOffset(-1)
{
}

void FlightRecorder::hookEuler( const Vector3D & euler)
{
	_iPitchRecordOffset = addDataRef(euler.x);
	_iHdgRecordOffset = addDataRef(euler.y);
	_iBankRecordOffset = addDataRef(euler.z);
}

void FlightRecorder::Extrapolate(DataRecord *start,DataRecord *end)
{
	ExtrapolateEuler( start, end, _iHdgRecordOffset );
	ExtrapolateEuler( start, end, _iBankRecordOffset );
	ExtrapolateEuler( start, end, _iPitchRecordOffset );
	DataRecorder::Extrapolate(start, end);
}

void FlightRecorder::ExtrapolateEuler(DataRecord *start,DataRecord *end, int iRecordOffset)
{
	if( iRecordOffset == -1 )
		return;

	double& fEndEuler = *(double*)end->data(iRecordOffset);
	double& fStartEuler = *(double*)start->data(iRecordOffset);;
	
	if( _iHdgRecordOffset == iRecordOffset )
	{
		if( fStartEuler < 0 )
			fStartEuler += 360;

		if( fEndEuler < 0 )
			fEndEuler += 360;

		if( fStartEuler < 90 && fEndEuler > 270 )
			fEndEuler -= 360;

		if( fEndEuler < 90 && fStartEuler > 270 )
			fStartEuler -= 360;
	}
	else
	{
		if( fStartEuler < -90 && fEndEuler > 90 )
			fEndEuler -= 360;

		if( fEndEuler < -90 && fStartEuler > 90 )
			fStartEuler -= 360;
	}
}
