#include "PfdRenderer.h"

#include <QPainter>
#include <QPainterPath>
#include <QtMath>

#include <algorithm>
#include <array>
#include <cmath>

namespace {
constexpr QColor darkGrayColor{118, 118, 118};

double pitchLadderOffset(double degrees, double radius)
{
    // Preserve the original PFD's scale changes at -10° and +20°.
    // The original OpenGL horizon used a radius of 100 design units.
    const double pixelsPerDegree = 3.2 * radius / 100.0;
    if (degrees < -10.0)
        return pixelsPerDegree * (-10.0 + (degrees + 10.0) * 0.5);
    if (degrees > 20.0)
        return pixelsPerDegree * (20.0 + (degrees - 20.0) * 0.8);
    return pixelsPerDegree * degrees;
}
}

bool PfdParameters::operator==(const PfdParameters &other) const noexcept
{
    return bankDegrees == other.bankDegrees
        && pitchDegrees == other.pitchDegrees
        && airspeedKnots == other.airspeedKnots
        && altitudeFeet == other.altitudeFeet
        && headingDegrees == other.headingDegrees
        && verticalSpeedFeetPerMinute == other.verticalSpeedFeetPerMinute
        && accelerationKnotsPerSecond == other.accelerationKnotsPerSecond;
}

void PfdRenderer::render(QPainter &painter, const QRectF &viewport,
                         const PfdParameters &parameters)
{
    if (viewport.isEmpty())
        return;

    painter.save();
    painter.setClipRect(viewport, Qt::IntersectClip);
    painter.fillRect(viewport, Qt::black);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::black);
    painter.setBrush(Qt::NoBrush);
    painter.setBackgroundMode(Qt::TransparentMode);
    painter.translate(viewport.topLeft());
    painter.scale(viewport.width() / DesignSize, viewport.height() / DesignSize);

    drawAttitudeIndicator(painter, parameters);
    drawFixedWings(painter);
    drawAirspeedTape(painter, parameters);
    drawAltimeterTape(painter, parameters);
    drawScrollingAltimeter(painter, parameters);
    drawVSI(painter, parameters);
    drawVerticalRight(painter);
    drawHorizontalBottom(painter);
    drawHeading(painter, parameters);
    drawModesSeparatorLines(painter);

    painter.restore();
}

void PfdRenderer::drawInnerBankAngleMarkers(QPainter &painter, QRect boundingRect)
{
    QPen pen(Qt::yellow);
    pen.setWidth(2);

    painter.setPen(pen);

    // Calculate positions for markers and text
    const int centerX = boundingRect.center().x();
    const int centerY = boundingRect.center().y();
    const int radius = boundingRect.width() / 2;

    {
        QPolygon poly;
        poly << QPoint(centerX, centerY-radius+2);
        poly << QPoint(centerX+10, centerY-radius+2+14);
        poly << QPoint(centerX-10, centerY-radius+2+14);

        painter.drawPolygon(poly);
    }

    {
        int offSet = 0;
        QPolygon poly;
        poly << QPoint(centerX-12+offSet, centerY-radius+2+18);
        poly << QPoint(centerX+12+offSet, centerY-radius+2+18);
        poly << QPoint(centerX+17+offSet, centerY-radius+2+26);
        poly << QPoint(centerX-17+offSet, centerY-radius+2+26);

        painter.drawPolygon(poly);
    }
}

