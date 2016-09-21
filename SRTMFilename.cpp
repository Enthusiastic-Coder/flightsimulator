#include "stdafx.h"
#include "SRTMFilename.h"
#include <algorithm>


std::string SRTMFilename::GetSRTM3(double fNS, double fWE)
{
	char str_SRTM3[128] = {};

	sprintf_s(str_SRTM3, _countof(str_SRTM3), "%c%02.0f%c%03.0f.hgt",
		fNS < 0 ? 's' : 'n', fabs(floor(fNS)),
		fWE < 0 ? 'w' : 'e', fabs(floor(fWE)));

	return str_SRTM3;
}

std::string SRTMFilename::GetSRTM30(double fNS, double fWE)
{
	int iNS = 0;
	int iWE = 0;

	std::string sWE;
	std::string sNS;

	for (int iWECheck = -180; iWECheck <= 140; iWECheck += 40)
	{
		if (fWE >= iWECheck)
		{
			iWE = abs(iWECheck);
			sWE = iWECheck < 0 ? "w" : "e";
		}
	}

	for (int iNSCheck = -110; iNSCheck <= 90; iNSCheck += 50)
	{
		if (fNS - 1 >= iNSCheck)
		{
			iNS = abs(iNSCheck + 50);
			sNS = iNSCheck + 50 < 0 ? "s" : "n";
		}
	}

	char str_SRTM30[128] = {};
	sprintf_s(str_SRTM30, _countof(str_SRTM30), "%s%03d%s%02d.dem", sWE.c_str(), iWE, sNS.c_str(), iNS);
	return str_SRTM30;
}

GPSLocation SRTMFilename::GetGPSFromSRTM3(std::string strSRTM3FullPath)
{
	char fname[_MAX_FNAME] = {};
	_splitpath_s(strSRTM3FullPath.c_str(), NULL, 0, NULL, 0, fname, _countof(fname), NULL, 0);
	std::string strSRTM3 = fname;

	std::transform(strSRTM3.begin(), strSRTM3.end(), strSRTM3.begin(), ::tolower);
	GPSLocation gpsLocation;

	gpsLocation._lat = atof((strSRTM3.substr(1, 2).c_str()));
	gpsLocation._lng = atof((strSRTM3.substr(4, 3).c_str()));

	if (strSRTM3[0] == 's')
		gpsLocation._lat = -gpsLocation._lat;

	if (strSRTM3[3] == 'w')
		gpsLocation._lng = -gpsLocation._lng;

	return gpsLocation;
}
