#pragma once

#include <array>
#include <optional>
#include <QRectF>

class QPainter;

struct EngineDisplayReading
{
    int n1Tenths = 0;
    int n2Tenths = 0;
    std::optional<int> commandedN1Tenths;
    bool operator==(const EngineDisplayReading& other) const
    {
        return n1Tenths == other.n1Tenths && n2Tenths == other.n2Tenths &&
            commandedN1Tenths == other.commandedN1Tenths;
    }
};

using EngineDisplayParameters = std::optional<std::array<EngineDisplayReading, 2>>;

class EngineDisplayRenderer
{
public:
    static void render(QPainter& painter, const QRectF& viewport, const EngineDisplayParameters& engines);
};
