#pragma once

#include "SceneryManager.h"
#include "JSONRigidBodyCollection.h"
#include "Interfaces.h"
#include "Camera.h"
#include "Weather.h"
#include "CPPSourceCodeMeshModel.h"
#include "MipMapTerrainMeshModelCollection.h"
#include "CameraViewProvider.h"

struct RunwayMeshObject
{
	GPSLocation location;
	SimpleRunwayMeshModel meshModel;
};

class SimpleCloud;

class WorldSystem :
        public IFilePersist,
        public IRapidJsonPersist,
        public IInputHandlerReceiver,
        public ISceneryEnvironment,
        public ICameraViewProvider,
        public ICameraChaseable
{
public:

    enum ViewPosition
    {
        View_Delegate,
        Free,
        FreeLooking,
        ViewPositionEnd,
    };

    WorldSystem();

	void onInitialise();
	void loadTerrain();
	void onUnintialise();

	double GetGravityFromPosition(const GPSLocation& location);
    double GetGravityFromPosition(const Vector3D& location);

    void update( double dt );

	ISceneryEnvironment * getEnv();
	IWeather * getWeather();
	IJoystick * getJoystick();
	
	void loadBodyRecorderedData();
    JSONRigidBody* focusedRigidBody();

    void prevFocusedRigidBody();
    void nextFocusedRigidBody();

	void setLightFraction(float f);
    void incrLightFraction(float incr);

	float getLightFraction();
	float getLightTanAngle();

    void setLightDirection(float pitch, float hdg);
    Vector3F getLightDirection();
    Vector3F getEyeLightDirection();

    bool rigidBodyToggleUsingMouse();
    bool isUsingMouse();

    void RenderTerrain(Renderer *r, bool bLand, bool bReflection, unsigned int shadowMapCount, GPSLocation cameraPos);
    void RenderModels(Renderer *r, bool bReflection, unsigned int shadowMapCount, double dt);
    void RenderModelForceGenerators(Renderer *r);

	MipMapTerrainMeshModelCollection _TerrainCollection;

	void attachModel(int idx, MeshModel& m, const GPSLocation& loc);

	void addJSONBody(JSONRigidBody* pRigidBody);
	void addStaticJSONBody(JSONRigidBody* pRigidBody, MeshModel* model=nullptr);

// IFilePersist
    void persistReadState(FILE* fPersistFile) override;
    void persistWriteState(FILE* fPersistFile) override;

// IRapidJsonPersist
    void persistReadState(rapidjson::Document& doc) override;
    void persistWriteState(rapidjson::Document& doc) override;

// ISceneryEnvironment
    bool getHeightFromPosition(const GPSLocation& position, HeightData& heightData) const override;

// IInputHandlerReceiver
    bool onAsyncKeyPress(IScreenMouseInfo* scrn, float dt) override;
    bool onSyncKeyPress() override;
    bool onMouseMove( int x, int y) override;
    bool onMouseWheel(int wheelDelta) override;

// ICameraPositionProvider
    bool nextView() override;
    bool prevView() override;
    int curViewIdx() const override;
    void updateCameraView() override;
    CameraView* getCameraView() override;
    std::string getCameraDescription() const override;

// ICameraChaseable
    void incrChaseAngle(float fDiff) override;
    void incrChaseDistance(float fDiff) override;
    void incrChaseHeight(float fDiff) override;

private:
	float _fLightingFraction;
    Vector3F _fLightDirection;
    Vector3F _eyeLightDirection;
	float _fLightYTanAngle;
	SceneryManager _sceneryManager;
	JSONRigidBodyCollection _rigidBodyCollection;
	Weather _weather;
    IJoystick* _Joy;

    std::vector<std::unique_ptr<RunwayMeshObject>> _runwayList;
	GPSLocation _simplePlaneLocation;
	SimpleCloud* _simpleCloud;

    CameraViewProvider _cameraProvider;
};
