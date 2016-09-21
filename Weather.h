#pragma once

#include "interfaces.h"
#include <vector3.h>

class Weather : public IWeather
{
public:
	Weather();
    Vector3D getWindFromPosition(const Vector3D& bodyOffset) override;
	void update(double dt);

private:
    Vector3D _windUnit;

	double _speed;
	double _dir;
	double _dirSpeed;
	double _dirAccl;
};
