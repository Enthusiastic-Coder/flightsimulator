#pragma once

#include "stdafx.h"
#include "Paths.h"
#include "MipMapTerrainMeshModel.h"

class Renderer;

class MipMapTerrainMeshModelCollection : public ISceneryEnvironment
{
public:
	void OnInitialise();
	bool LoadSRTM30(std::string strTitleName);
	bool LoadTile( std::string strFilename );
	bool SaveLastTile( std::string strFilename );
	
	void Build();
	void BuildLastTile();
	void BuildVertexBuffers();
	size_t size() const;
	MipMapTerrainMeshModel* operator[](size_t idx );

    void Render(Renderer *args, bool bLand, bool bReflection, unsigned int shadowMapCount, const GPSLocation& cameraPos);
    void RenderChildren(Renderer *r, unsigned int shadowMapCount, const GPSLocation& cameraPos);
	bool getHeightFromPosition( const GPSLocation& gpsLocation, HeightData& heightData ) const override;

private:
    std::vector<MipMapTerrainMeshModel*> _collection;
};



