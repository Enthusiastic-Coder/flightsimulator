#include "stdafx.h"
#include <limits>
#include "GSRigidBody.h"
#include "AeroControlSurfaceBoundary.h"
#include "AeroSectionElementForceGenerator.h"
#include "AeroForceGenerator.h"
#include "AeroControlSurface.h"
#include "AeroSectionForceGenerator.h"
#include "AeroSectionSubElementForceGenerator.h"
#include "AirProperties.h"
#include "OpenGLRenderer.h"

AeroSectionSubElementForceGenerator::AeroSectionSubElementForceGenerator(AeroSectionElementForceGenerator *parent) :
	_parent(parent),
	_clAdjustRatio(1.0)
{}

AeroSectionElementForceGenerator * AeroSectionSubElementForceGenerator::parent()
{
	return _parent;
}

void AeroSectionSubElementForceGenerator::addControlSurfaceReference(AeroControlSurface *pControlSurfaceRef)
{
	float u[4] = {}, v[4] = {};

	Vector3F dMlow = pControlSurfaceRef->getBoundary(1) - pControlSurfaceRef->getBoundary(0);
	Vector3F dMhigh = pControlSurfaceRef->getBoundary(2) - pControlSurfaceRef->getBoundary(3);

	for( size_t i=0; i < 4; i++ )
	{
		pControlSurfaceRef->calcFractionT( getBoundary(i), &u[i], &v[i] );
		
		if( u[i] < 0.0f ) 
			u[i] = 0.0f;

		if( u[i] > 1.0f ) 
			u[i] = 1.0f;

		Vector3F p;

		if( i < 2 )
			p = pControlSurfaceRef->getBoundary(0) + (u[i] * dMlow.Magnitude()) * dMlow.Unit();
		else
			p = pControlSurfaceRef->getBoundary(3) + (u[i] * dMhigh.Magnitude()) * dMhigh.Unit();

		calcFractionT( p, &u[i], &v[i] );
		v[i] *= 0.99;
	}

	u[0] = 0.01f;
	u[1] = 0.99f;
	u[2] = 0.99f;
	u[3] = 0.01f;

	float back1 = (calcRealT( 0, v[0] ) - calcRealT( 0, v[3] )).Magnitude();
	float back2 = (calcRealT( 1, v[1] ) - calcRealT( 1, v[2] )).Magnitude();
	float back = (back1 + back2 ) / 2.0f;

	float front1 = (calcRealT( 0, v[3] ) - calcRealT( 0, 1 )).Magnitude();
	float front2 = (calcRealT( 1, v[2] ) - calcRealT( 1, 1 )).Magnitude();
	float front = (front1 + front2) / 2.0;

	//calculate front back
	auto* pControlSurfaceBoundary = new AeroControlSurfaceBoundary(this, pControlSurfaceRef);
	pControlSurfaceBoundary->setBack( back );
	pControlSurfaceBoundary->setFront( front );
	pControlSurfaceBoundary->setBoundaryFromParentT( u, v );

    _controlSurfaceBoundaries.push_back( pControlSurfaceBoundary );
}

AoaClData* AeroSectionSubElementForceGenerator::getAoaClData()
{
	return parent()->parent()->parent()->getClCdCm();
}

