#pragma once

#include <jibbs/math/MathSupport.h>
#include <jibbs/matrix/Matrix3x3.h>

template<class T>
class RingRoadOpenGL : public IMeshModel
{
public:
	RingRoadOpenGL():
		_fLightingFraction(1.0f),
		_bshadow(false),
		_ModelRoadListID(0),
		_ModelRoadLinesListID(0)
	{
	}

	~RingRoadOpenGL()
	{
		glDeleteLists( _ModelRoadListID, 1 );
		glDeleteLists( _ModelRoadLinesListID, 1 );
	}

	void Initialise( double dWidth, double dDir, double dRadius, double dAngleStep )
	{
		_road_texture.Load( "images\\roads\\road_doubleyellow_512.png" );
		double dCircum = 2 * M_PI * dRadius;
		double dDist = dCircum / (360.0/ dAngleStep);
		QuarternionD qCirclePath = MathSupport<double>::MakeQOrientation(Vector3D(- dAngleStep, -dDir, 0));
		QuarternionD qStraightPath = MathSupport<double>::MakeQOrientation(Vector3D(0, -dDir, 0 ));
	
		QuadPlaneBoundaryT<> boundary;
		VectorD b2(dWidth, -1.0, 0);
		VectorD b3(0, -1.0, 0);
		boundary[0] = b3;
		boundary[1] = b2;
		boundary[2] = b2;
		boundary[3] = b3;

		_roadBoundaryLayout.clear();
		_roadTextureLayout.clear();

		double dU(0);
		VectorD itPath( 0, 0.02f , -dDist );
		float distAngle = 40/dDist;

		for( double dAngle = -distAngle; dAngle < 360+distAngle; dAngle += dAngleStep )
		{
			if( dAngle > 0 && dAngle < 370 )
				itPath = QVRotate( qCirclePath, itPath );
			else
				itPath = QVRotate( qStraightPath, itPath );

			boundary[2] += itPath;
			boundary[3] += itPath;

			_roadBoundaryLayout.push_back( boundary );

			_roadTextureLayout.push_back( std::make_pair( dU, 1 )  );
			_roadTextureLayout.push_back( std::make_pair( dU, 0 )  );
			dU += dDist/10.0;
			_roadTextureLayout.push_back( std::make_pair( dU, 0 )  );
			_roadTextureLayout.push_back( std::make_pair( dU, 1 )  );

			boundary[0] = boundary[3];
			boundary[1] = boundary[2];
		}

		_boundingBox = MeshHelper::GetBoundingBox( _roadBoundaryLayout, VectorD(1,1,1) );

		_ModelRoadListID = glGenLists(1);
		glNewList( _ModelRoadListID, GL_COMPILE );
		glBegin(GL_QUADS);

			for( size_t i=0; i < _roadBoundaryLayout.size(); ++i )
			{
				glNormal3dv(&_roadBoundaryLayout[i].getNormal().x );
				glTexCoord2d( _roadTextureLayout[i*4].first, _roadTextureLayout[i*4].second );
				glVertex3dv( &_roadBoundaryLayout[i][0].x );

				glTexCoord2d( _roadTextureLayout[i*4+1].first, _roadTextureLayout[i*4+1].second );
				glVertex3dv( &_roadBoundaryLayout[i][1].x );

				glTexCoord2d( _roadTextureLayout[i*4+2].first, _roadTextureLayout[i*4+2].second );
				glVertex3dv( &_roadBoundaryLayout[i][2].x );

				glTexCoord2d( _roadTextureLayout[i*4+3].first, _roadTextureLayout[i*4+3].second );
				glVertex3dv( &_roadBoundaryLayout[i][3].x );
			}
		glEnd();
		glEndList();

		_ModelRoadLinesListID = glGenLists(1);
		glNewList( _ModelRoadLinesListID, GL_COMPILE );
		glBegin(GL_LINES);

			for( size_t i=0; i < _roadBoundaryLayout.size(); ++i )
			{
				for( int j=0; j < 2; ++j )
				{
					VectorD pt = _roadBoundaryLayout[i][j];
					VectorD pt2 = pt + _roadBoundaryLayout[i].getNormal();

					glVertex3dv( &pt.x );
					glVertex3dv( &pt2.x );
				}
			}

		glEnd();
		glEndList();
	}

	void renderMesh(float fLightingFraction, unsigned int camID, unsigned int shadowMapCount) override
	{
		if( _bshadow )
			return;

/*		if( !_boundingBox.InFrustrum( OpenGLPipeline<>::Get().GetFrustum() ) )
			return;*/

		T *pThis = static_cast<T*>(this);

		_fLightingFraction = fLightingFraction;
		glColor3f(_fLightingFraction,_fLightingFraction,_fLightingFraction);

		glEnable( GL_TEXTURE_2D );
		_road_texture.bind();
		glCallList( _ModelRoadListID );
		glDisable(GL_TEXTURE_2D );
		glCallList( _ModelRoadLinesListID );
	}

	GLuint _ModelRoadListID;
	GLuint _ModelRoadLinesListID;
	float _fLightingFraction;
	bool _bshadow;
	OpenGLTexture2D _road_texture;
	std::vector<QuadPlaneBoundaryT> _roadBoundaryLayout;
	std::vector<std::pair<double,double>> _roadTextureLayout;
	BoundingBox _boundingBox;
};

template<class T>
class RingRoadRigidBody : public GSRigidBody
{
public:
	RingRoadRigidBody (float fMass, const Matrix3x3D& inertia, JSONRigidBodyCollection *pParent);

    void update(double dt)
	{
		GSRigidBody::update(dt);
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
		JSONRigidBody *pJSONBody = pParent->createStaticJSONBody( this, JSONRigidBody::TYPE_PLANE|JSONRigidBody::TYPE_HAS_NO_SHADOW );
	}

	bool GetHeightFromPosition( const GPSLocation & position, HeightData &heightData ) const override
	{
		const RingRoadOpenGL<T> * pThis = static_cast<const T*>(this);

        Vector3F offsetPos = getGPSLocation().offSetTo( position );

		if( !pThis->_boundingBox.PointIsContained(offsetPos) )
			return false;

		std::vector<HeightData> planes;

		for( size_t i=0; i < pThis->_roadBoundaryLayout.size(); ++i )
		{
			if( pThis->_roadBoundaryLayout[i].PointIsContained( offsetPos ) )
			{
				HeightData pe;
				pe.setData( offsetPos, pThis->_roadBoundaryLayout[i].getPlane() );
				planes.push_back( pe );
			}
		}

		size_t idx = GetHeightIndexFromPlanes( planes );

		if( idx != -1 )
			heightData = planes[idx];

		return idx != -1;
	}
};

template<class T> 
RingRoadRigidBody<T>::RingRoadRigidBody(float fMass, const Matrix3x3D& inertia, JSONRigidBodyCollection *pParent)
{
	setMass( fMass );
	setInertiaMatrix( inertia );
	setCG(VectorD() );
}

//////////////////////////////////////////////////////////////////////////////////////////////

class RingRoad : 
	public RingRoadRigidBody<RingRoad>, 
	public RingRoadOpenGL<RingRoad>
{
public:
	RingRoad()
		: RingRoadRigidBody(100, Matrix3x3D(1,0,0,1,0,0,1,0,00),nullptr) 
	{ 
	}
};
