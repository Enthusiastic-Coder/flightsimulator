#include "stdafx.h"
#include "Weather.h"
#include <MathSupport.h>
#include <vector3.h>
#include <Quarternion.h>

static const double kts_to_meters = 1.15 * 1.609334 * 1000 / 3600;

Weather::Weather() :
	_speed(0 * kts_to_meters),
	_dir(180.0),
	_dirSpeed(0.0),
	_dirAccl(0.0)
{
}

Vector3D Weather::getWindFromPosition(const Vector3D& bodyOffset)
{
	//return _windUnit * _speed;
	return Vector3D(0, 0, kts_to_meters * 5 * 0);
}

void Weather::update(double dt)
{
	double dHdg = 180 - _dir;
	if (fabs(dHdg) > 180)
	{
		_dirAccl = (rand() % 10) * dt;

		if (dHdg < 0)
			_dirAccl = -_dirAccl;
	}
	else
	{
		_dirAccl = ((rand() % 10) - 5) * dt;
	}

	_dirSpeed += _dirAccl * dt;
	_dir += _dirSpeed * dt;

	_windUnit = QVRotate(MathSupport<double>::MakeQHeading(_dir), Vector3D(0, 0, -1));
}
