#pragma once

#include <QJsonValue>
#include <jibbs/vector/vector3.h>
#include <cmath>
#include <locale>
#include <optional>
#include <sstream>

// Saved coordinates use the SDK's space-separated string format.
template<typename T>
std::optional<Vector3<T>> readJsonVector(const QJsonValue& value)
{
    if (!value.isString())
        return std::nullopt;

    std::istringstream stream(value.toString().toStdString());
    stream.imbue(std::locale::classic());
    Vector3<T> coordinates;
    if (!(stream >> coordinates.x >> coordinates.y >> coordinates.z) ||
        !std::isfinite(coordinates.x) || !std::isfinite(coordinates.y) || !std::isfinite(coordinates.z))
        return std::nullopt;

    return coordinates;
}
