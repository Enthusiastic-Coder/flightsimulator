#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QOpenGLWindow>
#include <QTimer>
#include <QElapsedTimer>
#include "include_gl.h"
//
//#include "RunwayLites.h"
//#include "LondonGroundTile.h"
//#include "WaterTile.h"
//#include "SimpleCloud.h"
#include "PFDView.h"
#include "PerlinNoise.h"
//#include "FlatLifterRigidBody.h"
//#include "RingRoad.h"
//#include "PiccadillyLineTrainARigidBody.h"
//#include "AudiA8RigidBody.h"
//#include "TowTruckRigidBody.h"
//#include "TowBarRigidBody.h"
//#include "HeathrowRadarRigidBody.h"
//#include "HeathrowTower.h"
#include "AirProperties.h"
#include "BA320RigidBody.h"
#include "HeathrowTower.h"
#include "BA320RigidBody.h"
#include "BA380RigidBody.h"
#include "SimpleCloud.h"
#include "HarrierRigidBody.h"
#include "AircraftCarrierRigidBody.h"


#include "WorldSystem.h"
#include "OpenGLShaderProgram.h"
#include "OpenGLFrameBuffer.h"
#include "OpenGLRenderBuffer.h"
#include "MeshModel.h"
#include "SimpleSpringModel.h"
#include <shellapi.h>

class MainWindow : public QOpenGLWindow
{
    Q_OBJECT

public:
    explicit MainWindow();
    ~MainWindow();

    const char* persistFilename() const;

protected:
    void setFramePerSecond( double fps);
    void setRunning( bool bRunning );
    bool isRunning();
    double frameTime();

    void OnInitialiseLighting();
    void onUnInitialise();
    void OnInitSound();
    void OnUnitSound();
    void OnInitPolyMode();
    bool onCreateFrameBufferAndShaders();

    void setupCameraOrientation();
    void setupCameraPosition(bool bReflection=false);

    void onNextFrame(double dt);

    void RenderScene();
    void RenderReflection();
    void RenderDepthTextures(int camID, OpenGLTexture2D &shadowMap, OpenGLTexture2D& colorMap, int offset, int size);
    void RenderMouseFlying(float cx, float cy);
    void RenderDrivingPower(float cx, float cy);
    void RenderFPS();
    void RenderInfo();
    void RenderTexture(GLuint texID, int pos);
    void RenderSlopes();
    void RenderLeftSlope();
    void RenderRightSlope();

    void OnRender();
    void OnRenderSkyBox();
    void ensureCameraAboveGround();
    void sendDataToShader(GLuint shader, int slot, int width, int height);

protected:
    bool event(QEvent *) Q_DECL_OVERRIDE;
    void initializeGL() Q_DECL_OVERRIDE;
    void resizeGL(int w, int h) Q_DECL_OVERRIDE;
    void paintGL() Q_DECL_OVERRIDE;
    void keyPressEvent(QKeyEvent *) Q_DECL_OVERRIDE;
    void wheelEvent(QWheelEvent *) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *) Q_DECL_OVERRIDE;

private slots:
    void onTimeOut();

private:
    QTimer _timer;
    QElapsedTimer _eTimer;
    qint64 _lastTime = 0;

private:
    double _fInverseFPS;
    double _dt;
    bool _running;

    bool _showCursor;
    //bool _paused;
    bool _bUserPolygonLineView;
    double _framerate[FPS_RESOLUTION];
    int _framecount;
    OGLFont _oglFont;

    PFDView _pfdInstrument;
    WorldSystem _WorldSystem;
    SimpleSpringModel _towTruckSpring;
    //TowTruck _towTruck;
    //TowBar _towBar;
    //HeathrowRadar _heathrowRadar;
    //HeathrowTower _heathrowTower;
    //RunwaySurface _runwayscene;
    //RunwayLites _runwaylites;
    //RunwayLites _runwaylites2;
    //LondonTile _londonTile;
    //WaterTile _waterTile;
    //SimpleCloud _simpleCloud;
    //AircraftCarrier _aircraftCarrier;
    //FlatLifter _flatLifter;
    //AudiA8 _audiA8;
    //CrossRoads _crossRoads;
    //RingRoad _ringRoad;
    //PiccadillyLineTrainA _trainA;
    //HarrierAircaft _harrierJumpJet;
    GPSLocation _Runway_Location;
    OpenGLTexture2D _runway1;
    OpenGLTexture2D _runway2;
    OpenGLTexture2D _slopeGrass;

    ////////////////////////////////
    double _chaseAngle;
    double _chaseHeight;
    double _chaseDistance;
    //Camera::Manager _cameraManager;
    Camera _camera;
    Camera::ViewPosition _view;
    //////////////////////////////////

    ALCdevice* _soundDevice;
    ALCcontext* _soundContext;

//// Sound Sources
    ALuint _soundEnvironmentSource;
    ALuint _soundEngineSource;
    ALuint _soundHeightCallOutSource;
    ALuint _soundTouchDownSource;
///////////////////////////

//// Sound Buffers
    ALuint _soundExternalEngineBuffer;
    ALuint _soundInternalEngineBuffer;
    ALuint _soundEnvironmentBuffer;
    ALuint _soundTouchDownBuffer;
    std::map<int,ALuint> _soundHeightCallOutBuffers;
///////////////////////////////////////////

    OpenGLShaderProgram _simpleShaderProgram;
    OpenGLShaderProgram _shadowShaderProgram;
    OpenGLShaderProgram _reflectionShaderProgram;
    OpenGLShaderProgram _waterShaderProgram;
    OpenGLFrameBuffer _openGLFrameBuffer;

    OpenGLTexture2D _shadowMap1;
    OpenGLTexture2D _shadowMap2;
    OpenGLTexture2D _shadowMap3;

    OpenGLTexture2D _shadowMapTexture1;
    OpenGLTexture2D _shadowMapTexture2;
    OpenGLTexture2D _shadowMapTexture3;

    OpenGLTexture2D _dUdVMap;
    OpenGLTexture2D _normalMap;
    float _nUVOffset = 0.0f;
    float g_WaterFlow = 0.02f;

    OpenGLTexture2D _reflectionTexture;
    OpenGLRenderBuffer _depthRenderBuffer;
    Vector4F _skyColor = Vector4F(3 / 255.0f, 100 / 255.0f, 180 / 255.0f, 1.0f);
    Vector2F _lightDir = Vector2F(-45, 180);
    //Vector2F _lightDir = Vector2F(-45, 90);
};

#endif // MAINWINDOW_H
