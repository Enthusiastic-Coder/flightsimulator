#pragma once

#include "GSRigidBody.h"
#include "PivotObjects.h"
#include "Hydraulics.h"
#include "Camera.h"
#include "interfaces.h"
#include <MassChannel.h>
#include "OpenGLRenderer.h"
#include "OpenGLPipeline.h"
#include "CameraViewProvider.h"
#include "WindTunnelForceGenerator.h"

#include <set>
#include <map>
#include <vector>

class MeshModel;

class JSONRigidBody :
	public GSRigidBody, 
    public IInputHandlerReceiver,
    public ISceneryEnvironment,
    public IMeshModel,
    public ICameraViewProvider,
    public ICameraChaseable,
    public IFilePersist,
    public IRapidJsonPersist
{
public:
	enum class Type : int
	{
		NORMAL,
		PLANE = 1,
		HAS_NO_SHADOW = 2,
	};

    enum ViewPosition
    {
        View_Delegate,
        CockpitForwards,
        PassengerLeftMiddle,
        RightGearWheel,
        Chase,
        ChaseRotate,
        ViewPositionEnd,
    };
	
	JSONRigidBody(std::string sName, JSONRigidBody::Type typeFlags = JSONRigidBody::Type::NORMAL );

	bool typeMask(Type t);
	std::string getID();
	std::string getName();
    std::string getPath();

	MeshModel* getMeshModel();
	const MassChannel& getMassChannel() const;

	void clear();
	void addForceGenerator(std::string sName, GSForceGenerator* pGenerator, int timeresolution = 0);

	Hydraulics& hydraulics();
	PivotObjects& pivots();

	GSForceGenerator* forceGenerator(const std::string& sName);
	std::map<std::string, GSForceGenerator*>& forceGeneratorMap();

	enum class STATE : short
	{
		NORMAL,
		RECORDING,
		PLAYBACK
	} _state;

	void reset();

	void setState(STATE state);
	STATE getState();

	void setShowState(bool bShow);
	bool getShowState() const;

	void saveRecorder(std::string sfilename);
	bool loadRecorder(std::string sfilename);

	JSONRigidBody::Type typeFlags() const;
	
	void setRecorderHook(FlightRecorder& a) override;

	FlightRecorder& getFlightRecorder();
	void onPlayRecording(double dt);
	void onForceGeneratorCheckActive();
	void onBeforeUpdate(double dt);
	void onUpdate(double dt);
	void onAfterUpdate(double dt);

    void renderForceGenerators(Renderer *r);

    void Render(Renderer *r, bool bReflection, unsigned int shadowMapCount);
    void drawWindTunnel(Renderer *args);

    virtual void onInitialise(WorldSystem *pWorldSystem);
	virtual bool isUsingMouse();
    virtual float getPower(int engine);
    virtual float getPowerOutput(int engine);
    virtual void setPower(float value);

    void setUsingMouse(bool value);
	GPSLocation getLastPosition();

/// ISceneryEnvironment
    bool getHeightFromPosition( const GPSLocation& position, HeightData & heightData ) const override;

/// IInputHandlerReceiver
    bool onAsyncKeyPress(IScreenMouseInfo* scrn, float dt) override;
    bool onSyncKeyPress() override;
    bool onMouseMove( int x, int y) override;
    bool onMouseWheel(int wheelDelta) override;

/// ICameraPositionProvider
    bool nextView() override;
    bool prevView() override;
    int curViewIdx() const override;
    CameraView* getCameraView() override;
    void updateCameraView() override;
    std::string getCameraDescription() const override;

    virtual void airResetPos();
    virtual void airResetApproachPos();
    virtual void airSpoilerToggle(bool bLeft);
    virtual void airFlapIncr(int incr);

    void startRecording();
    void togglePlayback();

// ICameraChaseable
    void incrChaseAngle(float fDiff) override;
    void incrChaseDistance(float fDiff) override;
    void incrChaseHeight(float fDiff) override;

// IFilePersist
    void persistReadState(FILE* fPersistFile ) override;
    void persistWriteState(FILE* fPersistFile ) override;

// IRapidJsonPersist
    void persistReadState(rapidjson::Document &doc) override;
    void persistWriteState(rapidjson::Document& doc) override;

protected:
/// IMeshModel
    void renderMesh(Renderer *r, unsigned int shadowMapCount) override;

	MeshModel* loadMeshModel(std::string sMeshName, MassChannel &mc);

	void onUpdateFinal(double dt);
	void onForceGeneratorAction( void (GSForceGenerator::*pActor)(Particle*,double), double dt );
	void onUpdateHelper(std::map<int, std::vector<GSForceGenerator*>>::iterator &it, double dt );
	void applyModelMatrix(OpenGLMatrixStack& mv);
	
	void buildMapList();
	void push();
	void pop();

    CameraViewProvider *getCameraProvider();

private:
    MeshModel* _pMeshModel;
	MassChannel _massChannel;
	std::string _sName;
	std::string _sID;
	Hydraulics _hydraulics;
	PivotObjects _pivots;    
    WindTunnelForceGenerator _windTunnel;
	bool _bShow;
    bool _bDrivingByMouse;
	Type _typeFlags;
    Vector3D _totalForceCache;
    Vector3D _totalTorqueCache;
	std::vector<GSForceGenerator::PivotObject*> _mesh_pivot_vector;
	std::map<std::string,GSForceGenerator*>  _force_generator_map;
	std::vector<GSForceGenerator*> _endList;
	std::set<std::pair<int,GSForceGenerator*>> _setList;
	std::map<int, std::vector<GSForceGenerator*>> _cookedMapList;
    std::stack<std::pair<Vector3D,Vector3D>> _stkForceTorque;
	FlightRecorder _flightRecorder;
    Vector3D _euler;
	GPSLocation _lastGPSPosition;
    CameraViewProvider _cameraPositionProvider;
    double _chaseAngle;
    double _chaseHeight;
    double _chaseDistance;

};

