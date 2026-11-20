#pragma once

#include <QRectF>

class QPainter;

struct ThrottleQuadrantParameters
{
    bool available = false;
    int command = 0;
    int output = 0;

    bool operator==(const ThrottleQuadrantParameters& other) const
    {
        return available == other.available && command == other.command && output == other.output;
    }
};

class ThrottleQuadrantRenderer
{
public:
    static constexpr int Width = 210;
    static constexpr int Height = 512;

    static QRectF controlBounds();
    static float commandAt(double y);
    static void render(QPainter& painter, const QRectF& viewport, const ThrottleQuadrantParameters& parameters);
};
