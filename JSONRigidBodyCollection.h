#pragma once

#include "JSONRigidBody.h"
#include "CameraViewProvider.h"

#include <list>
class Renderer;

class JSONRigidBodyCollection :
        public IFilePersist,
        public IRapidJsonPersist,
        public IInputHandlerReceiver,
        public ISceneryEnvironment,
        public ICameraViewProvider,
        public ICameraChaseable
{
public:
    JSONRigidBodyCollection();
	~JSONRigidBodyCollection();

    void loadBodyRecorderedData();

	void onInitialise();
    void onUnInitialise();
    void addJSONBody(WorldSystem *pWorldSystem, JSONRigidBody* pRigidBody );
    void addStaticJSONBody( WorldSystem *pWorldSystem, JSONRigidBody* pRigidBody, MeshModel* model=nullptr );

    JSONRigidBody* createJSONBody(WorldSystem *pWorldSystem, std::string sName, JSONRigidBody::Type typeFlags = JSONRigidBody::Type::NORMAL);
    JSONRigidBody* createStaticJSONBody( WorldSystem *pWorldSystem, std::string sName, JSONRigidBody::Type typeFlags = JSONRigidBody::Type::NORMAL);

	void clear();
	size_t bodyCount();
	size_t staticBodyCount();

	void update( double dt );
    void Render(Renderer *args, bool bReflection, unsigned int shadowMapCount, double dt);
    void RenderForceGenerators(Renderer *r);

    const JSONRigidBody* activeRigidBody() const;
    JSONRigidBody* activeRigidBody();
    JSONRigidBody* focusedRigidBody();
    const JSONRigidBody* focusedRigidBody() const;
    void nextFocusedRigidBody();
    void prevFocusedRigidBody();

// IFilePersist
    void persistReadState(FILE* fPersistFile) override;
    void persistWriteState(FILE* fPersistFile) override;

// IRapidJsonPersist
    void persistReadState(rapidjson::Document &doc) override;
    void persistWriteState(rapidjson::Document& doc) override;

// ISceneryEnvironment
    bool getHeightFromPosition( const GPSLocation&, HeightData& ) const override;

// IInputHandlerReceiver
    bool onAsyncKeyPress(IScreenMouseInfo *scrn, float dt) override;
    bool onSyncKeyPress() override;
    bool toggleUsingMouse();
    bool onMouseMove( int x, int y ) override;
    bool onMouseWheel(int wheelDelta) override;

// CameraPositionProvider
    bool nextView() override;
    bool prevView() override;
    int curViewIdx() const override;
    void updateCameraView() override;
    CameraView *getCameraView() override;
    std::string getCameraDescription() const override;

// ICameraChaseable
    void incrChaseAngle(float fDiff) override;
    void incrChaseDistance(float fDiff) override;
    void incrChaseHeight(float fDiff) override;

protected:
    void unLoadAll();

private:
	std::list<JSONRigidBody*>::iterator _focusRigidBody;
	std::list<JSONRigidBody*> _bodyList;
	std::list<JSONRigidBody*> _staticBodyList;
};


