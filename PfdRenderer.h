#pragma once

#include "IlsGuidance.h"

class QPainter;
class QRect;
class QRectF;

struct PfdParameters
{
    double bankDegrees = 0.0;
    double pitchDegrees = 0.0;
    double airspeedKnots = 0.0;
    double altitudeFeet = 0.0;
    double headingDegrees = 0.0;
    double verticalSpeedFeetPerMinute = 0.0;
    double accelerationKnotsPerSecond = 0.0;
    IlsGuidance ils;

    bool operator==(const PfdParameters &other) const noexcept;
};

class PfdRenderer
{
public:
    static constexpr int DesignSize = 512;

    // Draw into an active painter. The viewport is in painter coordinates.
    // Supply finite telemetry in the units above and heading in [0, 360).
    static void render(QPainter &painter, const QRectF &viewport, const PfdParameters &parameters);

private:
    static constexpr int horizCenterX = 220;
    static constexpr int horizCenterY = 260;
    static constexpr int horiz_radius = 136;

    static void drawBankAngleMarkers(QPainter &painter);
    static void drawInnerBankAngleMarkers(QPainter &painter, QRect boundingRect);
    static void drawAttitudeIndicator(QPainter &painter, const PfdParameters &parameters);
    static void drawAirspeedTape(QPainter &painter, const PfdParameters &parameters);
    static void drawAltimeterTape(QPainter &painter, const PfdParameters &parameters);
    static void drawScrollingAltimeter(QPainter &painter, const PfdParameters &parameters);
    static void drawVSI(QPainter &painter, const PfdParameters &parameters);
    static void drawHeading(QPainter &painter, const PfdParameters &parameters);
    static void drawFixedWings(QPainter &painter);
    static void drawIlsBars(QPainter &painter, const IlsGuidance &guidance);
    static void drawIls(QPainter &painter, const IlsGuidance &guidance);
    static void drawModesSeparatorLines(QPainter &painter);
};
