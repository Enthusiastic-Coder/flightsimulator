#pragma once

struct ICurvableOpenGLModel
{
    virtual void DoCurve() = 0;
};


class Renderer;
struct IMeshModel
{
    virtual void renderMesh(Renderer*, unsigned int shadowMapCount) = 0;
};

;
class GSRigidBody;
struct IForceGeneratorDrawable
{
    virtual void drawForceGenerator(GSRigidBody*, Renderer*) = 0;
};


class HeightData;
class GPSLocation;
template<typename> class Vector3;
typedef Vector3<double> Vector3D;
typedef Vector3<float> Vector3F;

struct ISceneryEnvironment
{
    virtual bool getHeightFromPosition( const GPSLocation& gpsLocation, HeightData& heightData ) const =0;
};

struct IWeather
{
    virtual Vector3D getWindFromPosition( const Vector3D& bodyOffset ) =0;
};

struct IJoystick
{
    virtual bool isAvailable() = 0;
    virtual void joyInit(char* id) = 0;
    virtual void joyUpdate() = 0;
    virtual float joyGetX() = 0;
    virtual float joyGetY() = 0;
    virtual float joyGetZ() = 0;
    virtual float joyGetR() = 0;
    virtual float joyGetU() = 0;
    virtual float joyGetV() = 0;
    virtual int getPOV() = 0;
    virtual int numberofButtonsPressed() = 0;
    virtual int buttonFlagPressed() =0;
};

struct IWindTunnelDrawable
{
    virtual void drawWindTunnel(double dt) =0 ;
};

struct IScreenMouseInfo
{
    virtual void GetMousePos(int& x, int& y) const = 0;
    virtual void GetScreenDims(int& w, int &h) const =0;
    virtual void GetScreenPos(int& x, int &y) const = 0;
};

struct IInputHandlerReceiver
{
    virtual bool onAsyncKeyPress(IScreenMouseInfo* screen, float dt) = 0;
    virtual bool onSyncKeyPress() = 0;
    virtual bool onMouseMove( int x, int ys) = 0;
    virtual bool onMouseWheel(int wheelDelta) = 0;
};

#include <string>

struct ICameraPositionSwitcher
{
    virtual bool incrView() = 0;
    virtual bool decrView() = 0;
    virtual int curViewIdx() const = 0;
};

class CameraView;
struct ICameraViewProvider
{
    virtual bool nextView() = 0;
    virtual bool prevView() = 0;
    virtual int curViewIdx() const = 0;
    virtual void updateCameraView() = 0;
    virtual CameraView* getCameraView() = 0;
    virtual std::string getCameraDescription() const = 0;
};

struct ICameraChaseable
{
    virtual void incrChaseAngle(float fDiff) = 0;
    virtual void incrChaseDistance(float fDiff) = 0;
    virtual void incrChaseHeight(float fDiff) = 0;

};

struct IFilePersist
{
    virtual void persistReadState(FILE* fPersistFile) = 0;
    virtual void persistWriteState(FILE* fPersistFile) = 0;
};

#include <rapidjson/document.h>

struct IRapidJsonPersist
{
    virtual void persistReadState(rapidjson::Document& doc) = 0;
    virtual void persistWriteState(rapidjson::Document& doc) = 0;
};
