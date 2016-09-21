#pragma once

template<class T>
class WaterTileOpenGL : public IMeshModel
{
public:
	WaterTileOpenGL():
		_fLightingFraction(1.0f),
		_bshadow(false),
		_WaterIdx(0.0f)
	{
	}

	void initialise()
	{
		for( size_t i=0 ; i < 300; i++ )
		{
			char buf[256]={};
			sprintf( buf, "dispWater\\disp_water%03d.jpg", i );
			_waterTextures[i].Load( buf );
		}
		
	}

	void CurveScenery( const GPSLocation& gpsLoc )
	{
		
	}

	void renderMesh(float fLightingFraction, unsigned int camID, unsigned int shadowMapCount) override
	{
		_fLightingFraction = fLightingFraction;
		glColor3f(_fLightingFraction*0.1f,_fLightingFraction*0.4f,_fLightingFraction*1.0f);

		glEnable(GL_TEXTURE_2D );
	
		glDepthMask(GL_FALSE);
		if( _WaterIdx > 299 )
			_WaterIdx = 0;

		_waterTextures[(int)_WaterIdx].bind();

		float texRepeat = 15.0f;
		float texFactor = texRepeat * 40;
		
		glBegin(GL_QUADS);
			glNormal3f( 0, 1,0);

			glTexCoord2f( 0, 0);
			glVertex3f( -texFactor, 0.0f, texFactor );

			glTexCoord2f( texRepeat, 0);
			glVertex3f( texFactor, 0.0f, texFactor );

			glTexCoord2f( texRepeat, texRepeat );
			glVertex3f( texFactor, 0.0f, -texFactor);

			glTexCoord2f( 0, texRepeat);
			glVertex3f( -texFactor, 0.0f, -texFactor );

		glEnd();
		glDepthMask(GL_TRUE);

		glDisable(GL_TEXTURE_2D );
	}

	float _fLightingFraction;
	bool _bshadow;
	OpenGLTexture2D _waterTextures[300];
	float _WaterIdx;
};

template<class T>
class WaterTileRigidBody : 
	public GSRigidBody
{
public:
	WaterTileRigidBody  (float fMass, const Matrix3x3D& inertia, JSONRigidBodyCollection *pParent);

	void update(double dt)
	{
		static_cast<T*>(this)->_WaterIdx += (float)dt*20;
	}

	void draw(float fLightFraction)
	{
		T* t = static_cast<T*>(this);
		t->renderMesh(fLightFraction,false);
	}

	void drawShadow(float fLightFraction)
	{
	}

	void drawWindTunnel(double dt)
	{
	}

	void Attach( JSONRigidBodyCollection *pParent )
	{
		JSONRigidBody *pJSONBody = pParent->createStaticJSONBody( this );
	}
};

template<class T> 
WaterTileRigidBody<T>::WaterTileRigidBody(float fMass, const Matrix3x3D& inertia, JSONRigidBodyCollection *pParent)
{
	setMass( fMass );
	setInertiaMatrix( inertia );
	setCG(VectorD() );
}

//////////////////////////////////////////////////////////////////////////////////////////////

class WaterTile : 
	public WaterTileRigidBody<WaterTile>, 
	public WaterTileOpenGL<WaterTile>
{
public:
	WaterTile()
		: WaterTileRigidBody(100, Matrix3x3D(1,0,0,1,0,0,1,0,0),nullptr) 
	{ 
	}

	void CurveScenery()
	{
		WaterTileOpenGL<WaterTile>::CurveScenery(getGPSLocation());
	}

	void persistRead(FILE* fPersistFile )
	{
	}

	void persistWrite(FILE* fPersistFile )
	{
	}

};
