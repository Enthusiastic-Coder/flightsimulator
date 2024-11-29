#include "stdafx.h"
#include <stdlib.h>
#include <string> 
#include <algorithm>
#include <fstream>
#include "MipMapTerrainMeshModelCollection.h"
#include "MipMapTerrainMeshModel.h"
#include "OpenGLRenderer.h"
#include "OpenGLShaderProgram.h"
#include "OpenGLPipeline.h"

size_t MipMapTerrainMeshModelCollection::size() const
{
	return _collection.size();
}

void MipMapTerrainMeshModelCollection::Render(Renderer *args, bool bLand, bool bReflection, unsigned int shadowMapCount, const GPSLocation& cameraPos)
{
    OpenGLPipeline& mat = OpenGLPipeline::Get(args->camID);

	float fDiff(0.01);

	mat.GetProjection().Top().m34 += fDiff;

    glEnable(GL_STENCIL_TEST);

	if (!bLand)
	{
		glStencilFunc(GL_EQUAL, 0, 0xFF);
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	}
	else
	{
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	}

    for (MipMapTerrainMeshModel* it : _collection)
	{
		mat.Push();

        for (unsigned int shadowID = args->camID + 1; shadowID <= args->camID + shadowMapCount; ++shadowID)
		{
			OpenGLPipeline::Get(shadowID).GetProjection().Top().m34 += fDiff;
			OpenGLPipeline::Get(shadowID).Push();
            OpenGLPipeline::Get(shadowID).GetModel().TranslateLocation(it->GetLocation());
            OpenGLPipeline::Get(shadowID).ApplyLightBPMV(args->progId());
		}

		GPSLocation loc = it->GetLocation();

        mat.GetModel().TranslateLocation(loc);
		
		if (bReflection)
		{
			mat.GetModel().Scale(1, -1, 1);
		}

        mat.bindMatrices(args->progId());

        it->Render(args, bLand, cameraPos);

        for (unsigned int shadowID = args->camID + 1; shadowID <= args->camID + shadowMapCount; ++shadowID)
		{
			OpenGLPipeline::Get(shadowID).Pop();
			OpenGLPipeline::Get(shadowID).GetProjection().Top().m34 -= fDiff;
            OpenGLPipeline::Get(shadowID).ApplyLightBPMV(args->progId());
		}

		mat.Pop();
	}

    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	mat.GetProjection().Top().m34 -= fDiff;
	glDisable(GL_STENCIL_TEST);
}

void MipMapTerrainMeshModelCollection::RenderChildren(Renderer* r, unsigned int shadowMapCount, const GPSLocation& cameraPos)
{
    for (MipMapTerrainMeshModel* it : _collection)
        it->RenderChildren(r, shadowMapCount);
}

bool MipMapTerrainMeshModelCollection::LoadTile( std::string strFilename )
{
	std::ifstream inFile(strFilename, std::ios::binary );
	if( !inFile.is_open() )
		return false;

	BinaryReadStream inStoreFile( inFile );
    _collection.push_back(new MipMapTerrainMeshModel );
	_collection.back()->Read(&inStoreFile);
	return true;
}

bool MipMapTerrainMeshModelCollection::LoadSRTM30(std::string strTitleName )
{
    _collection.push_back(new MipMapTerrainMeshModel);
	return _collection.back()->Load(strTitleName );
}

bool MipMapTerrainMeshModelCollection::SaveLastTile( std::string strFilename )
{
	std::ofstream outFile(strFilename, std::ios::ate | std::ios::binary );
	if( !outFile.is_open() )
		return false;

	BinaryWriteStream outStoreFile( outFile );
	_collection.back()->Write(&outStoreFile);
	return true;
}

void MipMapTerrainMeshModelCollection::OnInitialise()
{
}

void MipMapTerrainMeshModelCollection::BuildVertexBuffers()
{
    for(MipMapTerrainMeshModel* it : _collection )
		it->BuildVertexBuffers();
}

void MipMapTerrainMeshModelCollection::Build()
{
    for(MipMapTerrainMeshModel* it : _collection )
		it->Build();
}

void MipMapTerrainMeshModelCollection::BuildLastTile()
{
	_collection.back()->Build();
}

MipMapTerrainMeshModel* MipMapTerrainMeshModelCollection::operator[](size_t idx )
{
    return _collection[idx];
}

bool MipMapTerrainMeshModelCollection::getHeightFromPosition( const GPSLocation& gpsLocation, HeightData& heightData ) const
{
    for( MipMapTerrainMeshModel* it : _collection )
    {
        if( it->getHeightFromPosition( gpsLocation, heightData ) )
			return true;
    }

	return false;
}