void PfdRenderer::drawBankAngleMarkers(QPainter &painter)
{
    static const QRect bounds(-horiz_radius, -horiz_radius, horiz_radius * 2, horiz_radius * 2);
    static const auto markers = [] {
        constexpr std::array angles{-64, -62, -45, -30, -20, -10, 10, 20, 30, 45, 62, 64};
        const QPoint center = bounds.center();
        const auto point = [center](int radius, int angle) {
            const double radians = qDegreesToRadians(static_cast<double>(angle));
            return QPoint(center.x() - radius * qSin(radians),
                          center.y() - radius * qCos(radians));
        };

        std::array<QPolygon, angles.size() + 1> polygons;
        for (size_t i = 0; i < angles.size(); ++i)
        {
            const int angle = angles[i];
            const int magnitude = std::abs(angle);
            auto &polygon = polygons[i];

            if (magnitude > 30)
            {
                const int inner = magnitude > 60 ? -7 : 0;
                const int outer = magnitude > 60 ? 3 : 12;
                polygon << point(horiz_radius + inner, angle)
                        << point(horiz_radius + outer, angle);
            }
            else
            {
                const int outer = horiz_radius + (magnitude == 30 ? 12 : 6);
                polygon << point(horiz_radius, angle - 1)
                        << point(outer, angle - 1)
                        << point(outer, angle + 1)
                        << point(horiz_radius, angle + 1);
            }
        }

        polygons.back() << point(horiz_radius, 0)
                        << point(horiz_radius + 12, 3)
                        << point(horiz_radius + 12, -3);
        return polygons;
    }();

    painter.setPen(QPen(Qt::white, 2));
    painter.drawArc(bounds, 60 * 16, 60 * 16);
    for (size_t i = 0; i + 1 < markers.size(); ++i)
        painter.drawPolygon(markers[i]);

    painter.setPen(QPen(Qt::yellow, 2));
    painter.drawPolygon(markers.back());
}

void PfdRenderer::drawAttitudeIndicator(QPainter &painter, const PfdParameters &parameters)
{
    const double pitch = parameters.pitchDegrees;
    const double roll = parameters.bankDegrees;

    painter.save();
    painter.translate(horizCenterX, horizCenterY);

    static const QPainterPath path = [] {
        QPainterPath circle;
        circle.addEllipse(-horiz_radius, -horiz_radius, horiz_radius * 2, horiz_radius * 2);
        return circle;
    }();

    painter.setClipPath(path, Qt::IntersectClip);

    painter.rotate(-roll);

    const double pitchOffset = pitchLadderOffset(pitch, horiz_radius);
    painter.translate(0, pitchOffset);

    const double fieldHeight = horiz_radius + std::abs(pitchOffset);
    painter.setBrush(QBrush(QColor(66, 177, 242))); // Sky blue color
    painter.drawRect(QRectF(-horiz_radius, -fieldHeight, horiz_radius * 2, fieldHeight));

    painter.setBrush(QBrush(QColor(110, 62, 56))); // Brown color
    painter.drawRect(QRectF(-horiz_radius, 0, horiz_radius * 2, fieldHeight));

    painter.setPen(QPen(Qt::white, 3));
    painter.drawLine(-horiz_radius, 0, horiz_radius, 0);

    painter.setPen(QPen(Qt::white, 2));

    const QFontMetrics fm = painter.fontMetrics();
    const int labelMargin = std::max(fm.ascent(), fm.descent()) + 6;

    constexpr std::array marksInTenthsOfDegree{
        -900, -700, -500, -300, -200, -150, -100, -75, -50, -25,
        25, 50, 75, 100, 125, 150, 175, 200, 225, 250, 275, 300, 500, 700, 900
    };
    for (int tenths : marksInTenthsOfDegree)
    {
        const int magnitude = std::abs(tenths);
        const double y = -pitchLadderOffset(tenths / 10.0, horiz_radius);
        if (std::abs(y + pitchOffset) > horiz_radius + labelMargin)
            continue;

        if (tenths % 100 == 0)
        {
            const QString txt = QString::number(magnitude / 10);
            const int adv = fm.horizontalAdvance(txt);

            painter.drawLine(QPointF(-30, y), QPointF(30, y));
            painter.drawText(QPointF(-46 - adv, y + 5), txt);
            painter.drawText(QPointF(46, y + 5), txt);
        }
        else
        {
            const int halfWidth = tenths % 50 == 0 ? 15 : 8;
            painter.drawLine(QPointF(-halfWidth, y), QPointF(halfWidth, y));
        }
    }

    painter.translate(0, -pitchOffset);

    painter.setBrush(QBrush(QColor(110, 62, 56))); // Brown color

    double yPixel = parameters.altitudeFeet > 100 ? 100 : parameters.altitudeFeet;
    yPixel = std::max(pitchOffset,yPixel);
    int yHeight = parameters.altitudeFeet > 100 ? 50 : 150-parameters.altitudeFeet;

    painter.drawRect(-200, yPixel, 400, yHeight);

    painter.setPen(QPen(Qt::yellow, 2));

    painter.setBrush(QBrush(QColor(66, 177, 242))); // Sky blue color
    painter.drawRect(-200, -200, 400, 100);

    drawInnerBankAngleMarkers(painter,QRect(-horiz_radius, -horiz_radius, horiz_radius*2, horiz_radius*2) );

    if(parameters.altitudeFeet < 2500)
    {

        int altRounded = static_cast<int>(parameters.altitudeFeet);
        altRounded -= altRounded%10;

        static const QFont font("Verdana", 18);
        painter.setFont(font);
        QString altStr = QString::number(altRounded);
        int widthOfText = painter.fontMetrics().horizontalAdvance(altStr);

        painter.drawText(-widthOfText/2, horiz_radius-10, altStr);
    }

    painter.restore();

    painter.fillRect(-horiz_radius+horizCenterX, -66 + horizCenterY, 18, 122+20, Qt::black);
    painter.fillRect(horiz_radius-18+horizCenterX, -66 + horizCenterY, 18, 122+20, Qt::black);

    painter.save();
    painter.translate(horizCenterX, horizCenterY);

    drawBankAngleMarkers(painter);

    painter.restore();


}

