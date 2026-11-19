#pragma once

#include <QLineF>
#include <vector>

class QPainter;
class QRectF;

struct NdParameters
{
    bool available = false;
    double headingDegrees = 0;
    double trackDegrees = 0;
    double groundSpeedKnots = 0;
    std::vector<QLineF> heathrowRunways;

    bool operator==(const NdParameters& other) const;
};

class NavigationDisplayRenderer
{
public:
    static void render(QPainter& painter, const QRectF& viewport, const NdParameters& parameters);
};
