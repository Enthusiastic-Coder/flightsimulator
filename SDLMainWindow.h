#ifndef __SDLMAINWINDOW__
#define __SDLMAINWINDOW__


#include <include_gl.h>
#include "SDLGameLoop.h"

#ifdef WIN32
#include "loadWAV.h"
#endif

#include "WorldSystem.h"
#include "OpenGLShaderProgram.h"
#include "OpenGLFrameBuffer.h"
#include "OpenGLRenderBuffer.h"
#include "MeshModel.h"
#include "SimpleSpringModel.h"
#include "PFDView.h"
#include "SkyDome.h"
#include "OpenGLRenderer.h"

#include "OpenGLFontTexture.h"
#include "OpenGLFontRenderer2D.h"
#include "OpenGLButtonTextureManager.h"
#include "OpenGLTextureRenderer2D.h"
#include "OpenGLButtonTexture.h"
#include "OpenGLSliderControl.h"

#define Time_log(msg)
//#define Time_log(msg) \
  //      fprintf(_fTimeLog, msg##" - %.6f\n", timer.ElapsedMilliSeconds());


class SDLMainWindow : public SDLGameLoop
{
public:

    SDLMainWindow();
    ~SDLMainWindow();

protected:
    bool onInitialise() override;
    void onUnInitialise() override;
    void onUpdate() override;
    void onRender() override;
    void onHandleAsyncKeys(const Uint8* keys, SDL_Keymod mod, float dt) override;
    void onKeyDown(SDL_KeyboardEvent* e) override;
    void onKeyUp(SDL_KeyboardEvent* e) override;
    void onMouseMotion(SDL_MouseMotionEvent* e) override;
    void onMouseDown(SDL_MouseButtonEvent* e) override;
    void onMouseUp(SDL_MouseButtonEvent* e) override;
    void onMouseWheel(SDL_MouseWheelEvent *e ) override;
    void onFingerDown(SDL_TouchFingerEvent* e) override;
    void onFingerMotion(SDL_TouchFingerEvent* e) override;
    void onFingerUp(SDL_TouchFingerEvent* e) override;
    void onSize(int width, int height) override;
    void onMove(int x, int y) override;

    bool createFrameBufferAndShaders();
    void OnInitPolyMode();
    void RenderScene();
    void RenderReflection();
    void RenderDepthTextures(int camID, OpenGLTexture2D &shadowMap, OpenGLTexture2D& colorMap, int offset, int size);
    void RenderMouseFlying(float cx, float cy);
    void RenderDrivingPower();
    void RenderTransparentRectangle( int x, int y, int cx, int cy, float R, float G, float B, float A);
    void RenderFPS();
    void RenderInfo();
    void RenderTexture(OpenGLTexture2D &texID, float U, float V);
    void OnRenderSkyBox();
    void OnInitSound();
    void OnUnitSound();
    void ensureCameraAboveGround();
    void sendDataToShader(OpenGLShaderProgram &progID, int slot, int width, int height);

    void setupCameraOrientation();
    void setupCameraPosition(bool bReflection=false);
    void processInputsForCamera();

    void persistSettings(bool bSerialise);

private:
    bool _showCursor;
    //bool _paused;
    bool _bUserPolygonLineView;
    float _framerate[FPS_RESOLUTION];
    int _framecount;

    PFDView _pfdInstrument;
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
    OpenGLRenderBuffer _pfdStencilBuffer;

    Vector4F _skyColor = Vector4F(3 / 255.0f, 100 / 255.0f, 180 / 255.0f, 1.0f);
    Vector2F _lightDir = Vector2F(-45, 180);
    //Vector2F _lightDir = Vector2F(-45, 90);

    Renderer* _renderer = new OpenGLRenderer;

    SkyDome* _skyDome = new SkyDome(2, 20, 25, true);
    OpenGLTexture2D _skyDomeTexture;

    OpenGLFontRenderer2D _fontRenderer;
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

    OpenGLSliderControl _powerSliderControl;
};


#endif //__SDLMAINWINDOW__
