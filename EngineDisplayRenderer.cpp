#include "EngineDisplayRenderer.h"
#include "CockpitDisplayFont.h"

#include <QPainter>
#include <QtMath>
#include <algorithm>
#include <cmath>

namespace
{
constexpr double designSize = 512;
constexpr double startAngle = 210;
constexpr double sweep = 225;
constexpr double scaleMaximum = 105;
// CFM56-5B LP rotor limit, EASA TCDS E.003 section IV.2.1.
constexpr double n1Redline = 104;
const QColor white(235, 242, 243);
const QColor green(104, 233, 113);
const QColor cyan(69, 212, 234);
const QColor amber(255, 190, 77);
const QColor red(255, 75, 70);
const QColor dim(75, 94, 101);

double dialAngle(double percent)
{
    return startAngle - sweep * std::clamp(percent, 0.0, scaleMaximum) / scaleMaximum;
}

QPointF dialPoint(const QPointF& center, double radius, double percent)
{
    const double radians = qDegreesToRadians(dialAngle(percent));
    return center + QPointF(radius * std::cos(radians), -radius * std::sin(radians));
}

void label(QPainter& painter, const QRectF& field, const QString& text, int size, const QColor& colour)
{
    painter.setFont(cockpitDisplayFont(size));
    painter.setPen(colour);
    painter.drawText(field, Qt::AlignCenter, text);
}

void digitalValue(QPainter& painter, const QRectF& field, std::optional<int> tenths)
{
    painter.setPen(QPen(dim, 1.5));
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(field);
    label(painter, field, tenths ? QString::number(*tenths / 10.0, 'f', 1) : QStringLiteral("XX"),
          35, tenths ? green : amber);
}

void drawN1(QPainter& painter, const QPointF& center, const EngineDisplayReading* engine)
{
    constexpr double radius = 90;
    const QRectF arc(center.x() - radius, center.y() - radius, 2 * radius, 2 * radius);
    painter.setPen(QPen(white, 2));
    painter.drawArc(arc, qRound(startAngle * 16), qRound(-sweep * 16));
    for (int tick = 0; tick <= 100; tick += 10)
    {
        const bool major = tick % 20 == 0;
        painter.setPen(QPen(white, major ? 2 : 1));
        painter.drawLine(dialPoint(center, radius, tick), dialPoint(center, major ? 78 : 84, tick));
        if (major && tick != 0)
        {
            const auto position = dialPoint(center, 64, tick);
            label(painter, QRectF(position.x() - 18, position.y() - 10, 36, 20),
                  QString::number(tick), 15, white);
        }
    }
    painter.setPen(QPen(red, 3));
    painter.drawLine(dialPoint(center, radius - 9, n1Redline),
                     dialPoint(center, radius + 2, n1Redline));

    if (engine)
    {
        painter.setPen(QPen(green, 3));
        painter.drawLine(center, dialPoint(center, 87, engine->n1Tenths / 10.0));
        if (engine->commandedN1Tenths)
        {
            const double command = *engine->commandedN1Tenths / 10.0;
            painter.setPen(QPen(cyan, 3));
            painter.drawLine(dialPoint(center, 94, command), dialPoint(center, 104, command));
        }
    }

    digitalValue(painter, QRectF(center.x() - 56, center.y() + 23, 112, 49),
                 engine ? std::optional(engine->n1Tenths) : std::nullopt);
}
}

void EngineDisplayRenderer::render(QPainter& painter, const QRectF& viewport,
                                   const EngineDisplayParameters& engines)
{
    painter.save();
    painter.setClipRect(viewport, Qt::IntersectClip);
    painter.fillRect(viewport, QColor(5, 10, 13));
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::TextAntialiasing);
    painter.translate(viewport.topLeft());
    painter.scale(viewport.width() / designSize, viewport.height() / designSize);
    painter.translate(0, 50);

    label(painter, QRectF(0, 20, 512, 36), QStringLiteral("ENG"), 23, white);
    label(painter, QRectF(224, 130, 64, 30), QStringLiteral("N1"), 23, white);
    label(painter, QRectF(224, 160, 64, 24), QStringLiteral("%"), 18, cyan);
    label(painter, QRectF(224, 309, 64, 29), QStringLiteral("N2"), 23, white);
    label(painter, QRectF(224, 338, 64, 24), QStringLiteral("%"), 18, cyan);

    for (int index = 0; index < 2; ++index)
    {
        const int x = index == 0 ? 124 : 388;
        const auto* engine = engines ? &(*engines)[index] : nullptr;
        label(painter, QRectF(x - 25, 50, 50, 29), QString::number(index + 1), 19, white);
        drawN1(painter, QPointF(x, 185), engine);
        digitalValue(painter, QRectF(x - 56, 309, 112, 49),
                     engine ? std::optional(engine->n2Tenths) : std::nullopt);
    }

    painter.restore();
}
