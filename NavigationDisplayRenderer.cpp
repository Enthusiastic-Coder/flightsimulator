#include "NavigationDisplayRenderer.h"
#include "CockpitDisplayFont.h"

#include <QPainter>
#include <QPainterPath>
#include <QPolygonF>
#include <QtMath>
#include <cmath>

namespace
{
constexpr double designSize = 512;
constexpr double radius = 369;
constexpr double rangeNm = 10;
constexpr int arcStart = 30;
constexpr int arcSpan = 120;
const QPointF ownship(256, 445);
const QColor white(235, 242, 243);
const QColor dim(111, 132, 142);
const QColor cyan(69, 212, 234);
const QColor green(104, 233, 113);

QString bearingText(double degrees)
{
    return QStringLiteral("%1").arg(qRound(degrees) % 360, 3, 10, QLatin1Char('0'));
}

void label(QPainter& painter, const QRectF& field, const QString& text, int size, const QColor& colour,
           Qt::Alignment alignment = Qt::AlignCenter)
{
    painter.setFont(cockpitDisplayFont(size));
    painter.setPen(colour);
    painter.drawText(field, alignment, text);
}

QRectF ring(double distance)
{
    return QRectF(ownship.x() - distance, ownship.y() - distance, 2 * distance, 2 * distance);
}
}

bool NdParameters::operator==(const NdParameters& other) const
{
    return available == other.available && headingDegrees == other.headingDegrees &&
        trackDegrees == other.trackDegrees && groundSpeedKnots == other.groundSpeedKnots &&
        trueAirspeedKnots == other.trueAirspeedKnots && heathrowRunways == other.heathrowRunways;
}

