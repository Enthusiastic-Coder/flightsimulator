#pragma once

#include <MathSupport.h>

struct MeshModelArgs
{
    BoundingBox _BoundingBox;
    size_t _polyCount = 0;
    float _fTotalVol = 0.0f;
    float _fTotalArea = 0.0f;
    Vector3F _centroidLocation;
};