void PfdRenderer::drawFixedWings(QPainter &painter)
{
    painter.save();

    painter.translate(horizCenterX, horizCenterY);

    QPen yellowPen{Qt::yellow};

    yellowPen.setWidth(1);

    painter.setPen(yellowPen);  // White color and 3-pixel wide lines

    painter.setBrush(Qt::black);
    painter.drawRect(-5, -5, 10, 10);

    const int h = 4;

    QPolygon polygon;
    polygon << QPoint( -62,  -h)
            << QPoint( -111, -h)
            << QPoint( -111 ,  +h)
            << QPoint( -71,  +h)
            << QPoint( -71,  +20)
            << QPoint( -62,  +20)
            << QPoint( -62,  -h);

    painter.drawPolygon(polygon);

    painter.scale(-1,1);
    painter.drawPolygon(polygon);

    painter.restore();
}

void PfdRenderer::drawVerticalRight(QPainter &painter)
{
    painter.save();

    painter.translate(horizCenterX, horizCenterY);

    QPen yellowPen{Qt::yellow};

    yellowPen.setWidth(5);

    painter.setPen(yellowPen);
    painter.drawLine(horiz_radius-10, 0, horiz_radius+10,0);

    painter.setPen(Qt::white);
    painter.drawEllipse(QRect{horiz_radius-10, -45-3, 6, 6});
    painter.drawEllipse(QRect{horiz_radius-10, -95-3, 6, 6});

    painter.drawEllipse(QRect{horiz_radius-10, 45-3, 6, 6});
    painter.drawEllipse(QRect{horiz_radius-10, 95-3, 6, 6});

    painter.restore();
}

void PfdRenderer::drawHorizontalBottom(QPainter &painter)
{
    painter.save();

    painter.translate(horizCenterX, horizCenterY);

    QPen yellowPen{Qt::yellow};

    yellowPen.setWidth(5);

    painter.setPen(yellowPen);
    painter.drawLine(0, horiz_radius+15,0, horiz_radius+35);

    painter.setPen(Qt::white);
    painter.drawEllipse(QRect{-45-3, horiz_radius+20, 6, 6});
    painter.drawEllipse(QRect{-95-3, horiz_radius+20, 6, 6});

    painter.drawEllipse(QRect{45-3, horiz_radius+20, 6, 6});
    painter.drawEllipse(QRect{95-3, horiz_radius+20, 6, 6});

    painter.restore();
}

