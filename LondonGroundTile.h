#pragma once

#include "JSONRigidBodyBuilder.h"
#include "MeshModel.h"

class LondonTileMeshModel : public MeshModel
{
public:
	void Build();

};

template<class T>
class LondonTileOpenGL : public IMeshModel
{
public:
	LondonTileOpenGL() :
		_fLightingFraction(1.0f),
		_bshadow(false)
	{
	}

	void initialise()
	{
		_VertexData.clear();
	}

	void renderMesh(float fLightingFraction, unsigned int camID, unsigned int shadowMapCount) override
	{
		if( _bshadow )
			return;

		T *pThis = static_cast<T*>(this);

		if( !_ground_texture )
			_ground_texture.Load( "images\\central_london.bmp" );

		_fLightingFraction = fLightingFraction;
		glColor3f(_fLightingFraction,_fLightingFraction,_fLightingFraction);

		glEnable(GL_TEXTURE_2D );
	
		//glDisable(GL_DEPTH_TEST);
		glDepthMask(GL_FALSE);
		_ground_texture.bind();
		float texRepeat = 10.0f;
		float texFactor = texRepeat * 1609;
		
		glBegin(GL_QUADS);
			glNormal3f( 0, 1,0);

			glTexCoord2f( 0, 0);
			glVertex3f( -texFactor, -1, texFactor );

			glTexCoord2f( texRepeat, 0);
			glVertex3f( texFactor, -1, texFactor );

			glTexCoord2f( texRepeat, texRepeat );
			glVertex3f( texFactor, -1, -texFactor);

			glTexCoord2f( 0, texRepeat);
			glVertex3f( -texFactor, -1, -texFactor );

		glEnd();
		glDepthMask(GL_TRUE);
		//glEnable( GL_DEPTH_TEST );

		glDisable(GL_TEXTURE_2D );
	}

	std::vector<Vector3D> _VertexData;
	float _fLightingFraction;
	bool _bshadow;
	OpenGLTexture2D _ground_texture;
};

/////////////////////////////////////////////////////////////

class LondonTileRigidBody : public JSONRigidBody
{
public:
	LondonTileRigidBody ();

};


