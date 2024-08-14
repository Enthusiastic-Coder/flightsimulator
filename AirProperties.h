#pragma once

class AirProperties
{
public:
	static double Density( double alt );
	static double SpeedOfSound( double alt, int dt );	
	static double GeoPotential( double alt);
	static double Pressure(double alt);
	static double TAS( double airspeed, double alt, int dTemp);
	static double Airspeed( double TAS, double alt, int dTemp);
	static double CalculateVSI(double airspeed, double altitude, int dTemp);
	static double GlideRatio(double airspeed, double altitude, int dTemp);
};