void PfdRenderer::drawModesSeparatorLines(QPainter &painter)
{
    painter.save();

    painter.setPen(darkGrayColor);
    painter.drawLine(105, 2, 105, 70);
    painter.drawLine(215, 2, 215, 70);
    painter.drawLine(330, 2, 330, 70);
    painter.drawLine(430, 2, 430, 70);

    painter.restore();
}

void PfdRenderer::drawAirspeedTape(QPainter &painter, const PfdParameters &parameters)
{
    const int width = 55;
    const int height = horiz_radius*2;

    const float pixelsPerKnot = height/80;

    painter.save();

    painter.translate(5,horizCenterY);

    painter.setPen(QColor(118, 118, 118));
    painter.setBrush(QBrush(QColor(118, 118, 118))); // Dark background color
    painter.drawRect(0, -horiz_radius, width, horiz_radius*2);

    painter.setPen(Qt::white);
    painter.drawLine(0,-horiz_radius, width+15, -horiz_radius);
    painter.drawLine(0,horiz_radius, width+15, horiz_radius);
    painter.drawLine(width,-horiz_radius, width, horiz_radius);


    {
        QPen yellowPen(Qt::yellow);
        yellowPen.setWidth(3);
        painter.setPen(yellowPen);
        painter.drawLine(-5,0, 0, 0);

        painter.setPen(Qt::black);
        painter.setBrush(Qt::yellow);

        QPolygon polygon;
        polygon << QPoint( width-15,  -2)
                << QPoint( width+5, -2)
                << QPoint( width+15, -7)

                << QPoint( width+15, 7)
                << QPoint( width+5, 2)
                << QPoint( width-15,  2);

        painter.drawPolygon(polygon);
    }



    painter.setPen(Qt::white);

    painter.setClipRect(0, -horiz_radius, width + 15, horiz_radius * 2, Qt::IntersectClip);

    int spd = std::max(30.0,parameters.airspeedKnots) - height / pixelsPerKnot;

    const int diffY = ((int)(spd)%20);
    spd -= diffY;

    static const QFont f("Verdana", 14);
    painter.setFont(f);
    const auto metrics = painter.fontMetrics();
    const int labelMargin = std::max(metrics.ascent(), metrics.descent()) + 6;

    for(int y=height; y >=-height; y -= 10*pixelsPerKnot)
    {
        const int yLevel = y + diffY * pixelsPerKnot;
        if (spd >= 35 && std::abs(yLevel) <= horiz_radius + labelMargin)
        {
            if(spd%20==0)
            {
                QString  spdStr = QString("%1").arg(spd,3,10, QChar('0'));
                painter.drawText(0, yLevel + 5, spdStr);
                painter.drawLine(width - 10, yLevel, width, yLevel);
            }
            else
            {
                painter.drawLine(width - 5, yLevel, width, yLevel);
            }
        }

        spd+=10;
    }

    if(std::abs(parameters.accelerationKnotsPerSecond) >0.1 && parameters.airspeedKnots > 30)
    {
        QPen yellowPen(Qt::yellow);
        yellowPen.setWidth(2);
        painter.setPen(yellowPen);

        const int offset = parameters.accelerationKnotsPerSecond> 0? -2 : 2;
        const int accOffset = parameters.accelerationKnotsPerSecond*pixelsPerKnot*10;

        painter.drawLine(width-10, offset, width - 10, offset - accOffset);
        painter.drawLine(width-10, offset - accOffset, width-14, -offset- accOffset);
        painter.drawLine(width-10, offset - accOffset, width-6, -offset- accOffset);
    }

    painter.restore();

}

