#include "stdafx.h"
#include <jibbs/boundary/BoundaryHelperT.h>
#include "CompositeListForceGenerator.h"
#include "AeroSectionSubElementForceGenerator.h"
#include "AeroSectionForceGenerator.h"
#include "AeroForceGenerator.h"
#include "AeroSectionElementForceGenerator.h"
#include "OpenGLRenderer.h"

AeroSectionElementForceGenerator::AeroSectionElementForceGenerator(AeroSectionForceGenerator *p) 
	: CompositeListForceGenerator(p) {}

void AeroSectionElementForceGenerator::drawForceGenerator(GSRigidBody* b, Renderer* r)
{
    float vertices[] = {
        getBoundaryPtr(0)->x, getBoundaryPtr(0)->y, getBoundaryPtr(0)->z,
        getBoundaryPtr(1)->x, getBoundaryPtr(1)->y, getBoundaryPtr(1)->z,
        getBoundaryPtr(2)->x, getBoundaryPtr(2)->y, getBoundaryPtr(2)->z,
        getBoundaryPtr(3)->x, getBoundaryPtr(3)->y, getBoundaryPtr(3)->z,
    };

    float colors[] = {
        1, 1, 0, 1,
        1, 1, 0, 1,
        1, 1, 0, 1,
        1, 1, 0, 1,
    };

    r->bindVertex(Renderer::Vertex, 3, vertices);
    r->bindVertex(Renderer::Color, 4, colors);
    r->setPrimitiveType(GL_LINE_LOOP);
    r->setUseIndex(false);
    r->setVertexCountOffset(arrayCount(vertices)/3);

    r->Render();
    r->unBindBuffers();

    CompositeListForceGenerator<AeroSectionElementForceGenerator, AeroSectionSubElementForceGenerator, AeroSectionForceGenerator>
        ::drawForceGenerator(b,r);// Want to draw sub items.
}

