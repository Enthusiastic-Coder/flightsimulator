#include <algorithm>
#include <math.h>
#include <cmath>
#include "AirProperties.h"

double AirProperties::Density( double alt )
{
	// Simplified ISA model
	double temperature = 288.15 - 0.0065 * alt; // Temperature in K
	double pressure = 101325 * pow((temperature / 288.15), (9.80665 / (0.0065 * 287.05))); // Pressure in Pascals
	return pressure / (287.05 * temperature); // Air density in kg/m^3
}

double AirProperties::SpeedOfSound( double alt, int dt )
{
	double gamma = 1.4;
	double R = 287.05;  // Specific gas constant for air in J/(kg·K)
	double M = 0.0289644;  // Molar mass of air in kg/mol

	alt = std::max(alt, 0.0);  // Ensure altitude is non-negative
	double dTemp = 288.15 - 0.0065 * alt + dt;  // Temperature in Kelvin

	return std::sqrt(gamma * R * dTemp / M);
}

double AirProperties::GeoPotential( double alt)
{
	double R_earth = 6371000.0;  // Radius of the Earth in meters
	alt = std::max(alt, 0.0);  // Ensure altitude is non-negative
	return R_earth * alt / (R_earth + alt);
}

double AirProperties::Pressure(double alt)
{
	double T0 = 288.15;  // Base temperature in K at sea level
	double P0 = 101325.0;  // Base pressure in Pa at sea level
	double L = 0.0065;  // Temperature lapse rate in K/m
	double R = 287.05;  // Specific gas constant for air in J/(kg·K)
	double g0 = 9.80665;  // Standard gravitational acceleration in m/s²
	double H0 = 11000.0;  // Tropospheric height in meters

	alt = std::max(alt, 0.0);  // Ensure altitude is non-negative

	double T;
	double P;

	if (alt <= H0) {
		T = T0 - L * alt;
		P = P0 * std::pow(1 - (L * alt / T0), g0 / (R * L));
	}
	else {
		T = 216.65;  // Temperature in the stratosphere
		P = 22632.06 * std::exp(-g0 * (alt - H0) / (R * T));
	}

	return P;
}

double AirProperties::TAS( double airspeed, double alt, int dTemp)
{
	double soundSpeed = SpeedOfSound(alt, dTemp);
	double pressure = Pressure(alt);

	double gamma = 1.4;
	double R = 287.05;  // Specific gas constant for air in J/(kg·K)
	double p0 = 101325.0;  // Sea level pressure in Pa

	double machNumber = airspeed / soundSpeed;
	double comp = p0 * (std::pow(airspeed * airspeed / (soundSpeed * soundSpeed) * (gamma - 1) / 2 + 1, gamma / (gamma - 1)) - 1);

	return machNumber * soundSpeed;
}

double AirProperties::Airspeed( double TAS, double alt, int dTemp)
{
	double soundSpeed = SpeedOfSound(alt, dTemp);
	double pressure = Pressure(alt);

	double gamma = 1.4;
	double R = 287.05;  // Specific gas constant for air in J/(kg·K)
	double p0 = 101325.0;  // Sea level pressure in Pa

	double machNumber = TAS / soundSpeed;
	double comp = pressure * (std::pow(1 + (gamma - 1) / 2 * machNumber * machNumber, gamma / (gamma - 1)) - 1);

	return soundSpeed * std::sqrt(2 / (gamma - 1) * (std::pow(comp / p0 + 1, (gamma - 1) / gamma) - 1));
}

double AirProperties::GlideRatio(double airspeed, double altitude, int dTemp) 
{
	// Example fixed glide ratio, replace with actual computation or lookup
	return 15.0; // Glide ratio can be different for different speeds and configurations
}

double AirProperties::CalculateVSI(double airspeed, double altitude, int dTemp) 
{
	// Calculate the true airspeed
	double tas = TAS(airspeed, altitude, dTemp); // Assume TAS function is defined as given

	// Calculate glide ratio (this function needs to be implemented based on aircraft data)
	double glideRatio = GlideRatio(airspeed, altitude, dTemp); // You need to implement GlideRatio function

	// Calculate VSI
	double vsi = tas / glideRatio; // Vertical speed in meters per second

	// Convert VSI from m/s to feet/min
	double vsiFeetPerMin = vsi * 60 * 3.281;

	return vsiFeetPerMin;
}