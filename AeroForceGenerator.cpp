#include "stdafx.h"
#include "CompositeListForceGenerator.h"
#include "AeroControlSurface.h"
#include "AeroSectionForceGenerator.h"
#include "AeroSectionSubElementForceGenerator.h"
#include "AeroSectionElementForceGenerator.h"
#include "AeroForceGenerator.h"


AeroForceGenerator::AeroForceGenerator(AoaClData *pAoadata, bool isWashing) : 
	CompositeListForceGenerator(nullptr),
	_pAoaClData(pAoadata),
	_controlSurface0(nullptr),
	_fAspectRatio(0.0),
	_bIsWashing(isWashing)
{}

AeroForceGenerator::~AeroForceGenerator()
{
	clearControlSurface0();
}

void AeroForceGenerator::clearControlSurface0()
{
	if( _controlSurface0 )
	{
		delete _controlSurface0;
		_controlSurface0 = nullptr;
	}
}

void AeroForceGenerator::drawForceGenerator(GSRigidBody* p, Renderer* r)
{
    CompositeListForceGenerator<AeroForceGenerator, AeroSectionForceGenerator, AeroForceGenerator>::drawForceGenerator(p, r);
}

void AeroForceGenerator::setControlSurface0(AeroControlSurface *controlSurface0)
{
	clearControlSurface0();
	_controlSurface0 = controlSurface0;
}

AeroControlSurface* AeroForceGenerator::controlSurface0()
{
	return _controlSurface0;
}

AoaClData* AeroForceGenerator::getClCdCm()
{
	return _pAoaClData;
}

void AeroForceGenerator::setRecorderHook(FlightRecorder& a)
{
	if( _controlSurface0 )
		_controlSurface0->setRecorderHook(a);

	CompositeListForceGenerator<AeroForceGenerator, AeroSectionForceGenerator, AeroForceGenerator>::setRecorderHook(a);
}

void AeroForceGenerator::calcAspectRatio()
{
	double fTotalArea = 0.0;
	double fSpan = 0.0;

	for( size_t i=0; i < size(); i++ )
	{
		fSpan += element(i)->span();
		fTotalArea += element(i)->getArea();
	}

	_fAspectRatio = 2.0 * fSpan * fSpan / fTotalArea;
}

double AeroForceGenerator::getAspectRatio()
{
	return _fAspectRatio;
}

bool AeroForceGenerator::isWashing()
{
	return _bIsWashing;
}


