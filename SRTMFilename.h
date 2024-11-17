#pragma once

#include <string>
#include <jibbs/gps/GPSLocation.h>

class SRTMFilename
{
public:
	static std::string GetSRTM3(double fNS, double fWE);
	static std::string GetSRTM30(double fNS, double fWE);
	static GPSLocation GetGPSFromSRTM3(std::string strSRTM3);
};

