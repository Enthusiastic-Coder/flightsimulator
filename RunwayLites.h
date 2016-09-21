#pragma once

template<class T>
class RunwayLitesOpenGL : public IMeshModel
{
public:
	RunwayLitesOpenGL():
		_fLightingFraction(1.0f),
		_bshadow(false),
		_light_approach_idx(0)
	{
	}

	void CurveScenery( const GPSLocation& gpsLoc )
	{
		
	}

	void initialiseSides()
	{
		for( int ypos = 0; ypos <= 2600; ypos +=25)
		{
			_ColorData.push_back(1);
			_ColorData.push_back(1);
			_ColorData.push_back(1);
			_VertexData.push_back(-27 );
			_VertexData.push_back(0.3f );
			_VertexData.push_back(-ypos);

			_ColorData.push_back(1);
			_ColorData.push_back(1);
			_ColorData.push_back(1);
			_VertexData.push_back(27 );
			_VertexData.push_back(0.3f );
			_VertexData.push_back(-ypos );
		}
	}

	void initialise()
	{
		float F_to_M = 1/3.2808f;
		_light_approach_idx = _VertexData.size();

		for( int ypos = -3000; ypos <= 3000; ypos +=100)
		{
			if( ypos < 0 )
			{
				_ColorData.push_back(1);
				_ColorData.push_back(1);
				_ColorData.push_back(1);

				_VertexData.push_back(-35* F_to_M );
				_VertexData.push_back(2* F_to_M );
				_VertexData.push_back(ypos* F_to_M );

				_ColorData.push_back(1);
				_ColorData.push_back(1);
				_ColorData.push_back(1);
				_VertexData.push_back( -45* F_to_M );
				_VertexData.push_back( 2* F_to_M );
				_VertexData.push_back( ypos* F_to_M );

				_ColorData.push_back(1);
				_ColorData.push_back(1);
				_ColorData.push_back(1);
				_VertexData.push_back( 35* F_to_M );
				_VertexData.push_back( 2* F_to_M );
				_VertexData.push_back( ypos* F_to_M );

				_ColorData.push_back(1);
				_ColorData.push_back(1);
				_ColorData.push_back(1);
				_VertexData.push_back( 45* F_to_M );
				_VertexData.push_back( 2* F_to_M );
				_VertexData.push_back( ypos* F_to_M );
			}
			else
			{
				_StrobeIdx.push_back(_ColorData.size());
				_ColorData.push_back(0.55f);
				_ColorData.push_back(0.55f);
				_ColorData.push_back(0.55f);

				_VertexData.push_back( -4* F_to_M );
				_VertexData.push_back( 2.5f* F_to_M );
				_VertexData.push_back( ypos* F_to_M );

				_ColorData.push_back(0.55f);
				_ColorData.push_back(0.55f);
				_ColorData.push_back(0.55f);
				_VertexData.push_back( 0 );
				_VertexData.push_back( 2.5f* F_to_M );
				_VertexData.push_back( ypos* F_to_M );

				_ColorData.push_back(0.55f);
				_ColorData.push_back(0.55f);
				_ColorData.push_back(0.55f);
				_VertexData.push_back( 4* F_to_M );
				_VertexData.push_back( 2.5f* F_to_M );
				_VertexData.push_back( ypos* F_to_M );
			}

			if( ypos < 1000 && ypos > 0 )
			{
				_ColorData.push_back(1);
				_ColorData.push_back(0);
				_ColorData.push_back(0);
				
				_VertexData.push_back( -25* F_to_M );
				_VertexData.push_back(6 * ypos/1000.0* F_to_M );
				_VertexData.push_back(ypos* F_to_M );

				_ColorData.push_back(1);
				_ColorData.push_back(0);
				_ColorData.push_back(0);
				_VertexData.push_back(-30* F_to_M );
				_VertexData.push_back(6 * ypos/1000.0* F_to_M);
				_VertexData.push_back(ypos* F_to_M );

				_ColorData.push_back(1);
				_ColorData.push_back(0);
				_ColorData.push_back(0);
				_VertexData.push_back( -35* F_to_M );
				_VertexData.push_back(6 * ypos/1000.0* F_to_M );
				_VertexData.push_back(ypos* F_to_M );
				
				_ColorData.push_back(1);
				_ColorData.push_back(0);
				_ColorData.push_back(0);
				_VertexData.push_back( 25* F_to_M );
				_VertexData.push_back( 6 * ypos/1000.0* F_to_M );
				_VertexData.push_back( ypos* F_to_M );

				_ColorData.push_back(1);
				_ColorData.push_back(0);
				_ColorData.push_back(0);
				_VertexData.push_back( 30* F_to_M );
				_VertexData.push_back( 6 * ypos/1000.0* F_to_M );
				_VertexData.push_back( ypos* F_to_M );

				_ColorData.push_back(1);
				_ColorData.push_back(0);
				_ColorData.push_back(0);
				_VertexData.push_back( 35* F_to_M );
				_VertexData.push_back( 6 * ypos/1000.0* F_to_M );
				_VertexData.push_back( ypos* F_to_M );
			}

			//Horizonal 1+2
			if( ypos == 500 || ypos == 1000 )
			{
				for( int j = -30; j < 40; j+=10 )
				{
					_ColorData.push_back(1);
					_ColorData.push_back(1);
					_ColorData.push_back(1);

					_VertexData.push_back( j* F_to_M );
					_VertexData.push_back( 6 * ypos/1000.0* F_to_M );
					_VertexData.push_back( ypos* F_to_M );
				}			
			}

			//Horizonal 3
			if( ypos == 1500 )
			{
				for( int j = -40; j < 50; j+=10 )
				{
					if( !j )
						continue;

					_ColorData.push_back(1);
					_ColorData.push_back(1);
					_ColorData.push_back(1);

					_VertexData.push_back( j* F_to_M );
					_VertexData.push_back( 6 * ypos/1000.0* F_to_M );
					_VertexData.push_back( ypos* F_to_M );
				}
			}
			//Horizonal 4
			if( ypos == 2000 )
			{
				for( int j = -85; j < 85; j+=20 )
				{
					if( !j )
						continue;
					
					_ColorData.push_back(1);
					_ColorData.push_back(1);
					_ColorData.push_back(1);
					
					_VertexData.push_back( j* F_to_M );
					_VertexData.push_back( 6 * ypos/1000.0* F_to_M );
					_VertexData.push_back( ypos* F_to_M );
				}		
			}
			//Horizonal 5
			if( ypos == 2500 )
			{
				for( int j = -105; j < 105; j+=15 )
				{
					if( !j )
						continue;

					_ColorData.push_back(1);
					_ColorData.push_back(1);
					_ColorData.push_back(1);
					
					_VertexData.push_back( j* F_to_M );
					_VertexData.push_back( 6 * ypos/1000.0* F_to_M );
					_VertexData.push_back( ypos* F_to_M );
				}		
			}
		}
	}

