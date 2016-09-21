#pragma once

#include "GSForceGenerator.h"

class PivotObjects
{
public:
	PivotObjects();

	bool add(std::string sName, GSForceGenerator::PivotObject *pPO);
	GSForceGenerator::PivotObject* operator[](const std::string& sName);
	std::string name(GSForceGenerator::PivotObject *pPO);
	void startLog(bool bStart, std::string sFileName = "");

private:
	FILE *_debugMapFile;
	bool _bDoLog;
	std::map<std::string, GSForceGenerator::PivotObject*> _pivotMap;
};