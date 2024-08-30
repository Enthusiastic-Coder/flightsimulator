#include "stdafx.h"
#include <algorithm>
#include <math.h>
#include <cmath>
#include "AirProperties.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

const double SEA_LEVEL_PRESSURE = 101325.0; // Sea level pressure in Pascals
const double SEA_LEVEL_TEMPERATURE = 288.15; // Sea level temperature in Kelvin
const double GRAVITY = 9.80665;

double AirProperties::Density( double alt )
{
    double temperature = SEA_LEVEL_TEMPERATURE - 0.0065 * alt;
    double pressure = SEA_LEVEL_PRESSURE * pow((1 - 0.0065 * alt / SEA_LEVEL_TEMPERATURE), 5.2561);
    return pressure / (287.05 * temperature); // Ideal gas law
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

// Function to calculate temperature at altitude in Kelvin
double AirProperties::Temperature(double altitude) {
    return SEA_LEVEL_TEMPERATURE - 0.0065 * altitude;
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

double AirProperties::TAS(double ias, double alt, int dTemp)
{
    double pressure = Pressure(alt); // Pressure at altitude in Pascals
    double temperature = Temperature(alt) + dTemp; // Temperature at altitude adjusted by deviation (dTemp)

    // Convert IAS from knots to meters per second
    double iasMps = ias * 0.514444; // 1 knot = 0.514444 m/s

    // Calculate TAS using the density ratio
    double tasMps = iasMps * std::sqrt((SEA_LEVEL_PRESSURE / pressure) * (temperature / SEA_LEVEL_TEMPERATURE));

    // Convert TAS from meters per second to knots
    return tasMps / 0.514444; // Convert back to knots
}


double AirProperties::Airspeed(double TAS, double alt, int dTemp)
{
    double pressure = Pressure(alt); // Pressure at altitude in Pascals
    double temperature = Temperature(alt) + dTemp; // Temperature at altitude adjusted by deviation (dTemp)

    // Convert TAS from knots to meters per second
    double tasMps = TAS * 0.514444; // 1 knot = 0.514444 m/s

    // Calculate IAS using the inverse of the TAS formula
    double iasMps = tasMps / std::sqrt((SEA_LEVEL_PRESSURE / pressure) * (temperature / SEA_LEVEL_TEMPERATURE));

    // Convert IAS from meters per second to knots
    return iasMps / 0.514444; // Convert back to knots
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