	void renderMesh(float fLightingFraction, unsigned int camID, unsigned int shadowMapCount) override
	{
		_fLightingFraction = fLightingFraction;

		glDisable(GL_TEXTURE_2D);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);

		glEnableClientState(GL_COLOR_ARRAY );
		glColorPointer(3, GL_FLOAT, 0, &_ColorData[0]);

		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0, &_VertexData[0]);
		
		glPointSize(2.0f);
		glDrawArrays(GL_POINTS, 0, _light_approach_idx/3 );
		glPointSize(1.0f);
		glDrawArrays(GL_POINTS, (_light_approach_idx+1)/3, (_VertexData.size()-_light_approach_idx)/3 );
		
		glDisableClientState(GL_COLOR_ARRAY );
		glDisableClientState(GL_VERTEX_ARRAY );

		glColor3f(1,1,1);
	}

	float _fLightingFraction;
	bool _bshadow;

	std::vector<GLfloat> _VertexData;
	std::vector<GLfloat> _ColorData;
	std::vector<GLuint> _StrobeIdx;
	int _light_approach_idx;
};

template<class T>
class RunwayLitesRigidBody : 
	public GSRigidBody
{
public:
	RunwayLitesRigidBody(float fMass, const Matrix3x3D& inertia, JSONRigidBodyCollection *pParent);

	void update(double dt)
	{
		static float F_to_M = 1/3.2808f;

		T* pT = static_cast<T*>(this);
		
		size_t strobe_count = pT->_StrobeIdx.size();

		_light_idx -= dt * strobe_count;

		if( _light_idx < 0 )
			_light_idx = strobe_count;
		
		for( size_t i = 0; i < strobe_count; i++ )
		{
			float fColor = 1.0f;
			if( abs(i -_light_idx) > 1 )
				fColor = 0.25f;
			
			size_t startidx = pT->_StrobeIdx[i];
	
			for( size_t j=0 ; j < 9; j++ )
				pT->_ColorData[startidx+j] = fColor;
		}
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

	float _light_idx;
};

template<class T> 
RunwayLitesRigidBody<T>::RunwayLitesRigidBody(float fMass, const Matrix3x3D& inertia, JSONRigidBodyCollection *pParent)
: _light_idx(0.0f)
{
	setMass( fMass );
	setInertiaMatrix( inertia );
	setCG(VectorD() );
}

//////////////////////////////////////////////////////////////////////////////////////////////

class RunwayLites : 
	public RunwayLitesRigidBody<RunwayLites>, 
	public RunwayLitesOpenGL<RunwayLites>
{
public:
	RunwayLites()
		: RunwayLitesRigidBody(100, Matrix3x3D(1,0,0,1,0,0,1,0,0),nullptr) { }

	void CurveScenery()
	{
		RunwayLitesOpenGL<RunwayLites>::CurveScenery(getGPSLocation());
	}

	void persistRead(FILE* fPersistFile )
	{
	}

	void persistWrite(FILE* fPersistFile )
	{
	}

};
