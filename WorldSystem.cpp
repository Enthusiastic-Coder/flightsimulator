#include "stdafx.h"
#include "interfaces.h"
#include "FlightRecorder.h"
#include "ForceGenerator.h"
#include "JSONRigidBodyCollection.h"
#include "JSONRigidBody.h"
#include "WorldSystem.h"

#include "MipMapTerrainMeshModelCollection.h"

#include "OpenGLRenderer.h"

#define TOWTRUCK_MASS_PROP	 7000, \
	Matrix3x3D(78523.6,      1708.93,         -28344.3, \
				1708.93,      96271.1,         3334.25, \
				-28344.3,     3334.25,         21440.1 )

#define TOWBAR_MASS_PROP	 1000, \
	Matrix3x3D(3000,      0,         0, \
	0,      3000,         0, \
	0,     0,         500)

#define HARRIER_MASS_PROP	 1000, \
	Matrix3x3D(3000,      0,         0, \
	0,      3000,         0, \
	0,     0,         500)



////////////////////////////////////////////////////////


#include "Win32JoystickSystem.h"

#include "SimpleCloud.h"


WorldSystem::WorldSystem() :
    _fLightingFraction(0.85f),
    _sceneryManager(this),
    _simpleCloud(0),
    _Joy(0),
    _fLightYTanAngle(0.0f),
    _cameraProvider(ViewPosition::ViewPositionEnd-1)
{
    _cameraProvider.nextView();//GO to first view
}

void WorldSystem::update( double dt )
{
    // Check key presses here?
    // -----------------------
    if( fabs(dt) <= FLT_EPSILON)
        return;

    _Joy->joyUpdate();
    _rigidBodyCollection.update( dt );

    GPSLocation loc = _cameraProvider.getCameraView()->getPosition();

    if (_simpleCloud != 0)
        _simpleCloud->setPosition(loc + Vector3F(0,-loc._height,0));
    //_weather.update( dt );

    updateCameraView();
}

bool WorldSystem::onAsyncKeyPress(IScreenMouseInfo *scrn, float dt)
{
    return _rigidBodyCollection.onAsyncKeyPress(scrn, dt);
}

void WorldSystem::onInitialise()
{
    loadTerrain();
    _rigidBodyCollection.onInitialise();
    _Joy = new Win32JoystickSystem;
    _Joy->joyInit(0);
}

void WorldSystem::onUnintialise()
{
    _rigidBodyCollection.onUnInitialise();
}

bool WorldSystem::onSyncKeyPress()
{
    return _rigidBodyCollection.onSyncKeyPress();
}

bool WorldSystem::rigidBodyToggleUsingMouse()
{
    return _rigidBodyCollection.toggleUsingMouse();
}

bool WorldSystem::onMouseMove( int x, int y)
{
    return _rigidBodyCollection.onMouseMove( x, y );
}

bool WorldSystem::onMouseWheel( int wheelDelta)
{
    return _rigidBodyCollection.onMouseWheel( wheelDelta );
}

bool WorldSystem::nextView()
{
    if( _rigidBodyCollection.curViewIdx())
    {
        if( !_rigidBodyCollection.nextView())
            while( !_cameraProvider.nextView());//Force World Camera
    }
    else
    {
        if( !_cameraProvider.nextView())
            while( !_rigidBodyCollection.nextView());//Force RigidBody Camera
    }

    return true;
}

bool WorldSystem::prevView()
{
    if( _rigidBodyCollection.curViewIdx())
    {
        if( !_rigidBodyCollection.prevView())
            while( !_cameraProvider.prevView());//Force World Camera
    }
    else
    {
        if( !_cameraProvider.prevView())
            while( !_rigidBodyCollection.prevView());//Force RigidBody Camera
    }

    return true;
}

int WorldSystem::curViewIdx() const
{
    return _cameraProvider.curViewIdx() + _rigidBodyCollection.curViewIdx();
}

void WorldSystem::incrChaseAngle(float fDiff)
{
    if( _rigidBodyCollection.curViewIdx() != 0)
        _rigidBodyCollection.incrChaseAngle(fDiff);
}

