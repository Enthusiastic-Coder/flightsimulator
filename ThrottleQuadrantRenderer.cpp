#include "ThrottleQuadrantRenderer.h"
#include "CockpitDisplayFont.h"

#include <QLinearGradient>
#include <QPainter>
#include <algorithm>

namespace
{
constexpr double top = 133;
constexpr double bottom = 374;
const QColor text(212, 225, 228);
const QColor muted(146, 165, 174);
const QColor cyan(95, 215, 231);
const QColor amber(245, 190, 86);

double leverY(float percentage)
{
    return bottom - std::clamp(percentage, 0.0f, 100.0f) * (bottom - top) / 100.0;
}

void drawLabel(QPainter& painter, QRectF bounds, const QString& value, int size, const QColor& colour)
{
    painter.setFont(cockpitDisplayFont(size));
    painter.setPen(colour);
    painter.drawText(bounds, Qt::AlignCenter, value);
}
}

QRectF ThrottleQuadrantRenderer::controlBounds()
{
    return QRectF(13, top - 21, Width - 26, bottom - top + 42);
}

float ThrottleQuadrantRenderer::commandAt(double y)
{
    return static_cast<float>(std::clamp((bottom - y) * 100.0 / (bottom - top), 0.0, 100.0));
}

void ThrottleQuadrantRenderer::render(QPainter& painter, const QRectF& viewport,
                                      const ThrottleQuadrantParameters& parameters)
{
    painter.save();
    painter.setClipRect(viewport);
    painter.translate(viewport.topLeft());
    painter.scale(viewport.width() / Width, viewport.height() / Height);
    painter.setRenderHint(QPainter::Antialiasing);

    QLinearGradient pedestal(0, 0, 0, Height);
    pedestal.setColorAt(0, QColor(50, 58, 65));
    pedestal.setColorAt(1, QColor(24, 29, 33));
    painter.setPen(QPen(QColor(83, 96, 103), 2));
    painter.setBrush(pedestal);
    painter.drawRoundedRect(QRectF(7, 10, Width - 14, Height - 20), 12, 12);
    drawLabel(painter, QRectF(0, 35, Width, 27), QStringLiteral("THRUST"), 23, text);
    drawLabel(painter, QRectF(0, 69, Width, 26),
              parameters.available ? QStringLiteral("CMD %1%").arg(parameters.command) : QStringLiteral("CMD ---"), 20, cyan);

    painter.setPen(QPen(QColor(74, 84, 89), 1.5));
    painter.setBrush(QColor(13, 17, 21));
    painter.drawRoundedRect(QRectF(32, top - 14, 95, bottom - top + 28), 10, 10);
    for (int x : {55, 103})
    {
        painter.setPen(QPen(QColor(3, 6, 8), 13, Qt::SolidLine, Qt::RoundCap));
        painter.drawLine(QPointF(x, top), QPointF(x, bottom));
        painter.setPen(QPen(QColor(63, 73, 79), 1.5));
        painter.drawLine(QPointF(x, top), QPointF(x, bottom));
    }

    for (int percentage : {0, 25, 50, 75, 100})
    {
        const auto y = leverY(percentage);
        painter.setPen(QPen(muted, 1.5));
        painter.drawLine(QPointF(131, y), QPointF(141, y));
        const auto value = percentage == 0 ? QStringLiteral("IDLE") :
            percentage == 100 ? QStringLiteral("MAX") : QString::number(percentage);
        drawLabel(painter, QRectF(144, y - 10, 51, 20), value, 16, muted);
    }

    if (parameters.available)
    {
        const auto y = leverY(parameters.command);
        QLinearGradient grip(0, y - 14, 0, y + 14);
        grip.setColorAt(0, QColor(135, 145, 148));
        grip.setColorAt(0.45, QColor(84, 95, 100));
        grip.setColorAt(1, QColor(35, 43, 48));
        painter.setPen(QPen(QColor(9, 14, 17), 3));
        painter.setBrush(grip);
        for (int x : {55, 103})
        {
            painter.drawRoundedRect(QRectF(x - 22, y - 14, 44, 28), 6, 6);
            painter.setPen(QPen(text, 2.0));
            painter.drawLine(QPointF(x - 10, y - 3), QPointF(x + 10, y - 3));
            painter.setPen(QPen(QColor(9, 14, 17), 3));
        }
    }

    drawLabel(painter, QRectF(0, 423, Width, 27),
              parameters.available ? QStringLiteral("OUT %1%").arg(parameters.output) : QStringLiteral("OUT ---"), 19, amber);
    painter.restore();
}
