#include "stdafx.h"
#include "RunwayLights.h"
#include "MipMapTerrainMeshModel.h"
#include "OpenGLPipeline.h"
#include "OpenGLRenderer.h"
#include "OpenGLShaderProgram.h"

#include <jibbs/gps/GPSModelTransform.h>
#include <algorithm>
#include <cmath>
#include <iterator>

namespace
{
constexpr float lampCorners[] = {-1, -1, 1, -1, 1, 1, -1, -1, 1, 1, -1, 1};
constexpr int verticesPerLamp = 6;
}

RunwayLights::RunwayLights(const MipMapTerrainMeshModel& terrain, const GPSLocation& eastEnd,
                           float heading, float length, float westThresholdDisplacement,
                           double glideSlopeDegrees, double eastDatumMetres, double westDatumMetres)
    : _origin(eastEnd), _glideSlopeDegrees(glideSlopeDegrees)
{
    const GPSModelTransform modelSpace(_origin);
    const auto rotation = MathSupport<float>::MakeQHeading(heading);
    const auto lamp = [&](float lateral, float along, float height)
    {
        const auto offset = QVRotate(rotation, Vector3F(lateral, 0, -along));
        const GPSLocation position = _origin + offset;
        return modelSpace.ToLocal(position + (terrain.surfaceElevation(position) + height));
    };

    for (float along = 15.0f; along < length; along += 60.0f)
    {
        _edgeLights.add(lamp(-25.0f, along, 0.40f));
        _edgeLights.add(lamp(25.0f, along, 0.40f));
    }
    for (float along = 15.0f; along < length; along += 15.0f)
        _centreline.add(lamp(0, along, 0.23f));

    const auto addApproach = [&](Approach& approach, float threshold, float stopEnd, float direction,
                                 float ilsThreshold, double datumMetres)
    {
        approach.threshold = lamp(0, threshold, 0.3f);
        approach.inward = QVRotate(rotation, Vector3F(0, 0, -direction));
        // Use the ILS reference datum for the visual beam and place the lamps beside its origin.
        approach.papiBeamOffset = static_cast<float>(datumMetres / std::tan(DegreesToRadians(glideSlopeDegrees)));
        const auto thresholdLocation = _origin + QVRotate(rotation, Vector3F(0, 0, -ilsThreshold));
        approach.papiThreshold = thresholdLocation + terrain.surfaceElevation(thresholdLocation);
        const auto farEnd = _origin + QVRotate(rotation, Vector3F(0, 0, direction > 0 ? -length : 0));
        const auto axis = approach.papiThreshold.offSetTo(farEnd);
        approach.papiInbound = Vector3F(axis.x, 0, axis.z).Unit();
        const float papiAlong = ilsThreshold + direction * approach.papiBeamOffset;
        for (std::size_t i = 0; i < approach.papi.size(); ++i)
        {
            // Outer unit first, so the two red lights on slope are nearest the runway.
            const float lateral = -direction * (61.0f - 7.0f * static_cast<float>(i));
            approach.papi[i].add(lamp(lateral, papiAlong, 0.55f));
        }

        for (int i = 0; i < 18; ++i)
        {
            const float lateral = -25.0f + 50.0f * i / 17.0f;
            approach.thresholdBar.add(lamp(lateral, threshold, 0.30f));
            approach.stopBar.add(lamp(lateral, stopEnd, 0.30f));
        }

        for (int distance = 30; distance <= 270; distance += 30)
        {
            const float along = threshold - direction * distance;
            for (float lateral : {8.0f, 10.0f, 12.0f})
            {
                approach.redSideRows.add(lamp(-lateral, along, 0.55f));
                approach.redSideRows.add(lamp(lateral, along, 0.55f));
            }
        }

        for (int distance = 60; distance <= 900; distance += 60)
        {
            const float along = threshold + direction * distance;
            for (float lateral : {9.0f, 11.0f, 13.0f})
            {
                _touchdownZone.add(lamp(-lateral, along, 0.23f));
                _touchdownZone.add(lamp(lateral, along, 0.23f));
            }
        }

        // Store the strobes in firing order, starting at the threshold.
        approach.strobes.add(lamp(0, threshold, 0.36f));
        for (int distance = 30; distance <= 900; distance += 30)
        {
            const float along = threshold - direction * distance;
            approach.approachCentreline.add(lamp(0, along, 0.85f));
            if (distance % 60 == 0 || distance == 30)
                approach.strobes.add(lamp(0, along, 1.10f));
            if (distance % 150 == 0)
            {
                const int lampsPerSide = 3 + distance / 150;
                for (int index = 1; index <= lampsPerSide; ++index)
                {
                    const float lateral = 4.0f * index;
                    approach.steadyLights.add(lamp(-lateral, along, 0.85f));
                    approach.steadyLights.add(lamp(lateral, along, 0.85f));
                }
            }
        }
    };

    addApproach(_east, 23.0f, 1.0f, 1.0f, 0.0f, eastDatumMetres);
    const float westThreshold = length - westThresholdDisplacement;
    addApproach(_west, westThreshold, length - 1.0f, -1.0f, westThreshold, westDatumMetres);
}

