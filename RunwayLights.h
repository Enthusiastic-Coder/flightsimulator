#pragma once

#include <jibbs/gps/GPSLocation.h>
#include <array>
#include <cstdint>
#include <vector>

class MipMapTerrainMeshModel;
class Renderer;

class RunwayLights
{
public:
    RunwayLights(const MipMapTerrainMeshModel& terrain, const GPSLocation& eastEnd,
                 float heading, float length, float westThresholdDisplacement,
                 double glideSlopeDegrees, double eastDatumMetres, double westDatumMetres);

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
        Batch approachCentreline;
        Batch redSideRows;
        Batch strobes;
        GPSLocation papiThreshold;
        Vector3F papiInbound;
        float papiBeamOffset = 0;
        std::array<Batch, 4> papi;
    };

    GPSLocation _origin;
    double _glideSlopeDegrees;
    Batch _edgeLights;
    Batch _centreline;
    Batch _touchdownZone;
    Approach _east;
    Approach _west;
};