void AeroSectionSubElementForceGenerator::onApplyForce( Particle *p, double dt ) 
{
	GSRigidBody *pBody = static_cast<GSRigidBody *>(p);

	Vector3D normal = getNormal().toDouble();

	Vector3D radialDist = getContactPointCG(pBody->cg());
	Vector3D velocityTotalLocal = pBody->getTotalBodyVelocity(getContactPoint());
	velocityTotalLocal -= pBody->toLocalGroundFrame( getWeather()->getWindFromPosition( getContactPoint()) );

	Vector3D velUnit = velocityTotalLocal.Unit();
	Vector3D dragUnit = -velUnit;

	double fVelocity = AirProperties::Airspeed( velocityTotalLocal.Magnitude(), pBody->Height(), 0 );
	velocityTotalLocal = fVelocity * velUnit;
	double fWingSpeedVelocity2 = pow(velocityTotalLocal * normal, 2 ) + pow( velocityTotalLocal * getNormalFlow(), 2 );

	double dAspectRatio = parent()->parent()->parent()->getAspectRatio();
	double dArea = getArea();
	double dEfficiency = parent()->parent()->getEfficiency();

	double tmp = normal * dragUnit;
	
    tmp = std::max( std::min( tmp, 1.0 ), -1.0 );

	double fAoa = RadiansToDegrees(asin( tmp ));

	double cl(0.0), cd(0.0), cm(0.0);
	auto *pAOACLData = getAoaClData();
	
	if( pAOACLData ) 
		pAOACLData->ClCdCm( fAoa, cl, cd, cm );

	cl *= _clAdjustRatio; //Flap Control surfaces will adjust Cl.
	cd *= _clAdjustRatio;

	double spdOfSound = AirProperties::SpeedOfSound(pBody->Height(), 0 );
    double fVelSpdSoundRatio = std::min(1.0,fVelocity/spdOfSound);
	double spdOfSoundFactor = sqrt(1 - pow(fVelSpdSoundRatio, 2 ));
	double factor = M_PI * dAspectRatio * dEfficiency * ((cl > 0) ? 1 : -1);

	if( fabs(spdOfSoundFactor) > std::numeric_limits<double>::epsilon())
	{
		cl = cl/ ( 1.0 + cl/factor ) / spdOfSoundFactor;
		cd = cd/ ( 1.0 + cd/factor ) / spdOfSoundFactor;
		cm = cm/ ( 1.0 + cm/factor ) / spdOfSoundFactor;
	}
	else
		cl = cd = cm = 0.0;

	double fDynamicPressureArea = 0.5 * AirProperties::Density(pBody->Height()) * fWingSpeedVelocity2 * dArea;

	double fForce = fDynamicPressureArea * cl ;
	double fMomentForce = fDynamicPressureArea * cm * getChord() / radialDist.Magnitude();

	double fDrag = fDynamicPressureArea * cd;
	double fInducedDrag = fDynamicPressureArea * cl*cl / (M_PI * dAspectRatio * dEfficiency);

	double fFlagCoeff = 0.2 * fDynamicPressureArea / dArea;

	for( size_t i = 0; i < _controlSurfaceBoundaries.size(); ++i )
		fDrag += fFlagCoeff * _controlSurfaceBoundaries[i]->getArea() * fabs(_controlSurfaceBoundaries[i]->getNormal() * parent()->getNormal());

	Vector3D liftNormal = (dragUnit ^ normal) ^ dragUnit;

	pBody->setBodyFrame();
	applyForce( p, (fForce + fMomentForce) * liftNormal + (fDrag+fInducedDrag) * dragUnit);

	if( parent()->parent()->parent()->isWashing() )
	{
		double angle = 2 * cl / dAspectRatio / M_PI;
		_downWash = -sqrt(fWingSpeedVelocity2) * (liftNormal * sin(angle) + velUnit * (1 - cos(angle)) );
	}
}

void AeroSectionSubElementForceGenerator::drawForceGenerator(GSRigidBody* b, Renderer* r)
{
    const Vector3D& n = getNormal().toDouble();


    float vertices[] = {
        getBoundaryPtr(0)->x, getBoundaryPtr(0)->y, getBoundaryPtr(0)->z,
        getBoundaryPtr(1)->x, getBoundaryPtr(1)->y, getBoundaryPtr(1)->z,
        getBoundaryPtr(2)->x, getBoundaryPtr(2)->y, getBoundaryPtr(2)->z,
        getBoundaryPtr(3)->x, getBoundaryPtr(3)->y, getBoundaryPtr(3)->z,
    };

    float colors[] = {
        1, 0, 1, 1,
        1, 0, 1, 1,
        1, 0, 1, 1,
        1, 0, 1, 1,
    };

    r->bindVertex(Renderer::Vertex, 3, vertices);
    r->bindVertex(Renderer::Color, 4, colors);
    r->setPrimitiveType(GL_LINE_LOOP);
    r->setUseIndex(false);
    r->setVertexCountOffset(indicesCount(vertices,3));

    r->Render();

	const Vector3D& cp = getContactPoint();
	Vector3D ncp = n + cp;

    float vertices2[] = {
                cp.x, cp.y, cp.z,
                ncp.x, ncp.y, ncp.z
    };

    float colors2[] = {
        0, 0, 1, 1,
        0, 0, 1, 1
    };

    r->setVertexCountOffset(indicesCount(vertices2, 3));
    r->bindVertex(Renderer::Vertex, 3, vertices2);
    r->bindVertex(Renderer::Color, 4, colors2);
    r->setPrimitiveType(GL_LINES);
    r->Render();
    r->unBindBuffers();

    for( AeroControlSurfaceBoundary* it : _controlSurfaceBoundaries )
        it->drawNormalBoundaryLines(b, r);

    GSForceGenerator::drawForceGenerator(b, r);
}

void AeroSectionSubElementForceGenerator::rotateNormal( Vector3F& normal )
{
    for( AeroControlSurfaceBoundary* it : _controlSurfaceBoundaries )
        it->rotateNormal( normal );
}

void AeroSectionSubElementForceGenerator::setClAdjustRatio(float ratio)
{
	_clAdjustRatio = ratio;
}

const Vector3D& AeroSectionSubElementForceGenerator::getDownWash() const
{
	return _downWash;
}

void AeroSectionSubElementForceGenerator::setRecorderHook(FlightRecorder& a) 
{
	GSForceGenerator::setRecorderHook(a);
	a.addDataRef(_downWash);
}



