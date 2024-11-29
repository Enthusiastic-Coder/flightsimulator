#include "stdafx.h"
#include "AeroControlSurface.h"
#include "AeroSectionForceGenerator.h"
#include "AeroSectionElementForceGenerator.h"
#include "AeroSectionSubElementForceGenerator.h"
#include "AeroForceGenerator.h"
#include "AeroControlSurfaceBoundary.h"
#include "OpenGLRenderer.h"

AeroControlSurfaceBoundary::AeroControlSurfaceBoundary(AeroSectionSubElementForceGenerator *pSefg, AeroControlSurface *pControlSurface ) : 
	_pSubElementForceGenerator( pSefg ), 
	_pControlSurface( pControlSurface ), 
	_front(0.0f), 
	_back(1.0f)
{
	AeroControlSurface *pControlSurface0 = _pSubElementForceGenerator->parent()->parent()->parent()->controlSurface0();

	_pControlSurface->setParent( pControlSurface0);
	_pControlSurface->add( _pSubElementForceGenerator );

	if( pControlSurface0 )
		pControlSurface0->add( _pSubElementForceGenerator );
}

AeroSectionSubElementForceGenerator* AeroControlSurfaceBoundary::parent()
{
	return _pSubElementForceGenerator;
}

void AeroControlSurfaceBoundary::setFront(float v)
{
	_front = v;
}

void AeroControlSurfaceBoundary::setBack(float v)
{
	_back = v;
}

void AeroControlSurfaceBoundary::rotateNormal( Vector3F & v )
{
	QuarternionF qRot = _pControlSurface->onDeflectionChange(_front, _back);
	_pControlSurface->rotateNormal( qRot, v );
}

void AeroControlSurfaceBoundary::drawNormalBoundaryLines(GSRigidBody* b, Renderer* r)
{
    float vertices[] = {
        getBoundaryPtr(0)->x, getBoundaryPtr(0)->y, getBoundaryPtr(0)->z,
        getBoundaryPtr(1)->x, getBoundaryPtr(1)->y, getBoundaryPtr(1)->z,
        getBoundaryPtr(2)->x, getBoundaryPtr(2)->y, getBoundaryPtr(2)->z,
        getBoundaryPtr(3)->x, getBoundaryPtr(3)->y, getBoundaryPtr(3)->z,
    };

    float colors[] = {
        1, 1, 1, 1,
        1, 1, 1, 1,
        1, 1, 1, 1,
        1, 1, 1, 1,
    };

    r->setPrimitiveType(GL_LINE_LOOP);
    r->setUseIndex(false);
    r->setVertexCountOffset(indicesCount(vertices,3));
    r->bindVertex(Renderer::Vertex, 3, vertices);
    r->bindVertex(Renderer::Color, 4, colors);

    r->Render();

//    VectorF normal = getNormal();
//    VectorF cp = (getBoundary(0)+getBoundary(1)+getBoundary(2)+getBoundary(3))/4.0f;
//    VectorF normalContactPoint = normal + cp;

//    float vertices2[] = {
//        cp.x, cp.y, cp.z,
//        normalContactPoint.x, normalContactPoint.y, normalContactPoint.z
//    };

//    float colors2[] = {
//        0.5f, 0.5f, 0.0f, 1.0f,
//        0.5f, 0.5f, 0.0f, 1.0f,
//    };

//    r->bindVertex(Renderer::Vertex, 3, vertices2);
//    r->bindVertex(Renderer::Color, 4, colors2);
//    r->setPrimitiveType(GL_LINES);
//    r->setVertexCountOffset(sizeof(vertices2)/sizeof(float)/3);
//    r->Render();
    r->unBindBuffers();

}
