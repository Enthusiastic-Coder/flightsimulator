#pragma once

#include <optional>

enum class IlsRunway { Off, R27R, R27L, R09L, R09R, Count };

struct IlsGuidance
{
    IlsRunway runway = IlsRunway::Off;
    std::optional<double> localizerDots;  // Positive moves the cue right.
    std::optional<double> glideSlopeDots; // Positive moves the cue up.
    std::optional<int> thresholdDistanceTenthsNm;

    bool operator==(const IlsGuidance& other) const noexcept
    {
        return runway == other.runway && localizerDots == other.localizerDots &&
            glideSlopeDots == other.glideSlopeDots &&
            thresholdDistanceTenthsNm == other.thresholdDistanceTenthsNm;
    }
};
