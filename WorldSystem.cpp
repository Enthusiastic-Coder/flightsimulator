#include "stdafx.h"
#include "interfaces.h"
#include "FlightRecorder.h"
#include "ForceGenerator.h"
#include "JSONRigidBodyCollection.h"
#include "JSONRigidBody.h"
#include "WorldSystem.h"

#include "MipMapTerrainMeshModelCollection.h"
#include "HeightData.h"

#include "OpenGLRenderer.h"
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QtLogging>
#include <algorithm>
#include <cmath>
#include <limits>

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
    _fLightYTanAngle(0.0f),
    _sceneryManager(this),
    _Joy(0),
    _simpleCloud(0),
    _cameraProvider(ViewPosition::ViewPositionEnd-1)
{
    _cameraProvider.nextView();//GO to first view
   _cameraProvider.getCameraView(FreeLooking)->setDescription("World:FreeLooking");
   _cameraProvider.getCameraView(FreeLooking)->setShakingMode(true);

   _cameraProvider.getCameraView(Free)->setDescription("World:Free");
   _cameraProvider.getCameraView(Free)->setShakingMode(false);

}

void WorldSystem::update( double dt )
{
    // Check key presses here?
    // -----------------------
    if( fabs(dt) <= std::numeric_limits<float>::epsilon())
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

    addStaticJSONBody(_simpleCloud=new SimpleCloud("cloud_A"));
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

            view->setOrientation( MathSupport<float>::MakeEulerFromLookAt(lookAt));

            CameraView* otherView = _cameraProvider.getCameraView(Free);
            otherView->setPosition(view->getPosition());
        }
        else if( pos == Free)
        {
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
    QuarternionF qF = MathSupport<float>::MakeQOrientation(Vector3F(pitch, hdg, 0));
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

void WorldSystem::loadHeathrowSurfaces(const GPSLocation& origin, float heading)
{
    QFile file(QStringLiteral("Scenery/Heathrow.json"));
    if (!file.open(QIODevice::ReadOnly))
    {
        qWarning("Could not open Heathrow scenery layout");
        return;
    }

    const auto layout = QJsonDocument::fromJson(file.readAll()).object();
    const auto bounds = layout.value(QStringLiteral("bounds")).toArray();
    if (bounds.size() != 4 || std::any_of(bounds.begin(), bounds.end(),
        [](const QJsonValue& value) { return !value.isDouble(); }))
    {
        qWarning("Invalid Heathrow scenery bounds");
        return;
    }

    const float westMin = bounds[0].toDouble();
    const float southMin = bounds[1].toDouble();
    const float length = bounds[2].toDouble() - westMin;
    const float width = bounds[3].toDouble() - southMin;
    if (length <= 0 || length > 5000 || width <= 0 || width > 3000)
    {
        qWarning("Invalid Heathrow scenery dimensions");
        return;
    }

    const auto rotation = MathSupport<float>::MakeQHeading(heading);
    const auto point = [&](float west, float south)
    {
        return QVRotate(rotation, Vector3F(-south, 0, -west));
    };
    const GPSLocation groundOrigin = origin + point(westMin, southMin + width);
    _TerrainCollection[0]->setGroundDetail("Textures/heathrow_ground.png", groundOrigin,
        origin + point(westMin, southMin), origin + point(westMin + length, southMin + width));

    auto taxiways = std::make_unique<TaxiwayMeshModel>();
    const auto routes = layout.value(QStringLiteral("routes")).toArray();
    for (const auto& route : routes)
    {
        std::vector<Vector3F> waypoints;
        for (const auto& entry : route.toArray())
        {
            const auto position = entry.toArray();
            if (position.size() != 2 || !position[0].isDouble() || !position[1].isDouble())
                continue;
            const float west = position[0].toDouble();
            const float south = position[1].toDouble();
            if (west < westMin || west > westMin + length || south < southMin || south > southMin + width)
                continue;
            const auto waypoint = point(west, south);
            if (waypoints.empty() || (waypoint - waypoints.back()).MagnitudeXZ() > 1.0f)
                waypoints.push_back(waypoint);
        }
        if (waypoints.size() < 2)
            continue;

        std::vector<Vector3F> centreline{waypoints.front()};
        const auto addStraight = [&](const Vector3F& end)
        {
            const auto start = centreline.back();
            const auto delta = end - start;
            const int steps = static_cast<int>(std::ceil(delta.MagnitudeXZ() / 12.0f));
            for (int step = 1; step <= steps; ++step)
                centreline.push_back(start + delta * (static_cast<float>(step) / steps));
        };
        for (std::size_t i = 1; i + 1 < waypoints.size(); ++i)
        {
            const auto& corner = waypoints[i];
            const auto incoming = waypoints[i - 1] - corner;
            const auto outgoing = waypoints[i + 1] - corner;
            const float cutback = std::min({55.0f, 0.35f * incoming.MagnitudeXZ(), 0.35f * outgoing.MagnitudeXZ()});
            const auto entry = corner + incoming.Unit() * cutback;
            const auto exit = corner + outgoing.Unit() * cutback;
            addStraight(entry);
            for (int step = 1; step <= 10; ++step)
            {
                const float t = step / 10.0f;
                const float u = 1.0f - t;
                centreline.push_back(entry * (u * u) + corner * (2 * u * t) + exit * (t * t));
            }
        }
        addStraight(waypoints.back());
        taxiways->addRoute(centreline, 28.0f);
    }
    if (taxiways->size() > 0)
    {
        _TerrainCollection[0]->attachModel(*taxiways, origin, 0.075f);
        _airportSurfaceMeshes.push_back(std::move(taxiways));
    }
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
    // Runway dimensions and eastern threshold positions from UK AIP EGLL AD 2.12.
    constexpr float runwayWidth = 50.0f;
    const auto addHeathrowRunway = [&](GPSLocation eastThreshold, float heading, float length,
                                      float westThresholdDisplacement, const char* texture)
    {
        auto obj = std::make_unique<RunwayMeshObject>();
        const auto rotation = MathSupport<float>::MakeQHeading(heading);
        obj->location = eastThreshold + QVRotate(rotation, Vector3F(-runwayWidth / 2, 0, 0));
        obj->meshModel.setTextureName(texture, GL_LINEAR_MIPMAP_LINEAR);
        obj->meshModel.Build(heading, runwayWidth, length, fRunwayMaxDiff, true);
        _runwayList.push_back(std::move(obj));
        _runwayLights.emplace_back(*_TerrainCollection[0], eastThreshold, heading, length, westThresholdDisplacement);
    };

    const GPSLocation northThreshold(51.477675, -0.433283);
    const GPSLocation southThreshold(51.464950, -0.434100);
    addHeathrowRunway(northThreshold, -90.29f, 3901, 309, "heathrow_09l_27r.png");
    addHeathrowRunway(southThreshold, -90.28f, 3658, 308, "heathrow_09r_27l.png");
    loadHeathrowSurfaces(northThreshold, -90.29f);

    // Simplified terminal-side parallels and exits, measured from each eastern threshold.
    const auto addHeathrowTaxiways = [&](GPSLocation threshold, float heading, float length, float offset)
    {
        constexpr float taxiwayWidth = 29.0f;
        const auto rotation = MathSupport<float>::MakeQHeading(heading);
        const auto point = [&](float lateral, float along)
        {
            return QVRotate(rotation, Vector3F(lateral, 0, -along));
        };
        const auto attach = [&](std::unique_ptr<TaxiwayMeshModel> mesh, float terrainOffset)
        {
            _TerrainCollection[0]->attachModel(*mesh, threshold, terrainOffset);
            _airportSurfaceMeshes.push_back(std::move(mesh));
        };

        auto parallel = std::make_unique<TaxiwayMeshModel>();
        std::vector<Vector3F> centreline;
        for (float along = 20.0f; along < length - 20.0f; along += 15.0f)
            centreline.push_back(point(offset, along));
        centreline.push_back(point(offset, length - 20.0f));
        parallel->addRoute(centreline, taxiwayWidth);
        attach(std::move(parallel), 0.08f);

        auto exits = std::make_unique<TaxiwayMeshModel>();
        const float side = offset < 0 ? -1.0f : 1.0f;
        const auto addExit = [&](float along, float sweep)
        {
            const auto start = point(side * (runwayWidth / 2 - 3.0f), along);
            const auto firstControl = point(side * 67.0f, along + sweep * 0.28f);
            const auto secondControl = point(offset, along + sweep * 0.65f);
            const auto end = point(offset, along + sweep);
            std::vector<Vector3F> curve;
            for (int i = 0; i <= 20; ++i)
            {
                const float t = i / 20.0f;
                const float u = 1.0f - t;
                curve.push_back(start * (u * u * u) + firstControl * (3 * u * u * t) +
                                secondControl * (3 * u * t * t) + end * (t * t * t));
            }
            exits->addRoute(curve, taxiwayWidth);
        };

        addExit(75, 70);
        addExit(length * 0.25f, 125);
        addExit(length * 0.49f, -125);
        addExit(length * 0.72f, 125);
        addExit(length - 75.0f, -70);
        attach(std::move(exits), 0.06f);
    };

    addHeathrowTaxiways(northThreshold, -90.29f, 3901, -142.0f);
    addHeathrowTaxiways(southThreshold, -90.28f, 3658, 125.0f);

    {

		std::unique_ptr<RunwayMeshObject> obj2(new RunwayMeshObject);
		obj2->location = GPSLocation(51.471866, -0.465477) +Vector3F(-5000/2, 0, 5000/2);
		obj2->meshModel.setTextureName("grass.jpg");
		obj2->meshModel.setTextureMag(20, 20);
		obj2->meshModel.Build(0, 5000, 5000, fRunwayMaxDiff);
		//_runwayList.push_back(std::move(obj2));

    }

#elif defined(LOCATED_AT_GIBRALTER)

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

void WorldSystem::persistReadState(const QJsonObject& doc)
{
    _fLightingFraction = doc.value(QStringLiteral("LightingFraction")).toDouble(_fLightingFraction);
    _rigidBodyCollection.persistReadState(doc);
    _cameraProvider.persistReadState(doc);
}

void WorldSystem::persistWriteState(QJsonObject& doc)
{
    doc.insert(QStringLiteral("LightingFraction"), _fLightingFraction);
    _rigidBodyCollection.persistWriteState(doc);
    _cameraProvider.persistWriteState(doc);
}

JSONRigidBody *WorldSystem::focusedRigidBody()
{
    return _rigidBodyCollection.focusedRigidBody();
}

void WorldSystem::prevFocusedRigidBody()
{
    _rigidBodyCollection.prevFocusedRigidBody();
}

void WorldSystem::nextFocusedRigidBody()
{
    _rigidBodyCollection.nextFocusedRigidBody();
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
    {
        glEnable(GL_BLEND);
        _TerrainCollection.RenderChildren(r, shadowMapCount, cameraPos);
        glDisable(GL_BLEND);
    }
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

void WorldSystem::renderRunwayLights(Renderer& renderer, const GPSLocation& camera, std::int64_t elapsedMs) const
{
    for (const auto& lights : _runwayLights)
        lights.render(renderer, camera, elapsedMs, _fLightingFraction);
}

void WorldSystem::attachModel(int idx, MeshModel & m, const GPSLocation & loc)
{
	_TerrainCollection[idx]->attachModel(m, loc, 0.1f);
}

void WorldSystem::addJSONBody(JSONRigidBody * pRigidBody)
{
    _rigidBodyCollection.addJSONBody(this, pRigidBody);
}

void WorldSystem::addStaticJSONBody(JSONRigidBody * pRigidBody, MeshModel* m)
{
    _rigidBodyCollection.addStaticJSONBody(this, pRigidBody, m);
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
