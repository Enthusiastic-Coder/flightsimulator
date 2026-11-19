#ifndef QT_SIMULATOR_WINDOW_H
#define QT_SIMULATOR_WINDOW_H


#include <include_gl.h>
#include "QtGameLoop.h"

#ifdef WIN32
#include "loadWAV.h"
#endif

#include "WorldSystem.h"
#include "OpenGLShaderProgram.h"
#include "OpenGLFrameBuffer.h"
#include "OpenGLRenderBuffer.h"
#include "MeshModel.h"
#include "SimpleSpringModel.h"
#include "PfdRenderer.h"
#include "SkyDome.h"
#include "OpenGLRenderer.h"
#include <QImage>
#include <optional>

#include "OpenGLFontTexture.h"
#include "OpenGLFontRenderer.h"
#include "OpenGLButtonTextureManager.h"
#include "OpenGLTextureRenderer2D.h"
#include "OpenGLButtonTexture.h"
#include "OpenGLSliderControl.h"

#define Time_log(msg)
//#define Time_log(msg) \
  //      fprintf(_fTimeLog, msg##" - %.6f\n", timer.ElapsedMilliSeconds());


class QtSimulatorWindow : public QtGameLoop
{
public:

    QtSimulatorWindow();
    ~QtSimulatorWindow();

protected:
    bool onInitialise() override;
    void onUnInitialise() override;
    void onUpdate() override;
    void onRender() override;
    void onKeyDown(QKeyEvent* e) override;
    void mouseMoveEvent(QMouseEvent* e) override;
    void mousePressEvent(QMouseEvent* e) override;
    void mouseReleaseEvent(QMouseEvent* e) override;
    void wheelEvent(QWheelEvent* e) override;
    void onSize(int width, int height) override;

    bool createFrameBufferAndShaders();
    void OnInitPolyMode();
    void RenderScene();
    void RenderReflection();
    void RenderDepthTextures(int camID, OpenGLTexture2D &shadowMap, OpenGLTexture2D& colorMap, int offset, int size);
    void RenderMouseFlying(float cx, float cy);
    void RenderDrivingPower();
    void RenderPlaybackRecordingGrafix();
    void RenderTransparentRectangle( int x, int y, int cx, int cy, float R, float G, float B, float A);
    void RenderFPS();
    void RenderInfo();
    void RenderTexture(OpenGLTexture2D &texID, float U, float V, float heightFraction = 0.75f);
    void OnRenderSkyBox();
    void OnInitSound();
    void OnUnitSound();
    void ensureCameraAboveGround();
    void sendDataToShader(OpenGLShaderProgram &progID, int slot, int width, int height);

    void setupCameraOrientation();
    void setupCameraPosition(bool bReflection=false);
    void processInputsForCamera();

    void persistSettings(bool bSerialise);
    std::pair<float,float> UVtoScreen( float U, float V);
    void UVsToScreen(float*pts, int count);

private:
    int _wheelDelta = 0;
    bool _showCursor;
    //bool _paused;
    std::pair<int, int> _currentDims;
    bool _bUserPolygonLineView;
    float _framerate[FPS_RESOLUTION];
    int _framecount;

    PfdParameters _pfdParameters;
    const JSONRigidBody* _pfdBody = nullptr;
    std::optional<PfdParameters> _renderedPfdParameters;
    QImage _pfdImage;
    WorldSystem _WorldSystem;
    SimpleSpringModel _towTruckSpring;
    //TowTruck _towTruck;
    //TowBar _towBar;
    //RunwaySurface _runwayscene;
    //RunwayLites _runwaylites;
    //RunwayLites _runwaylites2;
    //LondonTile _londonTile;
    //WaterTile _waterTile;
    //SimpleCloud _simpleCloud;
    //FlatLifter _flatLifter;
    //AudiA8 _audiA8;
    //CrossRoads _crossRoads;
    //RingRoad _ringRoad;
    //PiccadillyLineTrainA _trainA;
    GPSLocation _Runway_Location;
    OpenGLTexture2D _runway1;
    OpenGLTexture2D _runway2;
    OpenGLTexture2D _slopeGrass;

    ////////////////////////////////
    Camera _camera;
    //////////////////////////////////

#ifndef LOCATED_AT_LONDON
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


    OpenGLShaderProgram _reflectionShaderProgram;
    OpenGLShaderProgram _waterShaderProgram;
#endif

    OpenGLShaderProgram _simpleShaderProgram;
    OpenGLShaderProgram _shadowShaderProgram;
    OpenGLShaderProgram _simplePrimitiveShaderProgram;
    OpenGLShaderProgram _simpleColorPrimitiveShaderProgram;
    OpenGLShaderProgram _fontShaderProgram;
    OpenGLShaderProgram _textureShaderProgram;
    OpenGLFrameBuffer _openGLFrameBuffer;

    OpenGLTexture2D _shadowDepthMap1;
#ifndef LOCATED_AT_LONDON
    OpenGLTexture2D _shadowMap2;
    OpenGLTexture2D _shadowMap3;
#endif

    OpenGLTexture2D _shadowTextureMap1;
#ifndef LOCATED_AT_LONDON
    OpenGLTexture2D _shadowMapTexture2;
    OpenGLTexture2D _shadowMapTexture3;

    OpenGLTexture2D _dUdVMap;
    OpenGLTexture2D _normalMap;
    float _nUVOffset = 0.0f;
    float g_WaterFlow = 0.02f;

    OpenGLTexture2D _reflectionTexture;
    OpenGLRenderBuffer _depthRenderBuffer;
#endif

    OpenGLTexture2D _pfdColorTexture;

    Vector4F _skyColor = Vector4F(3 / 255.0f, 100 / 255.0f, 180 / 255.0f, 1.0f);
    Vector2F _lightDir = Vector2F(-45, 180);
    //Vector2F _lightDir = Vector2F(-45, 90);

    Renderer* _renderer = new OpenGLRenderer;

    SkyDome* _skyDome = new SkyDome(2, 20, 25, true);
    OpenGLTexture2D _skyDomeTexture;

    OpenGLFontRenderer _font;
    OpenGLFontTexture _myFontTexture;
    OpenGLTextureRenderer2D _textureRenderer;
    OpenGLButtonTextureManager _buttonTextureManager;
    OpenGLButtonTexture _buttonPrevCamera;
    OpenGLButtonTexture _buttonNextCamera;
    OpenGLButtonTexture _buttonJoystick;
    OpenGLButtonTexture _buttonBrakes;
    OpenGLButtonTexture _buttonPlayback;
    OpenGLButtonTexture _buttonResetPos;
    OpenGLButtonTexture _buttonResetApproach;
    OpenGLButtonTexture _buttonToggleInfo;
    OpenGLButtonTexture _buttonStartRecorder;

    OpenGLSliderControl _powerSliderControl;

    qint64 _playRecordFlashLastTickTime = 0;
    qint64 _playRecordFlashTickTime = 500;
    bool _bPlayBackFlashOn = false;
};


#endif //QT_SIMULATOR_WINDOW_H
