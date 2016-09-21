#include "stdafx.h"
#include "TurnDirection.h"
#include <limits>
#include <cmath>

float TurnDirection::GetTurnDiff(float fHdg, float fBrg)
{
    while (fHdg > 360)
        fHdg -= 360;

    while (fHdg< 0)
        fHdg += 360;

    while (fBrg > 360)
        fBrg -= 360;

    while (fBrg< 0)
        fBrg += 360;

    float fDiff = fBrg - fHdg;

    if (fDiff < -180)
        fDiff += 360;

    if (fDiff > 180)
        fDiff -= 360;

    return fDiff;
}

TurnDirection::Dir TurnDirection::GetTurnDir(float fHdg, float fBrg)
{
    float fDiff = GetTurnDiff(fHdg, fBrg);

    if (fabs((std::fabs(fDiff) - 180)) < std::numeric_limits<float>::epsilon())
		return Dir::Free;

    if (fDiff < 0)
        return Dir::Left;
    else
        return Dir::Right;
}
