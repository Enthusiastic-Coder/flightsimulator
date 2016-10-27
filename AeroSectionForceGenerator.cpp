#include "stdafx.h"
#include "CompositeListForceGenerator.h"
#include "AeroControlSurface.h"
#include "AeroSectionElementForceGenerator.h"
#include "AeroSectionSubElementForceGenerator.h"
#include "AeroForceGenerator.h"
#include "AeroSectionForceGenerator.h"
#include "OpenGLRenderer.h"


/////////////////////////////////////////////////////////////////////////////

AeroSectionForceGenerator::AeroSectionForceGenerator(AeroForceGenerator *p) :	
    CompositeListForceGenerator(p),
    _efficiency(0.745)
{}

double AeroSectionForceGenerator::getEfficiency()
{
    return _efficiency;
}

void AeroSectionForceGenerator::drawForceGenerator(GSRigidBody* b, Renderer* r)
{
    // Draw All surfaces
    for( size_t i=0; i <  controlSurfaceCount(); ++i )
    {
        AeroControlSurface *pmv = controlSurfaceN(i);

        float vertices[] = {
            pmv->getBoundaryPtr(0)->x, pmv->getBoundaryPtr(0)->y, pmv->getBoundaryPtr(0)->z,
            pmv->getBoundaryPtr(1)->x, pmv->getBoundaryPtr(1)->y, pmv->getBoundaryPtr(1)->z,
            pmv->getBoundaryPtr(2)->x, pmv->getBoundaryPtr(2)->y, pmv->getBoundaryPtr(2)->z,
            pmv->getBoundaryPtr(3)->x, pmv->getBoundaryPtr(3)->y, pmv->getBoundaryPtr(3)->z,
        };

        float colors[] = {
            0, 0, 0, 1,
            0, 0, 0, 1,
            0, 0, 0, 1,
            0, 0, 0, 1,
        };

        //r->bindVertexBuffer()
        r->bindVertex(Renderer::Vertex, 3, vertices);
        r->bindVertex(Renderer::Color, 4, colors);
        r->setPrimitiveType(GL_LINE_LOOP);
        r->setUseIndex(false);
        r->setVertexCountOffset(indicesCount(vertices,3));

        r->Render();
    }

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

    r->bindVertex(Renderer::Vertex, 3, vertices);
    r->bindVertex(Renderer::Color, 4, colors);
    r->setPrimitiveType(GL_LINE_LOOP);
    r->setUseIndex(false);
    r->setVertexCountOffset(indicesCount(vertices,3));

    r->Render();
    r->unBindBuffers();

    CompositeListForceGenerator<AeroSectionForceGenerator, AeroSectionElementForceGenerator, AeroForceGenerator>
            ::drawForceGenerator(b, r);// Want to draw sub items.
}

void AeroSectionForceGenerator::setRecorderHook(FlightRecorder& a)
{
    for( size_t i=0; i < controlSurfaceCount(); ++i )
        controlSurfaceN(i)->setRecorderHook(a);

    CompositeListForceGenerator<AeroSectionForceGenerator, AeroSectionElementForceGenerator, AeroForceGenerator>::setRecorderHook(a);
}

void AeroSectionForceGenerator::addControlSurface(AeroControlSurface *pControlSurface)
{
    _controlSurfaces.push_back(pControlSurface);
}

AeroControlSurface* AeroSectionForceGenerator::controlSurfaceN(int index)
{
    return _controlSurfaces[index];
}

size_t AeroSectionForceGenerator::controlSurfaceCount()
{
    return _controlSurfaces.size();
}
