#include "NavigationDisplayRenderer.h"

#include <QFont>
#include <QPainter>
#include <QPainterPath>
#include <QPolygonF>
#include <cmath>

namespace
{
constexpr double designSize = 512;
constexpr double radius = 184;
constexpr double rangeNm = 10;
const QPointF centre(256, 270);
const QColor white(229, 239, 241);
const QColor slate(106, 137, 146);
const QColor cyan(100, 213, 237);
const QColor green(112, 233, 147);

QString bearingText(double degrees)
{
    return QStringLiteral("%1").arg(qRound(degrees) % 360, 3, 10, QLatin1Char('0'));
}

void label(QPainter& painter, const QRectF& field, const QString& text, int size, const QColor& colour,
           Qt::Alignment alignment = Qt::AlignCenter)
{
    QFont font(QStringLiteral("Consolas"));
    font.setPixelSize(size);
    painter.setFont(font);
    painter.setPen(colour);
    painter.drawText(field, alignment, text);
}
}

bool NdParameters::operator==(const NdParameters& other) const
{
    return available == other.available && headingDegrees == other.headingDegrees &&
        trackDegrees == other.trackDegrees && groundSpeedKnots == other.groundSpeedKnots &&
        heathrowRunways == other.heathrowRunways;
}

void NavigationDisplayRenderer::render(QPainter& painter, const QRectF& viewport, const NdParameters& parameters)
{
    if (viewport.isEmpty())
        return;

    painter.save();
    painter.setClipRect(viewport);
    painter.fillRect(viewport, QColor(8, 15, 20));
    painter.setRenderHint(QPainter::Antialiasing);
    painter.translate(viewport.topLeft());
    painter.scale(viewport.width() / designSize, viewport.height() / designSize);

    const auto heading = parameters.headingDegrees;
    painter.setPen(QPen(slate, 1.2));
    painter.setBrush(QColor(10, 23, 30));
    painter.drawEllipse(centre, radius, radius);
    painter.setBrush(Qt::NoBrush);
    painter.setPen(QPen(slate, 0.8, Qt::DotLine));
    painter.drawEllipse(centre, radius / 2, radius / 2);

    if (parameters.available)
    {
        painter.save();
        QPainterPath mapClip;
        mapClip.addEllipse(centre, radius - 1, radius - 1);
        painter.setClipPath(mapClip, Qt::IntersectClip);
        painter.translate(centre);
        painter.rotate(-heading);
        const double pixelsPerMetre = radius / (rangeNm * 1852.0);
        painter.setPen(QPen(cyan, 2.5, Qt::SolidLine, Qt::RoundCap));
        const auto& runways = parameters.heathrowRunways;
        if (!runways.empty())
        {
            QPointF airport;
            for (const auto& runway : runways)
            {
                const auto start = runway.p1() * pixelsPerMetre;
                const auto end = runway.p2() * pixelsPerMetre;
                painter.drawLine(start, end);
                airport += (start + end) * 0.5;
            }
            airport /= static_cast<qreal>(runways.size());
            painter.translate(airport);
            painter.rotate(heading);
            label(painter, QRectF(-28, 13, 56, 18), QStringLiteral("EGLL"), 15, cyan);
        }
        painter.restore();

        if (parameters.groundSpeedKnots >= 3)
        {
            painter.save();
            painter.translate(centre);
            painter.rotate(parameters.trackDegrees - heading);
            painter.setPen(QPen(green, 1.5, Qt::DashLine));
            painter.drawLine(QPointF(0, -14), QPointF(0, -radius + 22));
            painter.setPen(QPen(green, 1.5));
            painter.drawPolygon(QPolygonF{{0, -radius + 17}, {5, -radius + 23},
                                          {0, -radius + 29}, {-5, -radius + 23}});
            painter.restore();
        }
    }

    for (int bearing = 0; bearing < 360; bearing += 5)
    {
        const bool largeTick = bearing % 10 == 0;
        const double radians = (bearing - heading) * 3.141592653589793 / 180.0;
        const QPointF direction(std::sin(radians), -std::cos(radians));
        painter.setPen(QPen(largeTick ? white : slate, 1.2));
        painter.drawLine(centre + direction * radius, centre + direction * (radius - (largeTick ? 9 : 4)));
        if (bearing % 30 == 0 && std::abs(std::remainder(bearing - heading, 360.0)) >= 12)
        {
            const auto anchor = centre + direction * (radius + 14);
            label(painter, QRectF(anchor.x() - 16, anchor.y() - 8, 32, 16),
                  QString::number(bearing / 10).rightJustified(2, QLatin1Char('0')), 14, white);
        }
    }

    painter.setPen(QPen(white, 2.5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.drawPolyline(QPolygonF{centre + QPointF(-12, 5), centre, centre + QPointF(12, 5)});
    painter.drawLine(centre + QPointF(0, -11), centre + QPointF(0, 13));
    painter.setBrush(cyan);
    painter.setPen(Qt::NoPen);
    painter.drawPolygon(QPolygonF{{256, 89}, {250, 80}, {262, 80}});

    label(painter, QRectF(208, 14, 96, 18), QStringLiteral("HDG"), 14, slate);
    painter.setBrush(QColor(13, 34, 45));
    painter.setPen(QPen(slate, 1));
    painter.drawRect(QRectF(216, 34, 80, 37));
    label(painter, QRectF(216, 34, 80, 37),
          parameters.available ? bearingText(heading) : QStringLiteral("---"), 28, white);
    label(painter, QRectF(25, 25, 134, 25),
          parameters.available ? QStringLiteral("GS %1").arg(qRound(parameters.groundSpeedKnots)) : QStringLiteral("GS ---"),
          19, cyan, Qt::AlignLeft | Qt::AlignVCenter);
    label(painter, QRectF(352, 25, 134, 25),
          parameters.available && parameters.groundSpeedKnots >= 3
              ? QStringLiteral("TRK %1").arg(bearingText(parameters.trackDegrees)) : QStringLiteral("TRK ---"),
          19, green, Qt::AlignRight | Qt::AlignVCenter);
    label(painter, QRectF(213, 481, 86, 17), QStringLiteral("10 NM"), 16, cyan);
    if (!parameters.available)
        label(painter, QRectF(176, 300, 160, 28), QStringLiteral("NO NAV DATA"), 18, slate);

    painter.restore();
}
