#pragma once

#include <QFont>
#include <QFontDatabase>
#include <QStringList>

inline QFont cockpitDisplayFont(int pixelSize)
{
    static const QString family = [] {
        const int id = QFontDatabase::addApplicationFont(QStringLiteral("Fonts/B612Mono-Regular.ttf"));
        const auto families = QFontDatabase::applicationFontFamilies(id);
        return families.isEmpty() ? QStringLiteral("Consolas") : families.first();
    }();
    QFont font(family);
    font.setPixelSize(pixelSize);
    return font;
}
