#pragma once

#include <jibbs/gps/GPSLocation.h>
#include <cstdint>
#include <vector>

class MipMapTerrainMeshModel;
class Renderer;

class RunwayLights
{
public:
    RunwayLights(const MipMapTerrainMeshModel& terrain, const GPSLocation& eastEnd,
                 float heading, float length, float westThresholdDisplacement);

    void render(Renderer& renderer, const GPSLocation& camera, std::int64_t elapsedMs,
                float daylight) const;

private:
    struct Batch
    {
        void add(const Vector3F& position);
        int lampCount() const;
        void draw(Renderer& renderer, const Vector3F& colour, float diameter,
                  float alpha, int lamp = -1) const;

        std::vector<float> centres;
        std::vector<float> corners;
    };

    struct Approach
    {
        Vector3F threshold;
        Vector3F inward;
        Batch thresholdBar;
        Batch stopBar;
        Batch steadyLights;
        Batch strobes;
    };

    GPSLocation _origin;
    Batch _edgeLights;
    Batch _centreline;
    Approach _east;
    Approach _west;
};
