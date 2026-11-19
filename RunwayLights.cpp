#include "stdafx.h"
#include "RunwayLights.h"
#include "MipMapTerrainMeshModel.h"
#include "OpenGLPipeline.h"
#include "OpenGLRenderer.h"
#include "OpenGLShaderProgram.h"

#include <jibbs/gps/GPSModelTransform.h>
#include <algorithm>
#include <iterator>

namespace
{
constexpr float lampCorners[] = {-1, -1, 1, -1, 1, 1, -1, -1, 1, 1, -1, 1};
constexpr int verticesPerLamp = 6;
}

RunwayLights::RunwayLights(const MipMapTerrainMeshModel& terrain, const GPSLocation& eastEnd,
                           float heading, float length, float westThresholdDisplacement)
    : _origin(eastEnd)
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

    const auto addApproach = [&](Approach& approach, float threshold, float stopEnd, float direction)
    {
        approach.threshold = lamp(0, threshold, 0.3f);
        approach.inward = QVRotate(rotation, Vector3F(0, 0, -direction));

        for (int i = 0; i < 18; ++i)
        {
            const float lateral = -25.0f + 50.0f * i / 17.0f;
            approach.thresholdBar.add(lamp(lateral, threshold, 0.30f));
            approach.stopBar.add(lamp(lateral, stopEnd, 0.30f));
        }

        // Follow the extended centreline from the outermost lamp to the threshold.
        for (int distance = 900; distance >= 30; distance -= 30)
        {
            const float along = threshold - direction * distance;
            approach.steadyLights.add(lamp(0, along, 0.85f));
            if (distance % 60 == 0 || distance == 30)
                approach.strobes.add(lamp(0, along, 1.10f));
            if (distance % 150 == 0 && distance <= 750)
            {
                const float span = 12.0f + distance / 40.0f;
                for (float lateral : {-span, -span / 2, span / 2, span})
                    approach.steadyLights.add(lamp(lateral, along, 0.85f));
            }
        }
        approach.strobes.add(lamp(0, threshold, 0.36f));
    };

    addApproach(_east, 23.0f, 1.0f, 1.0f);
    addApproach(_west, length - westThresholdDisplacement, length - 1.0f, -1.0f);
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
    const float brightness = 0.94f - 0.48f * std::clamp(daylight, 0.0f, 1.0f);
    _edgeLights.draw(renderer, white, 0.95f, brightness);
    _centreline.draw(renderer, white, 0.62f, brightness * 0.78f);

    const auto drawApproach = [&](const Approach& approach)
    {
        if ((cameraLocal - approach.threshold) * approach.inward >= 0.0f)
        {
            approach.stopBar.draw(renderer, Vector3F(1.0f, 0.12f, 0.08f), 0.9f, brightness);
            return;
        }
        approach.thresholdBar.draw(renderer, Vector3F(0.22f, 1.0f, 0.37f), 0.9f, brightness);
        approach.steadyLights.draw(renderer, white, 0.8f, brightness);

        constexpr int stepMs = 65;
        constexpr int cycleMs = 1900;
        const int phase = static_cast<int>(elapsedMs % cycleMs);
        const int activeLamp = phase / stepMs;
        if (activeLamp < approach.strobes.lampCount())
        {
            const float age = (phase % stepMs) / static_cast<float>(stepMs);
            approach.strobes.draw(renderer, white, 2.7f, 1.0f - 0.5f * age, activeLamp);
        }
    };
    drawApproach(_east);
    drawApproach(_west);
    pipeline.GetModel().Pop();
}