void PfdRenderer::drawAltimeterTape(QPainter &painter, const PfdParameters &parameters)
{
    painter.save();

    painter.translate(370,horizCenterY);

    painter.setPen(Qt::black);

    const int width = 60;
    const int height = horiz_radius*2;

    const float pixelsPerAlt = height/1000.0f;

    painter.setPen(Qt::black);
    painter.setBrush(QBrush(darkGrayColor)); // Dark background color
    painter.drawRect(5, -horiz_radius, width-10, horiz_radius*2);
    painter.setPen(Qt::white);
    painter.drawLine(5,-horiz_radius, width+15, -horiz_radius);
    painter.drawLine(5,horiz_radius, width+15, horiz_radius);
    painter.drawLine(width-5, -horiz_radius, width-5, horiz_radius);

    painter.setClipRect(0, -horiz_radius, width + 15, horiz_radius * 2, Qt::IntersectClip);


    int alt = parameters.altitudeFeet - height / pixelsPerAlt;

    const int diffY = ((int)(alt)%100);
    alt -= diffY;

    static const QFont f("Verdana", 14);
    painter.setFont(f);
    const auto metrics = painter.fontMetrics();
    const int labelMargin = std::max(metrics.ascent(), metrics.descent()) + 6;

    for(int y=height; y >=-height; y -= 100*pixelsPerAlt)
    {
        const int yLevel = y + diffY * pixelsPerAlt;
        if (alt >= 0 && std::abs(yLevel) <= horiz_radius + labelMargin)
        {
            if(alt%500==0)
            {
                QString  altStr = QString("%1").arg(alt/100,3,10, QChar('0'));
                painter.drawText(5, yLevel+5, altStr);
                painter.drawLine(width-15, yLevel, width-5, yLevel);

                painter.drawLine(0, yLevel-4, 4, yLevel);
                painter.drawLine(0, yLevel+4, 4, yLevel);
            }
            else
            {
                painter.drawLine(width - 10, yLevel, width - 5, yLevel);
            }
        }

        alt+=100;
    }

    painter.restore();
}

void PfdRenderer::drawScrollingAltimeter(QPainter &painter, const PfdParameters &parameters)
{
    painter.save();

    painter.translate(370,horizCenterY);

    QPolygon poly;
    poly << QPoint(5,-15);
    poly << QPoint(55,-15);
    poly << QPoint(55,-20);
    poly << QPoint(85,-20);
    poly << QPoint(85, 20);
    poly << QPoint(55, 20);
    poly << QPoint(55, 15);
    poly << QPoint(5, 15);

    painter.setBrush(Qt::black);
    painter.setPen(Qt::yellow);
    painter.drawPolygon(poly);

    painter.setPen(Qt::black);
    painter.drawLine(5, -15, 5, 15);

    {
        QPainterPath path;
        path.addPolygon(poly);

        painter.setClipPath(path, Qt::IntersectClip);
    }

    painter.setPen(Qt::green);

    int hundredAlt = static_cast<int>(parameters.altitudeFeet);

    hundredAlt = hundredAlt - hundredAlt/100 * 100;

    {
        static const QFont f("Verdana", 18);
        painter.setFont(f);

        const int height = painter.fontMetrics().height();

        const int width = painter.fontMetrics().horizontalAdvance(QChar('0'));

        QString strAlt = QString("%1").arg(int(parameters.altitudeFeet)/100, 3, 10, QChar(' '));

        bool doScroll{false};
        float perc = 0.0f;

        if( hundredAlt>=80)
        {
            perc = (hundredAlt-80)/20.0f;
            doScroll = true;
        }

        for(int i=2; i >= 0; --i)
        {
            int yPos = 8;

            const QChar currentDigit = strAlt[i];

            if( doScroll )
            {
                char digit = currentDigit == ' ' ? 0 : currentDigit.toLatin1() - '0';
                doScroll &= digit==9;

                yPos = height*perc+8;
                digit++;
                if( digit > 9)
                    digit = 0;

                painter.drawText(8+i*width, yPos-height, QChar(digit+'0'));
            }

            painter.drawText(8+i*width, yPos, currentDigit);
        }
    }

    {
        static const QFont f("Verdana", 8);
        painter.setFont(f);

        const int height = painter.fontMetrics().height();

        const int diffAlt = int(parameters.altitudeFeet) %20;

        const int heightOffset = diffAlt/20.0 * height;

        const int xOffset = 62;

        const int hundredAltNearest20 = hundredAlt - hundredAlt%20;

        for(int line=-2; line <= 2; line++)
        {
            const int finalyOffset = 4+height*line + heightOffset;

            int finalAlt = hundredAltNearest20 - 20*line;
            if( finalAlt < 0)
                finalAlt +=100;
            if( finalAlt >=100)
                finalAlt -=100;

            painter.drawText(xOffset, finalyOffset, QString("%1").arg(finalAlt, 2, 10, QChar('0')));
        }
    }

    painter.restore();
}

