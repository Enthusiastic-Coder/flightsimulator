#ifndef SKYDOME_H
#define SKYDOME_H

#include "MeshData.h"
class Renderer;

class SkyDome
{
public:
    struct properties
    {
        float radius;
        int sides;
        int slices;
        float hemisphereCount;
    } ;

    SkyDome(float radius, int sides, int slices, bool bFullSphere=false);

    void Render(Renderer* r);

private:
    meshData _meshData;
    properties _prop;
};

#endif // SKYDOME_H
