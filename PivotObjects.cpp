#include "stdafx.h"
#include "PivotObjects.h"


PivotObjects::PivotObjects() :
    _debugMapFile(nullptr),
    _bDoLog(false)
{}

bool PivotObjects::add(std::string sName, GSForceGenerator::PivotObject *pPO)
{
	if (sName.length() == 0)
		return false;

	if (_pivotMap[sName] != nullptr)
		return false;

	_pivotMap[sName] = pPO;
	return true;
}

GSForceGenerator::PivotObject* PivotObjects::operator[](const std::string& sName)
{
	return _pivotMap[sName];
}

std::string PivotObjects::name(GSForceGenerator::PivotObject *pPO)
{
	for (auto& it : _pivotMap)
		if (it.second == pPO)
		{
			if (_debugMapFile)
				fprintf(_debugMapFile, "%s\n", it.first.c_str());

			return it.first;
		}

	return "##noname##";
}

void PivotObjects::startLog(bool bStart, std::string sFileName)
{
	if (bStart)
	{
		if (_debugMapFile)
			fclose(_debugMapFile);

        fopen_s(&_debugMapFile, sFileName.c_str(), "wt");
	}
	else if (_debugMapFile)
	{
		fclose(_debugMapFile);
		_debugMapFile = nullptr;
	}
}