void NavigationDisplayRenderer::render(QPainter& painter, const QRectF& viewport, const NdParameters& parameters)
{
    if (viewport.isEmpty())
        return;

    painter.save();
    painter.setClipRect(viewport);
    painter.fillRect(viewport, QColor(4, 9, 12));
    painter.setRenderHint(QPainter::Antialiasing);
    painter.translate(viewport.topLeft());
    painter.scale(viewport.width() / designSize, viewport.height() / designSize);

    const double heading = parameters.headingDegrees;
    QPainterPath map;
    map.moveTo(ownship);
    map.arcTo(ring(radius), arcStart, arcSpan);
    map.closeSubpath();
    painter.fillPath(map, QColor(8, 16, 21));
    painter.setPen(QPen(dim, 1, Qt::DotLine));
    for (double fraction : {0.25, 0.5, 0.75})
        painter.drawArc(ring(radius * fraction), arcStart * 16, arcSpan * 16);

    if (parameters.available)
    {
        painter.save();
        painter.setClipPath(map, Qt::IntersectClip);
        painter.translate(ownship);
        painter.rotate(-heading);
        const double pixelsPerMetre = radius / (rangeNm * 1852.0);
        const auto& runways = parameters.heathrowRunways;
        if (!runways.empty())
        {
            QPointF airport;
            for (const auto& runway : runways)
            {
                const auto start = runway.p1() * pixelsPerMetre;
                const auto end = runway.p2() * pixelsPerMetre;
                painter.setPen(QPen(cyan, 3.1, Qt::SolidLine, Qt::FlatCap));
                painter.drawLine(start, end);
                painter.setPen(QPen(QColor(7, 18, 23), 1, Qt::DashLine));
                painter.drawLine(start, end);
                airport += (start + end) * 0.5;
            }
            airport /= static_cast<qreal>(runways.size());
            painter.translate(airport);
            painter.rotate(heading);
            label(painter, QRectF(8, 5, 72, 22), QStringLiteral("EGLL"), 17, cyan,
                  Qt::AlignLeft | Qt::AlignVCenter);
        }
        painter.restore();

        if (parameters.groundSpeedKnots >= 3)
        {
            painter.save();
            painter.setClipPath(map, Qt::IntersectClip);
            painter.translate(ownship);
            painter.rotate(parameters.trackDegrees - heading);
            painter.setPen(QPen(green, 1.5, Qt::DashLine));
            painter.drawLine(QPointF(0, -16), QPointF(0, -radius + 18));
            painter.setPen(QPen(green, 1.7));
            painter.setBrush(Qt::NoBrush);
            painter.drawPolygon(QPolygonF{{0, -radius + 12}, {6, -radius + 19},
                                          {0, -radius + 26}, {-6, -radius + 19}});
            painter.restore();
        }
    }

    painter.setPen(QPen(dim, 1.5));
    painter.drawArc(ring(radius), arcStart * 16, arcSpan * 16);
    for (int bearing = 0; bearing < 360; bearing += 5)
    {
        const double relative = std::remainder(bearing - heading, 360.0);
        if (std::abs(relative) > arcSpan / 2)
            continue;
        const bool largeTick = bearing % 10 == 0;
        const double radians = qDegreesToRadians(relative);
        const QPointF direction(std::sin(radians), -std::cos(radians));
        painter.setPen(QPen(largeTick ? white : dim, largeTick ? 1.4 : 1.0));
        painter.drawLine(ownship + direction * radius, ownship + direction * (radius - (largeTick ? 10 : 5)));
        if (largeTick && std::abs(relative) >= 8)
        {
            const auto anchor = ownship + direction * (radius + 16);
            label(painter, QRectF(anchor.x() - 20, anchor.y() - 9, 40, 18),
                  QString::number(bearing / 10).rightJustified(2, QLatin1Char('0')), 17, white);
        }
    }

    painter.setPen(QPen(white, 2.4, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.drawPolyline(QPolygonF{ownship + QPointF(-14, 5), ownship, ownship + QPointF(14, 5)});
    painter.drawLine(ownship + QPointF(0, -12), ownship + QPointF(0, 13));
    painter.setBrush(white);
    painter.setPen(Qt::NoPen);
    painter.drawPolygon(QPolygonF{{256, 76}, {251, 66}, {261, 66}});

    label(painter, QRectF(211, 4, 90, 17), QStringLiteral("TRUE"), 13, dim);
    painter.setBrush(QColor(13, 22, 27));
    painter.setPen(QPen(white, 1.2));
    painter.drawRect(QRectF(216, 23, 80, 37));
    label(painter, QRectF(216, 23, 80, 37),
          parameters.available ? bearingText(heading) : QStringLiteral("---"), 26, white);
    label(painter, QRectF(18, 11, 170, 22),
          parameters.available ? QStringLiteral("GS  %1").arg(qRound(parameters.groundSpeedKnots)) : QStringLiteral("GS  ---"),
          18, white, Qt::AlignLeft | Qt::AlignVCenter);
    label(painter, QRectF(18, 35, 170, 20),
          parameters.available ? QStringLiteral("TAS %1").arg(qRound(parameters.trueAirspeedKnots)) : QStringLiteral("TAS ---"),
          16, white, Qt::AlignLeft | Qt::AlignVCenter);
    label(painter, QRectF(345, 11, 149, 24),
          parameters.available && parameters.groundSpeedKnots >= 3
              ? QStringLiteral("TRK %1").arg(bearingText(parameters.trackDegrees)) : QStringLiteral("TRK ---"),
          17, green, Qt::AlignRight | Qt::AlignVCenter);
    label(painter, QRectF(18, 469, 104, 24), QStringLiteral("ARC"), 17, white, Qt::AlignLeft | Qt::AlignVCenter);
    label(painter, QRectF(390, 469, 104, 24), QStringLiteral("10 NM"), 17, cyan, Qt::AlignRight | Qt::AlignVCenter);
    label(painter, QRectF(270, ownship.y() - radius / 2 - 10, 23, 20), QStringLiteral("5"), 14, dim);
    if (!parameters.available)
        label(painter, QRectF(166, 302, 180, 29), QStringLiteral("NO NAV DATA"), 18, QColor(243, 188, 79));

    painter.restore();
}