void RunwayLights::Batch::add(const Vector3F& position)
{
    for (int vertex = 0; vertex < verticesPerLamp; ++vertex)
        centres.insert(centres.end(), {position.x, position.y, position.z});
    corners.insert(corners.end(), std::begin(lampCorners), std::end(lampCorners));
}

int RunwayLights::Batch::lampCount() const
{
    return static_cast<int>(centres.size() / (verticesPerLamp * 3));
}

void RunwayLights::Batch::draw(Renderer& renderer, const Vector3F& colour, float diameter,
                              float alpha, int lamp) const
{
    auto& shader = renderer.progId();
    shader.sendUniform("lampColour", colour);
    shader.sendUniform("lampDiameter", diameter);
    shader.sendUniform("lampAlpha", alpha);
    shader.sendUniform("strobe", lamp < 0 ? 0.0f : 1.0f);
    renderer.bindVertex(Renderer::Vertex, 3, centres.data());
    renderer.bindVertex(Renderer::TexCoord, 2, corners.data());
    renderer.setVertexCountOffset((lamp < 0 ? lampCount() : 1) * verticesPerLamp,
                                  (lamp < 0 ? 0 : lamp) * verticesPerLamp);
    renderer.Render();
    renderer.unBindBuffers();
}

void RunwayLights::render(Renderer& renderer, const GPSLocation& camera, std::int64_t elapsedMs,
                          float daylight) const
{
    const Vector3F cameraLocal = GPSModelTransform(_origin).ToLocal(camera);
    auto& pipeline = OpenGLPipeline::Get(renderer.camID);
    pipeline.GetModel().Push();
    pipeline.GetModel().TranslateLocation(_origin);
    pipeline.bindMatrices(renderer.progId());
    renderer.setUseIndex(false);
    renderer.setPrimitiveType(GL_TRIANGLES);

    const Vector3F white(0.97f, 0.98f, 1.0f);
    const float brightness = 0.98f - 0.32f * std::clamp(daylight, 0.0f, 1.0f);
    _edgeLights.draw(renderer, white, 1.7f, brightness);
    _centreline.draw(renderer, white, 1.1f, brightness * 0.8f);
    _touchdownZone.draw(renderer, white, 1.0f, brightness * 0.75f);

    const auto drawApproach = [&](const Approach& approach)
    {
        const auto fromThreshold = approach.papiThreshold.offSetTo(camera);
        const double forward = approach.papiBeamOffset - (fromThreshold * approach.papiInbound);
        const double side = fromThreshold.x * approach.papiInbound.z - fromThreshold.z * approach.papiInbound.x;
        if (forward > 0.0 && std::abs(side) <= forward * std::tan(DegreesToRadians(10.0)))
        {
            constexpr std::array transitions{-0.5, -1.0 / 6.0, 1.0 / 6.0, 0.5};
            const double elevation = RadiansToDegrees(std::atan2(static_cast<double>(fromThreshold.y), forward));
            for (std::size_t i = 0; i < approach.papi.size(); ++i)
            {
                const bool above = elevation >= _glideSlopeDegrees + transitions[i];
                approach.papi[i].draw(renderer, above ? white : Vector3F(1.0f, 0.035f, 0.015f), 1.9f, brightness);
            }
        }

        if ((cameraLocal - approach.threshold) * approach.inward >= 0.0f)
        {
            approach.stopBar.draw(renderer, Vector3F(1.0f, 0.12f, 0.08f), 1.5f, brightness);
            return;
        }
        approach.thresholdBar.draw(renderer, Vector3F(0.22f, 1.0f, 0.37f), 1.5f, brightness);
        approach.steadyLights.draw(renderer, white, 1.35f, brightness);
        approach.approachCentreline.draw(renderer, white, 1.0f, brightness * 0.16f);
        approach.redSideRows.draw(renderer, Vector3F(1.0f, 0.06f, 0.025f), 1.25f, brightness);

        constexpr int stepMs = 65;
        constexpr int cycleMs = 1900;
        const int phase = static_cast<int>(elapsedMs % cycleMs);
        const int activeLamp = phase / stepMs;
        const float age = (phase % stepMs) / static_cast<float>(stepMs);
        if (activeLamp < approach.strobes.lampCount())
        {
            const float remaining = 1.0f - age;
            approach.strobes.draw(renderer, white, 4.0f, 0.28f + 0.72f * remaining * remaining, activeLamp);
        }
    };
    drawApproach(_east);
    drawApproach(_west);
    pipeline.GetModel().Pop();
}