void WorldSystem::incrChaseDistance(float fDiff)
{
    if( _rigidBodyCollection.curViewIdx() != 0)
        _rigidBodyCollection.incrChaseDistance(fDiff);
}

void WorldSystem::incrChaseHeight(float fDiff)
{
    if( _rigidBodyCollection.curViewIdx() != 0)
        _rigidBodyCollection.incrChaseHeight(fDiff);
}

void WorldSystem::updateCameraView()
{
    if( _rigidBodyCollection.curViewIdx() != 0)
        _rigidBodyCollection.updateCameraView();
    else
    {
        ViewPosition pos = (ViewPosition) _cameraProvider.curViewIdx();
        JSONRigidBody* pBody = focusedRigidBody();
        CameraView* view = _cameraProvider.getCameraView();

        view->setSmoothMode(true);

        if( pos == View_Delegate)
        {
            _cameraProvider.nextView();
            pos = (ViewPosition)_cameraProvider.curViewIdx();
        }

        if( pBody != 0 && pos == FreeLooking )
        {
            Vector3F lookAt = view->getPosition().offSetTo(pBody->getGPSLocation() + pBody->cg());

            view->setShakingMode(true);
            view->setOrientation( MathSupport<float>::MakeEulerFromLookAt(lookAt));
            view->setDescription("World:FreeLooking");

            CameraView* otherView = _cameraProvider.getCameraView(Free);
            otherView->setPosition(view->getPosition());
        }
        else if( pos == Free)
        {
            view->setDescription("World:Free");
            view->setShakingMode(false);

            CameraView* otherView = _cameraProvider.getCameraView(FreeLooking);
            otherView->setPosition(view->getPosition());
        }
    }
}

CameraView *WorldSystem::getCameraView()
{
    if( _rigidBodyCollection.curViewIdx() != 0)
        return _rigidBodyCollection.getCameraView();
    else
        return _cameraProvider.getCameraView();
}

std::string WorldSystem::getCameraDescription() const
{
    if( _rigidBodyCollection.curViewIdx() != 0)
        return _rigidBodyCollection.getCameraDescription();
    else
        return _cameraProvider.getCameraDescription();
}

void WorldSystem::setLightDirection(float pitch, float hdg)
{
    QuarternionF qF = MathSupport<float>::MakeQ(pitch, hdg, 0);
    _fLightDirection = QVRotate(qF, Vector3F(0, 0, -1));
    _eyeLightDirection = OpenGLPipeline::Get(0).GetNormal() * _fLightDirection;
    _fLightYTanAngle = sqrt(_eyeLightDirection.x *_eyeLightDirection.x +
                            _eyeLightDirection.z * _eyeLightDirection.z) / _eyeLightDirection.y;
}

Vector3F WorldSystem::getLightDirection()
{
    return _fLightDirection;
}

Vector3F WorldSystem::getEyeLightDirection()
{
    return _eyeLightDirection;
}