void PfdRenderer::drawVSI(QPainter &painter, const PfdParameters &parameters)
{
    painter.save();

    painter.translate(475,260);

    QPolygon poly;
    poly << QPoint(0, 160);
    poly << QPoint(0, -160);
    poly << QPoint(15, -160);
    poly << QPoint(30,-110);
    poly << QPoint(30,110);
    poly << QPoint(15, 160);

    painter.save();
    QPainterPath path;
    path.addPolygon(poly);
    painter.setClipPath(path, Qt::IntersectClip);

    painter.setBrush(darkGrayColor);
    painter.drawPolygon(poly);

    QPen yellowPen(Qt::yellow);
    yellowPen.setWidth(5);
    painter.setPen(yellowPen);
    painter.drawLine(0, 0, 15, 0);

    QPen whitePen(Qt::white);
    whitePen.setWidth(2);
    painter.setPen(whitePen);

    constexpr std::array pixPerVals{85 / 1000.0f, 35 / 1000.0f, 30 / 4000.0f};

    painter.drawLine(10, -1000*pixPerVals[0]-1000*pixPerVals[1] - 2000*pixPerVals[2], 15, -1000*pixPerVals[0]-1000*pixPerVals[1]- 2000*pixPerVals[2]);
    painter.drawLine(10, -1000*pixPerVals[0]-500*pixPerVals[1], 15, -1000*pixPerVals[0]-500*pixPerVals[1]);
    painter.drawLine(10, -500*pixPerVals[0], 15, -500*pixPerVals[0]);

    painter.drawLine(10, 500*pixPerVals[0], 15, 500*pixPerVals[0]);
    painter.drawLine(10, 1000*pixPerVals[0] + 500*pixPerVals[1], 15, 1000*pixPerVals[0] +500*pixPerVals[1]);
    painter.drawLine(10, 1000*pixPerVals[0]+ 1000*pixPerVals[1] + 2000*pixPerVals[2], 15, 1000*pixPerVals[0]+1000*pixPerVals[1]+ 2000*pixPerVals[2]);


    whitePen.setWidth(4);
    painter.setPen(whitePen);

    float hDiv = painter.fontMetrics().height()/4.0f;

    painter.drawLine(10, -1000*pixPerVals[0]-1000*pixPerVals[1]-4000*pixPerVals[2], 15, -1000*pixPerVals[0]-1000*pixPerVals[1] -4000*pixPerVals[2]);
    painter.drawText(0, -1000*pixPerVals[0]-1000*pixPerVals[1]-4000*pixPerVals[2] + hDiv, "6");

    painter.drawLine(10, -1000*pixPerVals[0]-1000*pixPerVals[1], 15, -1000*pixPerVals[0]-1000*pixPerVals[1] );
    painter.drawText(0, -1000*pixPerVals[0]-1000*pixPerVals[1] + hDiv , "2");

    painter.drawLine(10, -1000*pixPerVals[0], 15, -1000*pixPerVals[0] );
    painter.drawText(0, -1000*pixPerVals[0] + hDiv, "1");

    painter.drawLine(10, 1000*pixPerVals[0], 15, 1000*pixPerVals[0] );
    painter.drawText(0, 1000*pixPerVals[0]+ hDiv, "1");

    painter.drawLine(10, 1000*pixPerVals[0] + 1000*pixPerVals[1], 15, 1000*pixPerVals[0] + 1000*pixPerVals[1] );
    painter.drawText(0, 1000*pixPerVals[0] + 1000*pixPerVals[1]+ hDiv, "2");

    painter.drawLine(10, 1000*pixPerVals[0]+1000*pixPerVals[1]+4000*pixPerVals[2], 15, 1000*pixPerVals[0]+1000*pixPerVals[1] +4000*pixPerVals[2]);
    painter.drawText(0, 1000*pixPerVals[0]+1000*pixPerVals[1]+4000*pixPerVals[2]+ hDiv, "6");

    painter.restore();

    QPen penColor(QColor{92,246,0});
    penColor.setWidth(4);
    painter.setPen(penColor);

    float Vsi = std::abs(parameters.verticalSpeedFeetPerMinute);

    int yValue = std::min(Vsi, 1000.0f) * pixPerVals[0];

    if(Vsi> 1000)
    {
        Vsi-=1000;
        yValue += std::min(Vsi, 1000.f) * pixPerVals[1];


        if(Vsi > 1000)
        {
            Vsi -= 1000;
            yValue += Vsi * pixPerVals[2];
        }
    }

    yValue =parameters.verticalSpeedFeetPerMinute>0? -yValue:yValue;

    painter.drawLine(15, yValue, 50,0);

    painter.setBackground(Qt::black);
    painter.setBackgroundMode(Qt::OpaqueMode);

    if(std::abs( parameters.verticalSpeedFeetPerMinute) > 100)
    {
        static const QFont f("Verdana", 10);
        painter.setFont(f);
        painter.drawText(15, yValue+hDiv*4, QString("%1").arg(static_cast<int>(std::abs(parameters.verticalSpeedFeetPerMinute)/100), 2, 10, QChar{'0'}));
    }

    painter.restore();
}

