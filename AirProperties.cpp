#include "StdAfx.h"
#include <math.h>
#include <cmath>
#include "AirProperties.h"

double AirProperties::Density( double alt )
{
	float dTemp;
	float dPressure;

	alt = alt < 0 ? 0 : alt;
	dTemp = 288.15 - 0.0065 * alt;
	double ratio = 1-0.0065f * alt/ 288.15;
	ratio = ratio < 0 ? 0 : ratio;
	dPressure = 101325 * pow(ratio, 5.25 );
	
	return 0.00348 * dPressure/dTemp;
}

double AirProperties::SpeedOfSound( double alt, int dt )
{
	double dTemp;
	double gamma;
	double rstar;
	double mo;

	alt = alt < 0 ? 0 : alt;
	gamma = 1.4;
	rstar = 8314.32;
	mo = 28.96442528;

	dTemp = 288.15 - 0.0065 * alt;
	dTemp += dt;

	return sqrt( gamma * rstar* dTemp / mo );
}

double AirProperties::GeoPotential( double alt)
{
	float ro = 6356766;
	alt = alt < 0 ? 0 : alt;
	return ro * alt / (ro + alt );
}

double AirProperties::Pressure(double alt)
{
	double geo = GeoPotential(alt);
	double i;

	alt = alt < 0 ? 0 : alt;
	geo > 11000? i=1: i=0;
	double basetemp = i? 216.65f : 288.15f;
	double hb = i? 11000 : 0;
	double tempgrad = i? 0 : -0.0065;
	double deltaalt = geo - hb;

	double temp = basetemp + tempgrad * deltaalt;
	double basepress = i ? 22632.06397 : 101325;

	double go = 9.80655f;
	double mo = 28.96442528f;
	double rstar = 8314.32f;

	double pressure;

    if( std::abs(tempgrad ) < 0.0000000001 )
	{
		pressure = basepress * exp(-go * mo* deltaalt /(rstar*basetemp));
	}
	else
	{
		pressure = basepress * pow( basetemp/temp, go*mo/ (rstar * tempgrad ) );
	}

	return pressure;
}

double AirProperties::TAS( double airspeed, double alt, int dTemp)
{
	static double gamma = 1.4f;
	static double rstar = 8314.32f;
	static double t0 = 288.15f;
	static double m0 = 28.96442528;
	static double a0 = sqrt( gamma * rstar * t0 / m0 );
	static double p0 = 101325;

	alt = alt < 0 ? 0 : alt;
	double comp = p0 * ( pow( airspeed*airspeed/ (a0*a0)* (gamma-1)/2 +1, gamma /(gamma-1)) -1 );
	double pressure = Pressure( alt );
	double sound = SpeedOfSound( alt, dTemp );
	double mach = sqrt( (pow(comp/pressure + 1, (gamma-1)/gamma ) -1 ) *2 / (gamma-1) );

	return mach * sound;
}

double AirProperties::Airspeed( double TAS, double alt, int dTemp)
{
	static double gamma = 1.4f;
	static double rstar = 8314.32f;
	static double t0 = 288.15f;
	static double m0 = 28.96442528f;
	static double a0 = sqrt( gamma * rstar * t0 / m0 );
	static double p0 = 101325;

	alt = alt < 0 ? 0 : alt;
	double pressure = Pressure( alt );
	double sound = SpeedOfSound( alt, dTemp );

	double mach = TAS / sound;
	double comp = pressure * (pow( 1+ (gamma -1 )/2 * mach * mach, gamma/ ( gamma-1) ) - 1);

	return a0* sqrt(2/(gamma-1)* (pow( comp /p0 + 1, (gamma-1)/gamma) -1 ));
}