void WorldSystem::loadTerrain()
{
	_TerrainCollection.OnInitialise();

	std::string rootDir(OUTPUT_SRTM3_ROOT_FOLDER_CACHE);
	std::string rootCacheDir(OUTPUT_SRTM3_ROOT_FOLDER_CACHE);

    float fRunwayMaxDiff(5.0f*2*2);

	//int width = 60;
	//int height = 2735;

	int width = 380;
	int height = 3200;

#ifdef LOCATED_AT_LONDON

    if (!_TerrainCollection.LoadTile(rootCacheDir + "n51w001.data"))
        if (_TerrainCollection.LoadSRTM30("n51w001"))
        {
            _TerrainCollection.BuildLastTile();
            _TerrainCollection.SaveLastTile(rootCacheDir + "n51w001.data");
        }

    fRunwayMaxDiff = 20.0f;
    {
        //27R EGLL
        std::unique_ptr<RunwayMeshObject> obj(new RunwayMeshObject);
        obj->location = GPSLocation(51.477681, -0.433258);
        obj->meshModel.setTextureName("runway_strip.png");
        obj->meshModel.Build(-90, 60, 2664, fRunwayMaxDiff);
        _runwayList.push_back(std::move(obj));
    }

    {
        //27L
        std::unique_ptr<RunwayMeshObject> obj(new RunwayMeshObject);
        obj->location = GPSLocation(51.464951, -0.434115);
        obj->meshModel.setTextureName("runway_strip.png");
        obj->meshModel.Build(-90, 60, 2664, fRunwayMaxDiff);

        _runwayList.push_back(std::move(obj));
    }

#elif LOCATED_AT_GIBRALTER

	// = GPSLocation(36.151149, -5.349627) + VectorF(0, 0, 0);
	//_simplePlaneMeshModel.Build(250, 250, 4.0f, RGB(50, 50, 0), 0);

	/*{
		std::unique_ptr<RunwayMeshObject> obj(new RunwayMeshObject);
		obj->location = GPSLocation(36.151149, -5.349627) + VectorF(0, 0, 500) + VectorF(0, 0, -4000) + VectorF(0, 0, 4000);
		obj->meshModel.setTextureName("runway_strip.png");
		obj->meshModel.Build(140, width, height, fRunwayMaxDiff);
		_runwayList.push_back(std::move(obj));
	}*/

	{
		std::unique_ptr<RunwayMeshObject> obj(new RunwayMeshObject);
        obj->location = GPSLocation(36.151149, -5.349627) + Vector3F(1000, 0, -3500);
		obj->meshModel.setTextureName("airport_layout.png");
		obj->meshModel.Build(-90, width, height, fRunwayMaxDiff);
		_runwayList.push_back(std::move(obj));
		//here
	}

	/*{
		std::unique_ptr<RunwayMeshObject> obj(new RunwayMeshObject);
		obj->location = GPSLocation(36.151473, -5.339343);
		obj->meshModel.setTextureName("runway_strip.png");
		obj->meshModel.Build(-90 - 45 - 15, width, height, fRunwayMaxDiff);
		_runwayList.push_back(std::move(obj));
	}*/

	{
		std::unique_ptr<RunwayMeshObject> obj(new RunwayMeshObject);
        obj->location = GPSLocation(36.151473, -5.339343)  + Vector3F(290,0,35);
		obj->meshModel.setTextureName("airport_layout.png");
		obj->meshModel.Build(-90, width, height, fRunwayMaxDiff);
		_runwayList.push_back(std::move(obj));
	}

    if( !_TerrainCollection.LoadTile( rootCacheDir + "n36w006.data" ) )
        if (_TerrainCollection.LoadSRTM30("n36w006"))
        {
            _TerrainCollection.BuildLastTile();
            _TerrainCollection.SaveLastTile( rootCacheDir + "n36w006.data" );
        }

    //PUT Back in

    if( !_TerrainCollection.LoadTile( rootCacheDir + "n35w006.data" ) )
        if (_TerrainCollection.LoadSRTM30("n35w006"))
        {
            _TerrainCollection.BuildLastTile();
            _TerrainCollection.SaveLastTile( rootCacheDir + "n35w006.data" );
        }

#else

	/*if( !_TerrainCollection.LoadTile( rootCacheDir + "n50w001.data" ) )
	if( _TerrainCollection.LoadSRTM30( "n50w001" ) )
	{
	_TerrainCollection.Build();
	_TerrainCollection.SaveLastTile( rootCacheDir + "n50w001.data" );
	}*/

	if (!_TerrainCollection.LoadTile(rootCacheDir + "n51w001.data"))
		if (_TerrainCollection.LoadSRTM30("n51w001"))
		{
            _TerrainCollection.BuildLastTile();
			_TerrainCollection.SaveLastTile(rootCacheDir + "n51w001.data");
		}

	fRunwayMaxDiff = 20.0f;
	{
		//27R EGLL
		std::unique_ptr<RunwayMeshObject> obj(new RunwayMeshObject);
		obj->location = GPSLocation(51.477681, -0.433258);
		obj->meshModel.Build(-90, 60 + 30, 3600 * 0 + 2735, fRunwayMaxDiff);
		_runwayList.push_back(std::move(obj));
	}

	{
		//27L
		std::unique_ptr<RunwayMeshObject> obj(new RunwayMeshObject);
		obj->location = GPSLocation(51.464951, -0.434115);
		obj->meshModel.Build(-90, 60 + 25, 3328 * 0 + 2735, fRunwayMaxDiff);
		_runwayList.push_back(std::move(obj));
	}
#endif

    for (auto& it : _runwayList)
		_TerrainCollection[0]->attachModel(it->meshModel, it->location, 0.1f);

	_TerrainCollection.BuildVertexBuffers();
}