void PfdRenderer::drawHeading(QPainter &painter, const PfdParameters &parameters)
{
    painter.save();

    painter.translate(horizCenterX,horizCenterY+210);

    const int width = 240;
    const int height = 35;

    painter.fillRect(QRect{-width/2,0, width,height-5}, darkGrayColor);

    {
        QPolygon poly;
        poly << QPoint(-width/2, height-5);
        poly << QPoint(-width/2,0);
        poly << QPoint(width/2, 0);
        poly << QPoint(width/2, height-5);


        painter.setPen(Qt::white);
        painter.drawPolyline(poly);
    }

    painter.save();
    painter.setClipRect(-width / 2, 0, width, height, Qt::IntersectClip);

    float pixelsPerHdg = width/50.0f;

    int diffHdg = static_cast<int>(parameters.headingDegrees) % 10;

    int centerHdg = static_cast<int>(parameters.headingDegrees) - diffHdg;

    static const QFont f("Verdana", 12);
    painter.setFont(f);

    for(int hdg = centerHdg-30; hdg <= centerHdg+30; hdg += 5)
    {
        const int xPos =  -(parameters.headingDegrees-hdg) * pixelsPerHdg;

        if( hdg % 10 == 0)
        {
            painter.drawLine(xPos, 0, xPos, 10);

            int hdgDisplay =static_cast<int>(hdg/10.0);
            if( hdgDisplay <0)
                hdgDisplay+=36;
            if( hdgDisplay>35)
                hdgDisplay-=36;

            QString strHeading = QString("%1").arg(hdgDisplay);

            painter.drawText(xPos-5, 25, strHeading);
        }
        else
        {
            painter.drawLine(xPos, 0, xPos, 5);
        }
    }

    painter.restore();
    QPen yellowPen(Qt::yellow);
    yellowPen.setWidth(5);
    painter.setPen(yellowPen);
    painter.drawLine(0,-15,0,5);


    painter.restore();
}
