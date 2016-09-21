#pragma once

#include "Interfaces.h"

class WorldSystem;

class SceneryManager : public ISceneryEnvironment
{
public:
	SceneryManager(WorldSystem *pWorldSystem);
    bool getHeightFromPosition(const GPSLocation& position, HeightData & heightData) const override;

private:
	WorldSystem *_pWorldSystem;
};