double WorldSystem::GetGravityFromPosition(const GPSLocation& location)
{
	return GetGravityFromPosition(location.position());
}

double WorldSystem::GetGravityFromPosition(const Vector3D& location)
{
	return 9.80665;
}

bool WorldSystem::getHeightFromPosition(const GPSLocation& position, HeightData& heightData) const
{
	std::vector<HeightData> planes;

	if (_TerrainCollection.getHeightFromPosition(position, heightData))
		planes.push_back(heightData);

	if (_rigidBodyCollection.getHeightFromPosition(position, heightData))
		planes.push_back(heightData);

	if (_sceneryManager.getHeightFromPosition(position, heightData))
		planes.push_back(heightData);

	size_t idx = GetHeightIndexFromPlanes(planes);

	if (idx == -1)
		heightData.setData(position._height);
	else
		heightData = planes[idx];

	return true;
}

void WorldSystem::loadBodyRecorderedData()
{
	_rigidBodyCollection.loadBodyRecorderedData();
}

void WorldSystem::persistReadState(FILE* fPersistFile)
{
	fread(&_fLightingFraction, sizeof(_fLightingFraction), 1, fPersistFile);
	_rigidBodyCollection.persistReadState(fPersistFile);
    _cameraProvider.persistReadState(fPersistFile);
}

void WorldSystem::persistWriteState(FILE* fPersistFile)
{
	fwrite(&_fLightingFraction, sizeof(_fLightingFraction), 1, fPersistFile);
	_rigidBodyCollection.persistWriteState(fPersistFile);
    _cameraProvider.persistWriteState(fPersistFile);
}

JSONRigidBody *WorldSystem::focusedRigidBody()
{
    return _rigidBodyCollection.focusedRigidBody();
}

void WorldSystem::setLightFraction(float f)
{
    _fLightingFraction = f;
}

void WorldSystem::incrLightFraction(float incr)
{
    _fLightingFraction += incr;

    if( _fLightingFraction < 0.05f )
        _fLightingFraction = 0.05f;

    if( _fLightingFraction > 0.99f )
        _fLightingFraction = 0.99f;
}

float WorldSystem::getLightFraction()
{
	return _fLightingFraction;
}

float WorldSystem::getLightTanAngle()
{
	return _fLightYTanAngle;
}

bool WorldSystem::isUsingMouse()
{
	if (focusedRigidBody() == nullptr)
		return false;

	return focusedRigidBody()->isUsingMouse();
}

void WorldSystem::RenderTerrain(Renderer* r, bool bLand, bool bReflection, unsigned int shadowMapCount, GPSLocation cameraPos)
{
    _TerrainCollection.Render(r, bLand, bReflection, shadowMapCount, cameraPos);

	if(bLand)	
        _TerrainCollection.RenderChildren(r, shadowMapCount, cameraPos);
}

void WorldSystem::RenderModels(Renderer* r, bool bReflection, unsigned int shadowMapCount, double dt)
{
    r->fLightingFraction = _fLightingFraction;

    _rigidBodyCollection.Render(r, bReflection, shadowMapCount, dt);
}

void WorldSystem::RenderModelForceGenerators(Renderer* r)
{
    _rigidBodyCollection.RenderForceGenerators( r);
}

void WorldSystem::addJSONBody(JSONRigidBody * pRigidBody)
{
    _rigidBodyCollection.addJSONBody(this, pRigidBody);
}

void WorldSystem::addStaticJSONBody(JSONRigidBody * pRigidBody)
{
    _rigidBodyCollection.addStaticJSONBody(this, pRigidBody);
}

ISceneryEnvironment *  WorldSystem::getEnv()
{
	return &_sceneryManager;
}

IWeather *  WorldSystem::getWeather()
{
	return &_weather;
}

IJoystick *  WorldSystem::getJoystick()
{
    return _Joy;
}
