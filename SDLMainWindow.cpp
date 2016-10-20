#include "stdafx.h"
#include "SDLMainWindow.h"
//
//#include "RunwayLites.h"
//#include "LondonGroundTile.h"
//#include "WaterTile.h"
//#include "SimpleCloud.h"
#include "PFDView.h"
//#include "PerlinNoise.h"
//#include "FlatLifterRigidBody.h"
//#include "RingRoad.h"
//#include "PiccadillyLineTrainARigidBody.h"
//#include "AudiA8RigidBody.h"
//#include "TowTruckRigidBody.h"
//#include "TowBarRigidBody.h"

#include "AirProperties.h"
#include "BA320RigidBody.h"
#include "HeathrowTower.h"
#include "BA320RigidBody.h"
#include "BA380RigidBody.h"
#include "SimpleCloud.h"
#include "HarrierRigidBody.h"
#include "AircraftCarrierRigidBody.h"
#include "HeathrowRadar.h"
#include <iostream>

#include "OpenGLFontTexture.h"
#include "OpenGLFontRenderer2D.h"
#include "OpenGLTextureRenderer2D.h"
#include "OpenGLPainter.h"

#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

#define MS_TO_KTS( value ) fabs(value*3.2808*3600/5280/1.15f)
#define MS_TO_MPH( value ) MS_TO_KTS( value*1.15 )


void SDLMainWindow::onHandleAsyncKeys(const Uint8* keys, SDL_Keymod mod, float dt)
{
    SDL_UNUSED(keys);
    SDL_UNUSED(mod);
    SDL_UNUSED(dt);
//    bool bShiftPressed = KMOD_SHIFT & mod;
//    bool bCtrlPressed = KMOD_CTRL & mod;
//    bool bAltPressed = KMOD_ALT & mod;

//    if( keys[SDL_SCANCODE_PAGEUP] )
//        _worldSystem.focusRigidBody().powerUp(dt);

//    if( keys[SDL_SCANCODE_PAGEDOWN])
//        _worldSystem.focusRigidBody().powerDown(dt);

//    if( keys[SDL_SCANCODE_B] )
//        _worldSystem.FocusRigidBody().applyStandardBrakes(bShiftPressed, dt);
}

void SDLMainWindow::onKeyUp(SDL_KeyboardEvent* e)
{ SDL_UNUSED(e); }

void SDLMainWindow::onMouseUp(SDL_MouseButtonEvent* e)
{
    _buttonTextureManager.handleMouseUp({e->x, e->y});
    _powerSliderControl.handleMouseUp({e->x, e->y});
}

void SDLMainWindow::onMouseWheel(SDL_MouseWheelEvent *e)
{
    _WorldSystem.onMouseWheel(e->y);
    JSONRigidBody *focus = _WorldSystem.focusedRigidBody();
    if (focus)
    {
        focus->getPower(0);
        _powerSliderControl.setValue(focus->getPowerOutput(0));
    }
}

void SDLMainWindow::onMouseMotion(SDL_MouseMotionEvent *e)
{
    _buttonTextureManager.handleMouseMove({e->x, e->y});
    _powerSliderControl.handleMouseMove({e->x, e->y});
}

void SDLMainWindow::onMouseDown(SDL_MouseButtonEvent *e)
{
    _buttonTextureManager.handleMouseDown({e->x, e->y});
    if(_powerSliderControl.handleMouseDown({e->x, e->y}))
    {
        JSONRigidBody *focus = _WorldSystem.focusedRigidBody();
        if (focus)
        {
            focus->setPower(_powerSliderControl.getValue());
        }
    }
}

void SDLMainWindow::onFingerDown(SDL_TouchFingerEvent* e)
{ SDL_UNUSED(e); }
void SDLMainWindow::onFingerMotion(SDL_TouchFingerEvent* e)
{ SDL_UNUSED(e); }
void SDLMainWindow::onFingerUp(SDL_TouchFingerEvent* e)
{ SDL_UNUSED(e); }

void SDLMainWindow::onSize(int width, int height)
{
    SDLGameLoop::onSize(width, height);
    _textureRenderer.onSize(0,0, width, height);
    _powerSliderControl.onSize(width, height);
    _buttonTextureManager.onSizeLayout();

    for(int i=0; i < 3; ++i)
    {
        OpenGLMatrixStack & mat = OpenGLPipeline::Get(i).GetProjection();
        mat.LoadIdentity();
        mat.SetPerspective(55, float(width)/height, 1, 120000);
    }
}

void SDLMainWindow::onMove(int x, int y)
{
    SDL_UNUSED(x);
    SDL_UNUSED(y);
}

SDLMainWindow::SDLMainWindow() :
    _bUserPolygonLineView(false),
    _showCursor(true),
    //_paused(false),
    _framecount(0),
#ifndef LOCATED_AT_LONDON
    _soundDevice(0),
    _soundContext(0),
#endif
    _textureRenderer(_renderer),
    _buttonTextureManager(&_textureRenderer)

{
    memset( _framerate, 0, sizeof(_framerate) );
}

SDLMainWindow::~SDLMainWindow()
{
    delete _renderer;
}

bool SDLMainWindow::createFrameBufferAndShaders()
{
    if (!_simpleShaderProgram.loadFiles("shaders/simpleShader.vert", "shaders/simpleShader.frag"))
    {
        SDL_Log("%s %s", _simpleShaderProgram.getError().c_str(), "simpleShader Failed" );
        return false;
    }

#ifdef LOCATED_AT_LONDON
    if (!_shadowShaderProgram.loadFiles("shaders/shadowShader-mobile.vert", "shaders/shadowShader-mobile.frag"))
    {
        SDL_Log("%s %s", _shadowShaderProgram.getError().c_str(), "shadowShader-mobile Failed");
        return false;
    }
#else
    if (!_shadowShaderProgram.loadFiles("shaders/shadowShader.vert", "shaders/shadowShader.frag"))
    {
        SDL_Log("%s %s", _shadowShaderProgram.getError().c_str(), "shadowShader Failed");
        return false;
    }

    if (!_reflectionShaderProgram.loadFiles("shaders/reflectionShader.vert", "shaders/reflectionShader.frag"))
    {
        SDL_Log("%s %s",  _reflectionShaderProgram.getError().c_str(), "reflectionShader Failed");
        return false;
    }

    if (!_waterShaderProgram.loadFiles("shaders/water.vert", "shaders/water.frag"))
    {
        SDL_Log("%s %s", _waterShaderProgram.getError().c_str(), "waterShader Failed");
        return false;
    }
#endif

    if( !_simplePrimitiveShaderProgram.loadFiles("shaders/simplePrimitiveShader.vert", "shaders/simplePrimitiveShader.frag"))
    {
        SDL_Log("%s %s", _simplePrimitiveShaderProgram.getError().c_str(), "simplePrimitiveShader Failed");
        return false;
    }

    if( !_simpleColorPrimitiveShaderProgram.loadFiles("shaders/simpleColorPrimitiveShader.vert", "shaders/simpleColorPrimitiveShader.frag"))
    {
        SDL_Log("%s %s", _simpleColorPrimitiveShaderProgram.getError().c_str(), "simpleColorPrimitiveShader Failed");
        return false;
    }

    if( !_fontShaderProgram.loadFiles("shaders/fontShader.vert", "shaders/fontShader.frag"))
    {
        SDL_Log("%s %s", _fontShaderProgram.getError().c_str(), "_fontShaderProgram Failed");
        return false;
    }

    if( !_textureShaderProgram.loadFiles("shaders/textureShader.vert", "shaders/textureShader.frag"))
    {
        SDL_Log("%s %s", _textureShaderProgram.getError().c_str(), "_textureShaderProgram Failed");
        return false;
    }

    if (!_openGLFrameBuffer.generate())
    {
        SDL_Log("Failed to be created. - FrameBuffer Failed");
        return false;
    }

    int f[3] = {
#ifdef ANDROID
     2, 2, 4
#else
     4, 2 ,4
#endif
    };


    if (!_shadowDepthMap1.generate(512 * f[0], 512 * f[0], true))
    {
        SDL_Log("Failed to be created. - Depth Texure Buffer Failed");
        return false;
    }

#ifndef LOCATED_AT_LONDON
    if (!_shadowMap2.generate(512 * f[1], 512 * f[1], true))
    {
        SDL_Log("Failed to be created. - Depth Texure 2 Buffer Failed");
        return false;
    }

    if (!_shadowMap3.generate(512 * f[2], 512 * f[2], true))
    {
        SDL_Log("Failed to be created. - Depth Texure 3 Buffer Failed");
        return false;
    }
#endif

    if (!_shadowTextureMap1.generate(512 * f[0], 512 * f[0], false))
    {
        SDL_Log( "Failed to be created. - Depth Color Texure Buffer Failed");
        return false;
    }

#ifndef LOCATED_AT_LONDON
    if (!_shadowMapTexture2.generate(512 * f[1], 512 * f[1], false))
    {
        SDL_Log("Failed to be created. - Depth Color Texure 2 Buffer Failed");
        return false;
    }

    if (!_shadowMapTexture3.generate(512 * f[2], 512 * f[2], false))
    {
        SDL_Log( "Failed to be created. - Depth Color Texure 3 Buffer Failed");
        return false;
    }
#endif

    int pfdWidth = 350;
    int pfdHeight = 350;

    if( !_pfdColorTexture.generate(pfdWidth, pfdHeight, false))
    {
        SDL_Log("Failed to create texture - PFD color Texture");
        return false;
    }

    if( !_pfdStencilBuffer.generateStencil(pfdWidth, pfdHeight))
    {
        SDL_Log("Failed to create stencil - PFD stencil buffer");
        return false;
    }

#ifndef LOCATED_AT_LONDON
    int width, height;
    GetScreenDims(width, height);

    if (!_depthRenderBuffer.generate(width, height))
    {
        SDL_Log("Depth Render buffer not complete. - FrameBuffer Failed");
        return false;
    }

    if (!_reflectionTexture.generate(width, height, false))
    {
        SDL_Log( "Failed to be created. - Reflection Texure");
        return false;
    }

    _dUdVMap.setMinification(GL_LINEAR_MIPMAP_NEAREST);
    _dUdVMap.setWrapMode(GL_REPEAT);

    if (!_dUdVMap.Load(ROOT_APP_TEXTURES_DIRECTORY"dudvmap.bmp"))
    {
        SDL_Log("Failed to be load. - dU/dV Texure");
        return false;
    }

    _normalMap.setMinification(GL_LINEAR_MIPMAP_NEAREST);
    _normalMap.setWrapMode(GL_REPEAT);

    if (!_normalMap.Load(ROOT_APP_TEXTURES_DIRECTORY"normalmap.bmp"))
    {
        SDL_Log("Failed to be load.  - Normal Texure");
        return false;
    }
#endif

    return true;
}

bool SDLMainWindow::onInitialise()
{
    try
    {
        if (!createFrameBufferAndShaders())
            return false;

        //_myFontTexture.loadfile("fonts/MS Shell Dlg 2-16.png");
        //_myFontTexture.loadfile("fonts/Verdana-20.png");
        //_myFontTexture.loadfile("fonts/MS Shell Dlg 2-28.png");
        //_myFontTexture.loadfile("fonts/Verdana-11-Bold.png");
        _myFontTexture.loadfile("fonts/Verdana-10.png");

        _fontRenderer.selectShader(&_fontShaderProgram);
        _fontRenderer.selectRenderer(_renderer);

        _WorldSystem.onInitialise();

        _WorldSystem.addJSONBody(new BAAirbus320JSONRigidBody("BAA320:A320_A_GIB"));
#ifndef LOCATED_AT_LONDON
        _WorldSystem.addJSONBody(new BAAirbus320JSONRigidBody("AirArabia:A320_B_GIB"));
        _WorldSystem.addJSONBody(new BAAirbus380JSONRigidBody("BAA380:A380_A_GIB"));
        _WorldSystem.addJSONBody(new HarrierJSONRigidBody("Harrier:Harrier_A_GIB"));
        _WorldSystem.addJSONBody(new AircraftCarrierJSONRigidBody("AircraftCarrier:Carrier_A_GIB"));
#endif
#ifndef LOCATED_AT_GIBRALTER
        _WorldSystem.addStaticJSONBody(new HeathrowTowerRigidBody);
        _WorldSystem.addStaticJSONBody(new HeathrowRadarRigidBody);
#endif
        _WorldSystem.setLightFraction(0.9f);

        _pfdInstrument.initialise();

        _WorldSystem.loadBodyRecorderedData();

        persistSettings(false);

        _camera.setRemoteViewPtr( _WorldSystem.getCameraView());
        _camera.fastForwardLocalView();

        OnInitPolyMode();

        glEnable(GL_DEPTH_TEST);

        glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE );

        glClearColor(0.0f,0.0f,0.0f, 1.0f);

    #if defined _DEBUG
        setFramePerSecond(30);
    #else
        setFramePerSecond(40);
    #endif

        glClearStencil(0);

        SDL_SetWindowTitle(getWindow(), "OpenGL Simulation by Mo");

        /*_runwayscene.setPosition(GPSLocation(51.46158,-0.459402)+ VectorD(120,0,-325) );
        _runwaylites.setPosition(GPSLocation(51.46158,-0.459402) + GPSLocation(0.0032569999999978450, -0.014778000000000013) );
        _runwaylites.setOrientation(0,90,0);
        _runwaylites2.setPosition(_runwaylites.getGPSLocation() + VectorD(2600,0,0));
        _runwaylites2.setOrientation(0,270,0);
        _londonTile.setPosition(_runwayscene.getGPSLocation() + VectorD(1000,0,-1000)  );
        _simpleCloud.setPosition(_runwayscene.getGPSLocation() +VectorD(0,100,0)  );
        _waterTile.setPosition( GPSLocation(51.46158,-0.459402) + VectorD(0,0,500 ));
    */

    //	_audiA8.setPosition( GPSLocation(51.4648026, -0.47427592 ) );
    //	_audiA8.initSpeedAndPos();

    //	_crossRoads.setPosition( GPSLocation(51.4648,-0.4719087,0) + VectorD(-200,0,-200) );

        //_ringRoad.Initialise( 7, -1, 10, 10 );
        //_ringRoad.Initialise( 10, -0.2, 50, 1 );
        /*_ringRoad.Initialise( 10, -0.05, 15, 1 );
        _ringRoad.setPosition( _crossRoads.getGPSLocation() + VectorD(-50,0,-100) );
        _ringRoad.setOrientation(0,0,0);*/

    //	_trainA.setPosition( _ringRoad.getGPSLocation() + VectorD(100,0,0) );
    //	_trainA.setOrientation(0,0,0);

        /*_londonTile.initialise();
        _runwaylites.initialiseSides();
        _runwaylites.initialise();
        _runwaylites2.initialise();

        _runwayscene.CurveScenery();
        _londonTile.CurveScenery();*/

    //	_towTruck.setPosition(GPSLocation(51.4648,-0.4719087,0) + VectorD(-10,0,0));
    //	_towTruck.setOrientation(0,0,0);
    //	_towBar.setPosition(GPSLocation(51.4648,-0.4719087,0)+ VectorD(-15,0,0));
    //	_towBar.setOrientation(0,0,0);

        /*_flatLifter.setPosition( GPSLocation(51.4648,-0.4719087,0) + VectorD(0,0,-200) );
        _flatLifter.initSpeedAndPos();*/

        //_londonTile.Attach(&_WorldSystem );
        //_towTruck.Attach( &_WorldSystem );
        //_towBar.Attach( &_WorldSystem );
    #if defined SATELLITE_RUNWAY
        //_runwayscene.Attach( &_WorldSystem );
    #endif
        //_heathrowTower.Attach( &_WorldSystem );
        //_simpleCloud.Attach( &_WorldSystem );
        //_runwaylites.Attach( &_WorldSystem );
        //_runwaylites2.Attach( &_WorldSystem );
        //_aircraftCarrier.Attach( &_WorldSystem );
        //_flatLifter.Attach( &_WorldSystem );
        //_audiA8.Attach( &_WorldSystem );
        //_crossRoads.Attach( &_WorldSystem );
        //_ringRoad.Attach( &_WorldSystem );

        //_trainA.Attach(&_WorldSystem );
        //_harrierJumpJet.Attach(&_WorldSystem );

    //#define __WATER__
    #if !defined _DEBUG
    #if defined __WATER__
        _waterTile.initialise();
        _waterTile.CurveScenery();
        _waterTile.Attach( &_WorldSystem );
    #endif
    #endif

        _towTruckSpring.setMass( 2000 );
        _towTruckSpring.setEquilibrium( 0.1f );
        _towTruckSpring.setMax( 0.75f);
        _towTruckSpring.setMu( 20000.0f );

        /*static_cast< ConstrainedSpring<0,TowTruckRigidBody<TowTruck>>*>( &_towTruck)
            ->setPartner<0,TowBarRigidBody<TowBar>,ConstrainedSpringForceGenerator>( VectorD(0, 0.47f, 3.69f ), &_towBar, VectorD(0, 0.56f, -3.09f), &_towTruckSpring );

        static_cast< ConstrainedSpring<1,TowTruckRigidBody<TowTruck>>*>( &_towTruck)
            ->setPartner<1,TowBarRigidBody<TowBar>,ConstrainedSpringForceGenerator>( VectorD(0, 0.47f, -3.69f ), &_towBar, VectorD(0, 0.56f, -3.09f), &_towTruckSpring);

        static_cast< WheelConstrainedSpring<0,BAAirbus320JSONRigidBody<AirbusAircaft>>*>( &_airbusAircraft )
            ->setPartner<2,TowBarRigidBody<TowBar>,ConstrainedSpringForceGenerator>( VectorD(0, 0.47f-0.47f, -12.30f ), &_towBar, VectorD(0, 0.47f, 2.6f), &_towTruckSpring);*/

        _runway1.setMinification( GL_NEAREST );
        _runway1.Load( "images/myrunway1.png");

        _runway2.setMinification( GL_NEAREST );
        _runway2.Load( "images/myrunway2.png" );
        _slopeGrass.Load( "images/grass_slope.png" );
        //_slopeGrass.Load("textures/countryside.png");

        _skyDomeTexture.Load("images/skydome.png");
        //_skyDomeTexture.Load("images/skygradient.png");

        //OnInitSound();
        _buttonTestTexture.load("images/ideal_colors.png");
        _buttonTextureManager.setButtonPos(&_buttonTestTexture, 0.5f, 0.0f, 0.1f, 0.1f);
        _buttonTestTexture.setHAlignment(OpenGLButtonTexture::Align_Middle);
        _buttonTestTexture.setVAlignment(OpenGLButtonTexture::Align_Low);

        _buttonJoystick.load("images/buttons/joystick.png");
        _buttonTextureManager.setButtonToggle(&_buttonJoystick, true);
        _buttonTextureManager.setButtonPos(&_buttonJoystick, 0.5f, 0.5f, 0.1f, 0.1f);
        _buttonTextureManager.setVisibleOnHover(&_buttonJoystick, true);
        _buttonJoystick.setHAlignment(OpenGLButtonTexture::Align_Middle);
        _buttonJoystick.setVAlignment(OpenGLButtonTexture::Align_Middle);
        _buttonJoystick.setColor(Vector4F(1,1,1,0.15));

        _powerSliderControl.setRange(0, 100);
        _powerSliderControl.setTickValue(5);
        _powerSliderControl.setRateValue(50);
        _powerSliderControl.setDimensions(OpenGLSliderControl::Orient_Vertical, 0.95, 0.5, 0.02, 0.8);
        _powerSliderControl.setVAlignment(OpenGLSliderControl::Align_Middle);
        _powerSliderControl.setHAlignment(OpenGLSliderControl::Align_High);
    }
    catch (std::string str)
    {
        SDL_Log( "Error :%s:", str.c_str());
        return false;
    }

    return true;
}

void SDLMainWindow::OnInitSound()
{
#ifndef LOCATED_AT_LONDON
    ALenum errorNo = alGetError();
    const ALCchar *defaultDevice = alcGetString(0, ALC_DEFAULT_DEVICE_SPECIFIER);

    if (defaultDevice == 0)
        return;

    _soundDevice = alcOpenDevice(defaultDevice);
    if (_soundDevice == 0)
        return;

    _soundContext = alcCreateContext(_soundDevice, 0);
    if (_soundContext == 0)
        return;

    if (alcMakeContextCurrent(_soundContext) == AL_FALSE)
        return;

    alGenSources( 1, &_soundEnvironmentSource );
    alGenSources( 1, &_soundEngineSource );
    alGenSources( 1, &_soundHeightCallOutSource );

    _soundEnvironmentBuffer = load_wave("sounds/environment.wav" );
    alSourcei(_soundEnvironmentSource, AL_BUFFER,_soundEnvironmentBuffer);
    alSourcef(_soundEnvironmentSource, AL_PITCH, 1.0f);
    alSourcef(_soundEnvironmentSource, AL_GAIN, 0.15f);

    _soundExternalEngineBuffer = load_wave( "sounds/X_b_JT8D_0.wav" );
    _soundInternalEngineBuffer = load_wave( "sounds/X320_ENG.wav" );
    alSourcei(_soundEngineSource, AL_BUFFER,_soundInternalEngineBuffer);
    alSourcef(_soundEngineSource, AL_PITCH, 0.0f);
    alSourcef(_soundEngineSource, AL_GAIN, 0.15f);

    alSourcef(_soundHeightCallOutSource, AL_PITCH, 1.0f);
    alSourcef(_soundHeightCallOutSource, AL_GAIN, 0.55f);

    _soundHeightCallOutBuffers[2] = load_wave( "sounds/touchr.wav" );
    _soundHeightCallOutBuffers[10] = load_wave( "sounds/FPDA_A330_10FT.wav" );
    _soundHeightCallOutBuffers[20] = load_wave( "sounds/FPDA_A330_20FT.wav" );
    _soundHeightCallOutBuffers[30] = load_wave( "sounds/FPDA_A330_30FT.wav" );
    _soundHeightCallOutBuffers[40] = load_wave( "sounds/FPDA_A330_40FT.wav" );
    _soundHeightCallOutBuffers[50] = load_wave( "sounds/FPDA_A330_50FT.wav" );
    _soundHeightCallOutBuffers[100] = load_wave( "sounds/FPDA_A330_100FT.wav" );
    _soundHeightCallOutBuffers[200] = load_wave( "sounds/FPDA_A330_200FT.wav" );
    _soundHeightCallOutBuffers[300] = load_wave( "sounds/FPDA_A330_300FT.wav" );
    _soundHeightCallOutBuffers[400] = load_wave( "sounds/FPDA_A330_400FT.wav" );
    _soundHeightCallOutBuffers[500] = load_wave( "sounds/FPDA_A330_500FT.wav" );
    _soundHeightCallOutBuffers[1000] = load_wave( "sounds/FPDA_A330_1000FT.wav" );
    _soundHeightCallOutBuffers[2500] = load_wave( "sounds/FPDA_A330_2500FT.wav" );

    ALfloat source0Pos[] = { 0.0, 0.0, 1.0 };
    ALfloat source0Vel[] = { 0.0, 0.0, 0.0 };
    alSourcefv(_soundEnvironmentSource, AL_POSITION, source0Pos);
    alSourcefv(_soundEnvironmentSource, AL_VELOCITY, source0Vel);

    alSourcefv(_soundEngineSource, AL_POSITION, source0Pos);
    alSourcefv(_soundEngineSource, AL_VELOCITY, source0Vel);

    alSourcefv(_soundHeightCallOutSource, AL_POSITION, source0Pos);
    alSourcefv(_soundHeightCallOutSource, AL_VELOCITY, source0Vel);

    errorNo = alGetError();
    alSourcei(_soundEnvironmentSource, AL_LOOPING, AL_TRUE);
    alSourcei(_soundEngineSource, AL_LOOPING, AL_TRUE);
    alSourcei(_soundHeightCallOutSource, AL_LOOPING, AL_FALSE );
#endif
}

void SDLMainWindow::OnUnitSound()
{
#ifndef LOCATED_AT_LONDON
    alSourceStop(_soundEnvironmentSource);
    alDeleteSources( 1, &_soundEnvironmentSource );
    alDeleteBuffers( 1, &_soundEnvironmentBuffer );

    alSourceStop(_soundEngineSource );
    alDeleteSources(1, &_soundEngineSource );
    glDeleteBuffers( 1, &_soundInternalEngineBuffer );
    glDeleteBuffers( 1, &_soundExternalEngineBuffer );

    for( auto& it : _soundHeightCallOutBuffers )
        glDeleteBuffers(1, &it.second);

    alDeleteSources( 1, &_soundHeightCallOutSource );

    alcMakeContextCurrent(NULL);
    alcDestroyContext(_soundContext);
    alcCloseDevice(_soundDevice);
#endif
}

void SDLMainWindow::ensureCameraAboveGround()
{
    HeightData data;
    _WorldSystem.getHeightFromPosition( _camera.remoteView()->getPosition(), data );
    float fMinHeight = 0.05f;

    if( data.Height() < fMinHeight )
    {
        GPSLocation loc = _camera.remoteView()->getPosition();
        loc._height = loc._height - data.Height() + fMinHeight;
        _camera.remoteView()->setPosition(loc);
        _camera.fastForwardLocalView();
    }
}

void SDLMainWindow::onUnInitialise()
{
    persistSettings(true);
    _WorldSystem.onUnintialise();
    OnUnitSound();
}


void SDLMainWindow::onKeyDown(SDL_KeyboardEvent *e)
{
    SDL_UNUSED(e);
    /*    if( e.key.keysym.scancode == SDL_SCANCODE_F11 )
            _WorldSystem.environment().incrLightFraction(-0.01f);

        if( e.key.keysym.scancode == SDL_SCANCODE_F12 )
            _WorldSystem.environment().incrLightFraction(0.01f);

        //Reset position
        if( e.key.keysym.scancode == SDL_SCANCODE_F5)
            _WorldSystem.RigidBody().resetPosition();

        if( e.key.keysym.scancode == SDL_SCANCODE_F7)
            _WorldSystem.RigidBody().resetApproach();*/

    JSONRigidBody *pBody = _WorldSystem.focusedRigidBody();
    bool bShiftPressed = (GetAsyncKeyState(VK_SHIFT) < 0);
    bool bAltPressed = GetAsyncKeyState(VK_MENU) & 0x8000;

    if( ::GetKeyState(VK_F5) < 0 )
    {
        /*_towTruck.initSpefile:///C:/Project/Code/Qt/FlightSimulator/Release/lastGPSpos.binedAndPos();
            _towTruck.reset();
            _towTruck.setPosition(GPSLocation(51.4648,-0.4719087) + VectorD(-150,0,-20));
            _towTruck.setOrientation(0,90,0);

            _towBar.initSpeedAndPos();
            _towBar.reset();
            _towBar.setPosition(GPSLocation(51.4648,-0.4719087)+ VectorD(-150,0,0));
            _towBar.setOrientation(0,90,0);

            _audiA8.reset();
            _audiA8.setPosition(GPSLocation(51.46731605,-0.47542586,0.5 ) + VectorD(0,0,250) );
            _audiA8.setOrientation(0,0,0);*/


        if( pBody )
        {
            _camera.remoteView()->setPosition( pBody->getGPSLocation() + Vector3F(0,0,100));
            _camera.fastForwardLocalView();
        }
    }

    if(pBody)
    {
        if( ::GetKeyState(VK_F5) < 0)
            pBody->airResetPos();

        if( GetKeyState(VK_F7) < 0)
            pBody->airResetApproachPos();

        if( GetKeyState(VK_F3) <  0)
            pBody->airSpoilerToggle(true);

        if( GetKeyState(VK_F4) <  0)
            pBody->airSpoilerToggle(false);

        if( GetKeyState(VK_F1) < 0)
            pBody->airFlapIncr( -1 );

        if( GetKeyState(VK_F2) < 0)
            pBody->airFlapIncr( 1 );

        if( ::GetKeyState('R') < 0)
        {
            if( bShiftPressed )
                pBody->startRecording();
            else if( bAltPressed )
                pBody->togglePlayback();
        }
    }

    if( ::GetKeyState('F') < 0)
    {
        if (GetAsyncKeyState(VK_SHIFT) < 0)
            _WorldSystem.prevFocusedRigidBody();
        else
            _WorldSystem.nextFocusedRigidBody();
    }

    if( _WorldSystem.onSyncKeyPress())
        return;

    if( ::GetKeyState(VK_F11) < 0 )
        _WorldSystem.incrLightFraction(-0.01f);

    if( ::GetKeyState(VK_F12) < 0 )
        _WorldSystem.incrLightFraction(0.01f);

    if( ::GetKeyState('R') < 0 )
    {
        if( !bShiftPressed && !bAltPressed)
        {
            setRunning(!isRunning());

            if( !isRunning() )
                update();
        }
    }

    if( ::GetKeyState('P') < 0 )
    {
        _bUserPolygonLineView = !_bUserPolygonLineView;
        update();
    }

    if( ::GetKeyState('D') < 0 )
    {
        if( ::GetKeyState(VK_SHIFT) < 0)
            global_info = !global_info;
        else
            global_fg_debug = !global_fg_debug;
    }

    if( !isRunning() )
    {
        processInputsForCamera();
        ensureCameraAboveGround();

        update();
    }

    if( ::GetKeyState(VK_F6) < 0 )
    {
        if( ::GetKeyState(VK_SHIFT) < 0 )
            _WorldSystem.prevView();
        else
            _WorldSystem.nextView();

        if (_WorldSystem.focusedRigidBody() && _WorldSystem.focusedRigidBody()->getID() == "A320_A_GIB")
        {
//            if (_view == Camera::ViewPosition::CockpitForwards)
//            {
//                ALfloat source0Pos[] = { 0.0, 0.0, 1.0 };
//                ALfloat source0Vel[] = { 0.0, 0.0, 0.0 };

//                alListener3f(AL_POSITION, 0, 0, 0);
//                alSourcefv(_soundEngineSource, AL_POSITION, source0Pos);
//                alSourcefv(_soundEngineSource, AL_VELOCITY, source0Vel);
//                alSourcef(_soundEngineSource, AL_GAIN, 0.15f);

//                alSourcePlay(_soundEnvironmentSource);
//                alSourceStop(_soundEngineSource);

//                alSourcei(_soundEngineSource, AL_BUFFER, _soundInternalEngineBuffer);
//                alSourcePlay(_soundEngineSource);
//            }
//            else
//            {
//                alSourceStop(_soundEnvironmentSource);
//                alSourceStop(_soundEngineSource);

//                alSourcef(_soundEngineSource, AL_GAIN, 1.0f);
//                alSourcef(_soundEngineSource, AL_PITCH, 1.0f);
//                alSourcei(_soundEngineSource, AL_BUFFER, _soundExternalEngineBuffer);
//                alSourcePlay(_soundEngineSource);
//            }
        }
    }

    if( ::GetKeyState(VK_F8) < 0 )
    {
        if( ::GetKeyState(VK_SHIFT) < 0 )
        {
            global_force_lines_debug = (__global_force_lines_debug)((int)global_force_lines_debug-1);
            if( (int)global_force_lines_debug <= (int)__global_force_lines_debug::force_lines_begin )
                global_force_lines_debug = (__global_force_lines_debug)(int)(__global_force_lines_debug::force_lines_end-1);
        }
        else
        {
            global_force_lines_debug =(__global_force_lines_debug) ((int)global_force_lines_debug+1);
            if( (int)global_force_lines_debug >= (int)__global_force_lines_debug::force_lines_end )
                global_force_lines_debug = (__global_force_lines_debug)(int)(__global_force_lines_debug::force_lines_begin+1);
        }
    }

    //		  if( key == VK_SHIFT )
    //		  _camera.toggleShift();
}

void SDLMainWindow::onUpdate()
{
    float dt = frameTime();

    if( dt >= FLT_EPSILON)
        _framerate[_framecount++] = 1/dt;

    if( _framecount >= FPS_RESOLUTION )
        _framecount = 0;

    _buttonJoystick.setButtonVisible(!_buttonJoystick.isButtonDown());
    _buttonTextureManager.update(dt);

    JSONRigidBody *focus = _WorldSystem.focusedRigidBody();
    if (focus)
    {
        if( focus->getState() == JSONRigidBody::STATE::PLAYBACK)
            _powerSliderControl.setValue(focus->getPowerOutput(0));
        else
            _powerSliderControl.setValue(focus->getPower(0));
    }

    _powerSliderControl.update(dt);

#ifndef LOCATED_AT_LONDON
    _nUVOffset += g_WaterFlow * dt;
#endif

    if (GetFocus() == _hWnd)
        processInputsForCamera();

    if( GetAsyncKeyState( 'C' ) < 0 )
        _WorldSystem.incrChaseAngle(-50*dt);

    if( GetAsyncKeyState( 'V' ) < 0 )
        _WorldSystem.incrChaseAngle(50*dt);

    if( GetAsyncKeyState( 'W' ) < 0 )
        _WorldSystem.incrChaseHeight(50*dt);

    if( GetAsyncKeyState( 'S' ) < 0 )
        _WorldSystem.incrChaseHeight(-50*dt);

    if( GetAsyncKeyState( VK_OEM_PERIOD ) < 0 )
        _WorldSystem.incrChaseDistance(-50*dt);

    if( GetAsyncKeyState( VK_OEM_COMMA ) < 0 )
        _WorldSystem.incrChaseDistance(50*dt);

    if( _buttonTextureManager.buttonClicked(&_buttonTestTexture))
        _WorldSystem.nextView();

    if( _buttonTextureManager.buttonClicked(&_buttonJoystick))
        _WorldSystem.rigidBodyToggleUsingMouse();

    if( isRunning() )
    {
        ensureCameraAboveGround();
        _WorldSystem.onAsyncKeyPress(this, dt);
        _WorldSystem.update(dt);
        _camera.setRemoteViewPtr(_WorldSystem.getCameraView());
        _camera.onUpdate(dt);

    }

    bool bPlaneActive = _WorldSystem.focusedRigidBody()
                        && _WorldSystem.focusedRigidBody()->getID() == "A320_A_GIB";

    BAAirbus320JSONRigidBody* pAir = (BAAirbus320JSONRigidBody*)_WorldSystem.focusedRigidBody();

    if (bPlaneActive)
    {
        HeightData data;
        _WorldSystem.getHeightFromPosition(pAir->getLastPosition(), data);

        double dHeightBefore = data.Height() * 3.2808 - 2;

//        if( _view == Camera::CockpitForwards )
//        {
//            _WorldSystem.getHeightFromPosition(pAir->getGPSLocation(), data);
//            double dHeightAfter = data.Height() * 3.2808 - 2;

//            for( auto& it : _soundHeightCallOutBuffers )
//            {
//                if( dHeightBefore > it.first && dHeightAfter < it.first )
//                {
//                    alSourceStop(_soundHeightCallOutSource);
//                    alSourcei(_soundHeightCallOutSource, AL_BUFFER,it.second);
//                    alSourcePlay( _soundHeightCallOutSource );
//                    break;
//                }
//            }
//        }
//        else
//        {
//            goto mylabel;
//        }
    }
    else
    {
    mylabel:
        auto* focus = _WorldSystem.focusedRigidBody();
        if (focus)
        {
            Vector3F offset = _camera.localView()->getPosition().offSetTo(focus->getGPSLocation());
            Vector3D velocity = focus->toNonLocalGroundFrame(focus->velocityBody());

            QuarternionD qCamera = MathSupport<double>::MakeQ(_camera.localView()->getOrientation());
            offset = QVRotate(~qCamera.toFloat(), offset);
            velocity = QVRotate(~qCamera, velocity);
/*
            if (_view == Camera::FreeLookingAtPlane)
                alSource3f(_soundEngineSource, AL_POSITION, 0, 0, offset.Magnitude());
            else if (_view == Camera::Chase || _view == Camera::ChaseRotate)
                alSource3f(_soundEngineSource, AL_POSITION, 0, 0, fabs(_chaseDistance));
            else
                alSource3f(_soundEngineSource, AL_POSITION, offset.x, offset.y, offset.z);

            alSource3f(_soundEngineSource, AL_VELOCITY, velocity.x, velocity.y, velocity.z);
*/

        }
    }

//    if (bPlaneActive && _view == Camera::CockpitForwards)
//    {
//        ALfloat pitchValue;

//        if (pAir->_left_engine._actual_thrust_percent < 30)
//            pitchValue = pAir->_left_engine._actual_thrust_percent / 30.0;
//        else
//            pitchValue = (pAir->_left_engine._actual_thrust_percent - 30) / 70.0 * 1.0 + 1.0;

//        alSourcef(_soundEngineSource, AL_PITCH, pitchValue);
//    }
//    else
//    {
//        if (pAir)
//        {
//            ALfloat pitchValue;

//            if (pAir->_left_engine._actual_thrust_percent < 30)
//                pitchValue = pAir->_left_engine._actual_thrust_percent / 30.0;
//            else
//                pitchValue = (pAir->_left_engine._actual_thrust_percent - 30) / 120.0 * 1.0 + 1.0;

//            alSourcef(_soundEngineSource, AL_PITCH, pitchValue);
//        }
//    }

    {
        JSONRigidBody *pRigidBody = _WorldSystem.focusedRigidBody();
        if( pRigidBody )
        {
            const Vector3D& euler = pRigidBody->getEuler();
            const Vector3D& pos = pRigidBody->position();
            const Vector3D& vel = pRigidBody->velocity();

            _pfdInstrument._fPitch = -euler.x;
            _pfdInstrument._fBank = euler.z;
            _pfdInstrument._fHdg = euler.y;

            HeightData data;
            _pfdInstrument._fAlt = pRigidBody->Height() * 3.2808 - 2;

            if (_pfdInstrument._fAlt < 762.0f && _WorldSystem.getHeightFromPosition(pRigidBody->getGPSLocation(), data))
                _pfdInstrument._fAlt = data.Height() * 3.2808 - 2;

            Vector3D vWind = _WorldSystem.getWeather()->getWindFromPosition(Vector3D());
            _pfdInstrument._fAirSpd = AirProperties::Airspeed((vel-vWind).Magnitude() * 3600.0/1000.0/1.60934/1.15, pRigidBody->Height(), 0 );
            _pfdInstrument._fVSI = vel * pos.Unit() * 3.2808 * 60.0;
        }
    }

    //_WorldSystem.updateCameraView(_camera.getView());
}

#ifndef LOCATED_AT_LONDON
void SDLMainWindow::RenderReflection()
{
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    _openGLFrameBuffer.bind();
    _openGLFrameBuffer.attachDepthRenderBuffer(_depthRenderBuffer);
    _openGLFrameBuffer.attachColorTexture2D(0, _reflectionTexture);

    if (_openGLFrameBuffer.checkFrameBufferStatusComplete() != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "checkFrameBufferStatusComplete failed - RenderReflection" << std::endl;
        postQuit();
        return;
    }

    GLenum drawbuffers[] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, drawbuffers);

    glViewport(0, 0, _reflectionTexture.width(), _reflectionTexture.height());

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);

    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    OpenGLMatrixStack &view = OpenGLPipeline::Get(0).GetView();
    view.Push();
        view.LoadIdentity();
        OpenGLPipeline::Get(0).setViewZoom(_camera.localView()->getZoom());
        setupCameraOrientation();
        setupCameraPosition(true);

        _renderer->dt = frameTime();
        _renderer->camID = 0;
        _renderer->useProgram(_reflectionShaderProgram);

        _renderer->fLightingFraction = _WorldSystem.getLightFraction();

        //_WorldSystem.RenderTerrain(true, false , _reflectionShaderProgram, 0, 0, _camera._location, frameTime());
        _WorldSystem.RenderModels(_renderer, false, 0, frameTime());
        OpenGLShaderProgram::useDefault();

    view.Pop();

    _openGLFrameBuffer.attachColorTexture2D(0, 0);
    _openGLFrameBuffer.attachDepthRenderBuffer(0);
    _openGLFrameBuffer.unbind();
    glEnable(GL_BLEND);

    glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
}
#endif

void SDLMainWindow::RenderDepthTextures(int camID, OpenGLTexture2D& shadowMap, OpenGLTexture2D& colorMap, int offset, int size)
{
    OpenGLMatrixStack &view = OpenGLPipeline::Get(camID).GetView();

    Vector3F cam(_camera.localView()->getOrientation().x, 0, _camera.localView()->getOrientation().z);
    Vector3F light(_lightDir.x, 0, _lightDir.z);
    cam.Normalize();
    light.Normalize();
    //float fDot = fabs(cam * light);

    float hSize = size / 2.0f;//  +_WorldSystem.getLightTanAngle() * 500 * fDot * camID;

    OpenGLPipeline::Get(camID).GetProjection().LoadIdentity();
    OpenGLPipeline::Get(camID).GetProjection().SetOrthographic(-hSize, hSize, -hSize, hSize, 1, 1000);

    //go to light's positon
    view.LoadIdentity();
    view.Translate(0, 0, 500);

    view.Top().RotateX(_lightDir.x + 90);
    view.Top().RotateY(-_lightDir.z + 180);

    view.Top().RotateModel(_camera.localView()->getOrientation());
    view.Translate(0, 0, -(offset + hSize));
    view.Top().RotateView(_camera.localView()->getOrientation());
    view.TranslateLocation(_camera.localView()->getPosition());

    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    _openGLFrameBuffer.bind();
    _openGLFrameBuffer.attachDepthTexture2D(shadowMap);
    _openGLFrameBuffer.attachColorTexture2D(0, colorMap);

    if (_openGLFrameBuffer.checkFrameBufferStatusComplete() != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "checkFrameBufferStatusComplete failed : RenderDepthTextures" << std::endl;
        postQuit();
        return;
    }

    //GLenum drawbuffers[] = { GL_COLOR_ATTACHMENT0 };
    //glDrawBuffers(1, drawbuffers);

    glViewport(0, 0, shadowMap.width(), shadowMap.height());
    glEnable(GL_DEPTH_TEST);

    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

    _renderer->dt = frameTime();
    _renderer->camID = camID;
    _renderer->useProgram( _simpleShaderProgram);
    _renderer->fLightingFraction = _WorldSystem.getLightFraction();

    _WorldSystem.RenderModels(_renderer, false, 0, frameTime());

    glDisable(GL_CULL_FACE);

    _openGLFrameBuffer.attachDepthTexture2D(0);
    _openGLFrameBuffer.attachColorTexture2D(0, 0);

    _openGLFrameBuffer.unbind();

    OpenGLShaderProgram::useDefault();
    glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
}

//Move to OpenGLPipeline
void SDLMainWindow::sendDataToShader(OpenGLShaderProgram& progID, int slot, int width, int height)
{
    progID.sendUniform("texID", 0);
    progID.sendUniform("useTex", 1);
    progID.sendUniform("debug", global_fg_debug == true);
    progID.sendUniform("shadowMap1", slot);
#ifndef LOCATED_AT_LONDON
    progID.sendUniform("dims", 1.0f / width, 1.0f / height);
    progID.sendUniform("shadowMap2", slot + 1);
    progID.sendUniform("shadowMap3", slot + 2);
    progID.sendUniform("reflectionMap", slot + 3);
#endif
    Vector4F eyeLightDirection = _WorldSystem.getEyeLightDirection();
    progID.sendUniform("eyeLightDirection", eyeLightDirection.x, eyeLightDirection.y, eyeLightDirection.z);
}

void SDLMainWindow::RenderScene()
{
#ifndef LOCATED_AT_LONDON
    RenderReflection();
#endif

    RenderDepthTextures(1, _shadowDepthMap1, _shadowTextureMap1, 0, 150*4);
#ifndef LOCATED_AT_LONDON
    RenderDepthTextures(2, _shadowMap2, _shadowMapTexture2, 145, 500);
    RenderDepthTextures(3, _shadowMap3, _shadowMapTexture3, 495, 2000);
#endif

    int width, height;
    GetScreenDims(width, height);

    int slot = 2;
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, _shadowDepthMap1);

#ifndef LOCATED_AT_LONDON
    glActiveTexture(GL_TEXTURE0 + slot + 1);
    glBindTexture(GL_TEXTURE_2D, _shadowMap2);

    glActiveTexture(GL_TEXTURE0 + slot + 2);
    glBindTexture(GL_TEXTURE_2D, _shadowMap3);

    glActiveTexture(GL_TEXTURE0 + slot + 3);
    glBindTexture(GL_TEXTURE_2D, _reflectionTexture);

    glActiveTexture(GL_TEXTURE0 + slot + 4);
    glBindTexture(GL_TEXTURE_2D, _normalMap);

    glActiveTexture(GL_TEXTURE0 + slot + 5);
    glBindTexture(GL_TEXTURE_2D, _dUdVMap);
#endif

    glActiveTexture(GL_TEXTURE0);

    _renderer->dt = frameTime();
    _renderer->camID = 0;
    _renderer->useProgram( _shadowShaderProgram);

    sendDataToShader(_shadowShaderProgram, slot, width, height);

    _renderer->fLightingFraction = _WorldSystem.getLightFraction();

    _WorldSystem.RenderTerrain( _renderer, true, false,
                            #ifndef LOCATED_AT_LONDON
                                3,
                            #else
                                1,
                            #endif
                                _camera.localView()->getPosition());

    glEnable(GL_BLEND);

    _WorldSystem.RenderModels( _renderer, false,
                           #ifndef LOCATED_AT_LONDON
                               3,
                           #else
                               1,
                           #endif
                               frameTime());

    glDisable(GL_BLEND);

#ifndef LOCATED_AT_LONDON
    _renderer->useProgram(_waterShaderProgram);

    sendDataToShader(_waterShaderProgram, slot, width, height);

    //Move To Pipeline
    // To do water shader parameters.
    _waterShaderProgram.sendUniform("nUVOffset", _nUVOffset);
    _waterShaderProgram.sendUniform("normalMap", slot + 4);
    _waterShaderProgram.sendUniform( "dudvMap", slot + 5);

    glDisable(GL_BLEND);

    _WorldSystem.RenderTerrain( _renderer, false, false, 3, _camera.localView()->getPosition());
#endif

    glEnable(GL_BLEND);

#ifndef LOCATED_AT_LONDON
    for (int i = slot+5; i >=0; --i)
#else
    for (int i = slot+2; i >=0; --i)
#endif
    {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    _renderer->useProgram( _simplePrimitiveShaderProgram);

    if (global_force_lines_debug == __global_force_lines_debug::force_lines_aero_force ||
        global_force_lines_debug == __global_force_lines_debug::force_lines_aero_force_and_wind_tunnel)
        _WorldSystem.RenderModelForceGenerators(_renderer);
}

void SDLMainWindow::onRender()
{
    OpenGLPipeline& pipeline = OpenGLPipeline::Get(0);

    SDL_Rect rect;
    GetScreenDims(rect.w, rect.h);
    GetScreenPos(rect.x, rect.y);

    int cx, cy;
    cx = rect.w;
    cy = rect.h;

    pipeline.Push();

    pipeline.GetView().LoadIdentity();
    pipeline.setViewZoom(_camera.localView()->getZoom());

    int dY= 0;
    float fFOV = 70.0f;

    pipeline.GetProjection().LoadIdentity();
    pipeline.GetProjection().SetPerspective(fFOV * cy / cx , float(cx) / cy, 0.1f, 1000000);
    glViewport(0,dY, cx, cy);

    setupCameraOrientation();
    _WorldSystem.setLightDirection(_lightDir.x, _lightDir.z);
    glClearColor(80.0/255, 121.0/255, 168.0/255, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    OnRenderSkyBox();

    OnInitPolyMode();
    setupCameraPosition();

    RenderScene();

    OpenGLShaderProgram::useDefault();

    cx = rect.w;
    cy = rect.h;
    glViewport(0,0, cx, cy);

    std::string desc = _WorldSystem.getCameraDescription();
    std::transform(desc.begin(), desc.end(), desc.begin(), ::tolower);
    if( desc.find("cockpit") != std::string::npos)
    {
        GLint viewport[4];
        glGetIntegerv(GL_VIEWPORT, viewport);

        _openGLFrameBuffer.bind();
        _openGLFrameBuffer.attachColorTexture2D(0, _pfdColorTexture);
        _openGLFrameBuffer.attachStencilRenderBuffer(_pfdStencilBuffer);

        if (_openGLFrameBuffer.checkFrameBufferStatusComplete() != GL_FRAMEBUFFER_COMPLETE)
        {
            std::cout << "checkFrameBufferStatusComplete failed : RenderDepthTextures" << std::endl;
            postQuit();
            return;
        }

        glViewport(0, 0, _pfdColorTexture.width(), _pfdColorTexture.height());

        OpenGLPainter painter;
        painter.selectFontRenderer(&_fontRenderer);
        painter.selectPrimitiveShader(&_simpleColorPrimitiveShaderProgram);

        _pfdInstrument.render(&painter, _pfdColorTexture.width(), _pfdColorTexture.height());

        _openGLFrameBuffer.attachColorTexture2D(0, 0);
        _openGLFrameBuffer.attachStencilRenderBuffer(0);
        _openGLFrameBuffer.unbind();

        glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
        RenderTexture(_pfdColorTexture, 0.5f, 0.5f);
    }

    if (_WorldSystem.isUsingMouse())
        RenderMouseFlying(cx, cy);

    RenderDrivingPower();

    if (global_info)
        RenderInfo();
    else
        RenderFPS();
    if( global_fg_debug )
    {
        //RenderTexture(_reflectionTexture, 0);
        RenderTexture(_shadowTextureMap1, 0, 1);

#ifndef LOCATED_AT_LONDON
        RenderTexture(_shadowMapTexture2, 1);
        RenderTexture(_shadowMapTexture3, 2);
#endif
    }

    pipeline.Pop();
}

void SDLMainWindow::RenderDrivingPower()
{
    glEnable(GL_BLEND);

    _renderer->useProgram(_textureShaderProgram);
    _renderer->progId().sendUniform("texID", 0);
    _buttonTextureManager.render();

    _renderer->useProgram(_simplePrimitiveShaderProgram);
    _powerSliderControl.render(_renderer);

    OpenGLShaderProgram::useDefault();
    glDisable(GL_BLEND);
}

void SDLMainWindow::RenderTransparentRectangle(int x, int y, int cx, int cy, float R, float G, float B, float A)
{
    OpenGLPipeline& pipeline = OpenGLPipeline::Get(0);
    pipeline.Push();
    pipeline.GetView().LoadIdentity();
    pipeline.GetModel().LoadIdentity();
    pipeline.GetProjection().LoadIdentity();
    int w, h;
    GetScreenDims(w, h);
    pipeline.GetProjection().SetOrthographic(0, w, h, 0, -1, 1);

    glDisable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    _renderer->useProgram(_simplePrimitiveShaderProgram);
    pipeline.bindMatrices(_renderer->progId());

    float vertices[] = {
        x, y, 0,
        x+cx, y, 0,
        x+cx, y+cy, 0,
        x, y + cy, 0
    };


    float colors[] = {
        R, G, B, A,
        R, G, B, A,
        R, G, B, A,
        R, G, B, A
    };

    _renderer->bindVertex(Renderer::Vertex, 3, vertices);
    _renderer->bindVertex(Renderer::Color, 4, colors);

    _renderer->setVertexCountOffset( indicesCount(vertices,3));
    _renderer->setPrimitiveType(GL_TRIANGLE_FAN);

    _renderer->Render();
    _renderer->unBindBuffers();

    pipeline.Pop();

    glDisable(GL_BLEND);

    OpenGLShaderProgram::useDefault();

    glEnable(GL_DEPTH_TEST);
}

void SDLMainWindow::RenderMouseFlying(float cx, float cy)
{
    glDisable(GL_DEPTH_TEST);

    _renderer->dt = frameTime();
    _renderer->camID = 0;
    _renderer->useProgram( _simplePrimitiveShaderProgram);

    OpenGLPipeline& pipeline = OpenGLPipeline::Get(_renderer->camID);

    pipeline.Push();
    pipeline.GetProjection().LoadIdentity();
    pipeline.GetProjection().SetOrthographic(0, cx, cy, 0, -1, 1);
    pipeline.GetModel().LoadIdentity();
    pipeline.GetView().LoadIdentity();

    pipeline.bindMatrices(_renderer->progId());

    float divide = 10.0f;

    {
        float vertices[] = {
            cx / 2 - cx / divide, cy / 2 - cy / divide, 0,
            cx / 2 + cx / divide, cy / 2 - cy / divide, 0,
            cx / 2 + cx / divide, cy / 2 + cy / divide, 0,
            cx / 2 - cx / divide, cy / 2 + cy / divide, 0
        };

        float colors[] = {
            1,1,1,1,
            1,1,1,1,
            1,1,1,1,
            1,1,1,1,
        };

        _renderer->bindVertex(Renderer::Vertex, 3, vertices);
        _renderer->bindVertex(Renderer::Color, 4, colors);
        _renderer->setPrimitiveType(GL_LINE_LOOP);
        _renderer->setUseIndex(false);
        _renderer->setVertexCountOffset(indicesCount(vertices,3));

        _renderer->Render();

    }

    {
        float vertices[] = {
            cx / 2 - 10, cy / 2, 0,
            cx / 2 + 10, cy / 2, 0,
            cx / 2, cy / 2 - 10, 0,
            cx / 2, cy / 2 + 10, 0,
        };

        float colors[] = {
            1,1,1,1,
            1,1,1,1,
            1,1,1,1,
            1,1,1,1,
        };

        _renderer->bindVertex(Renderer::Vertex, 3, vertices);
        _renderer->bindVertex(Renderer::Color, 4, colors);
        _renderer->setPrimitiveType(GL_LINES);
        _renderer->setUseIndex(false);
        _renderer->setVertexCountOffset(indicesCount(vertices,3));
        _renderer->Render();

    }

    POINT pt = {};
    GetCursorPos(&pt);
    ScreenToClient(_hWnd, &pt);

    {
        float vertices[] = {
            pt.x - 5, pt.y - 5, 0,
            pt.x + 5, pt.y - 5, 0,
            pt.x + 5, pt.y + 5, 0,
            pt.x - 5, pt.y + 5, 0
        };

        float colors[] = {
            1,1,1,1,
            1,1,1,1,
            1,1,1,1,
            1,1,1,1,
        };

        _renderer->bindVertex(Renderer::Vertex, 3, vertices);
        _renderer->bindVertex(Renderer::Color, 4, colors);
        _renderer->setPrimitiveType(GL_LINE_LOOP);
        _renderer->setUseIndex(false);
        _renderer->setVertexCountOffset(indicesCount(vertices,3));
        _renderer->Render();

    }

    pipeline.Pop();
    glEnable(GL_DEPTH_TEST);
}

void SDLMainWindow::RenderTexture(OpenGLTexture2D& texID, float U, float V)
{
    int sw, sh;
    GetScreenDims(sw, sh);

    int width = 150 * float(sh)/sw;
    int height = 150;
    glEnable(GL_BLEND);

    _renderer->useProgram(_fontShaderProgram);
    _renderer->progId().sendUniform("texID", 0);
    _renderer->progId().sendUniform("textColor", 1, 1, 1, 0.25);

    OpenGLTextureRenderer2D r(_renderer);
    r.onSize(0, 0, 200, 200);
    r.beginRender();
    texID.bind();
    r.render(200*U-width/2, 200*V-height/2, width, height);

//    _renderer->useProgram(_simplePrimitiveShaderProgram);
//    r.renderLineBorder( size*pos, 200-size, size, size);

    r.endRender();

    glDisable(GL_BLEND);
    OpenGLTexture2D::unBind();
    OpenGLShaderProgram::useDefault();
}

void SDLMainWindow::RenderFPS()
{
    glEnable(GL_BLEND);
    _renderer->dt = frameTime();
    _renderer->camID = 0;

    _fontRenderer.selectFont(&_myFontTexture);
    _fontRenderer.setFontColor(Vector4F(1,1,1,1));

    int w, h;
    GetScreenDims(w, h);

    OpenGLPipeline& p = OpenGLPipeline::Get(_renderer->camID);
    p.Push();
    OpenGLPipeline::applyScreenProjection(p, 0, 0, w, h);
    p.GetModel().Translate(0, 30,0);

    _fontRenderer.beginRender();

    float fps = 0.0f;
    for( int i=0; i < FPS_RESOLUTION; i++ )
        fps += _framerate[i];

    char text[256]="";
    sprintf_s( text, _countof(text), "%.0f", fps / FPS_RESOLUTION);
    _fontRenderer.renderText( 15, 5, text );

    _fontRenderer.endRender();
    p.Pop();
    glDisable(GL_BLEND);
}

void SDLMainWindow::RenderInfo()
{
    int w, h;
    GetScreenDims(w, h);

    RenderTransparentRectangle(0,0, w/2, h/2, 0, 0, 1, 0.5f);

    float fps = 0.0f;

    if( isRunning() )
        for( int i=0; i < FPS_RESOLUTION; i++ )
            fps += _framerate[i];

    glEnable(GL_BLEND);
    _renderer->dt = frameTime();
    _renderer->camID = 0;

    OpenGLPipeline& p = OpenGLPipeline::Get(_renderer->camID);
    p.Push();
    OpenGLPipeline::applyScreenProjection(p, 0, 0, w, h);
    p.GetModel().Translate(0, 30,0);

    _fontRenderer.selectFont(&_myFontTexture);
    _fontRenderer.setFontColor(Vector4F(1,1,1,1));
    _fontRenderer.beginRender();

    _fontRenderer.renderText( 15, 15, "3D Virtual World by Mo" );
    _fontRenderer.renderText( 15, 30, "------------------------------------------" );

    static char *Version = (char*)glGetString(GL_VERSION);
    static char *Extensions = (char*)glGetString(GL_EXTENSIONS);
    static char *WglExtensionsExt = (char*)wglGetExtensionsStringEXT();
    static char *Renderer = (char*)glGetString(GL_RENDERER);
    static char *Vendor = (char*)glGetString(GL_VENDOR);
    static char Extensions2[51200];

    char text[1024]= {};
    JSONRigidBody *pRigidBody = _WorldSystem.focusedRigidBody();

    sprintf_s( text, _countof(text), "OpenGL [%s], Vendor [%s], Renderer[%s]", Version, Vendor, Renderer );
    _fontRenderer.renderText( 15, 45, text );

    std::string strFocus;
    if (pRigidBody)
        strFocus = pRigidBody->getID();
    else
        strFocus = "##NONE##";

    sprintf_s( text, _countof(text), "Object [%s], Running [%d]", strFocus.c_str(), isRunning() );
    _fontRenderer.renderText( 15, 60, text );

    GPSLocation cameraGPS(_camera.localView()->getPosition());
    Vector3F orientation = _camera.localView()->getOrientation();

    sprintf_s( text, _countof(text), "FPS :%d: Q: [%.2f,%.2f,%.2f]",
                        int(fps / FPS_RESOLUTION),
                        orientation.x,
                        orientation.y,
                        orientation.z);
    _fontRenderer.renderText( 15, 75, text );

    sprintf_s( text, _countof(text), "CameraPosition: [%.8f N, %.8f E] (%.2f)", cameraGPS._lat, cameraGPS._lng, cameraGPS._height );
    _fontRenderer.renderText( 15, 90, text );

    if( pRigidBody )
    {
        Vector3D pos = pRigidBody->position();
        Vector3D vel = pRigidBody->velocity();

        Vector3D e = pRigidBody->getEuler();
        Vector3D vb = pRigidBody->velocityBody();

        GPSLocation gpsLocation(pRigidBody->position());

        sprintf_s( text, _countof(text), "ModelPosition: [%.8f N, %.8f E] (%.2f)", gpsLocation._lat, gpsLocation._lng, gpsLocation._height );

        _fontRenderer.renderText( 15, 105, text );

        double velMag = vel.Magnitude();

        sprintf_s( text, _countof(text), "Vel: [%.2f,%.2f,%.2f] (Kts:%.2f)(Mph:%.2f)", vel.x, vel.y, vel.z, MS_TO_KTS(velMag), MS_TO_MPH(velMag) );
        _fontRenderer.renderText( 15, 120, text );

        //MOJ_JEB
        /*if( _audiA8.hasFocus() )
            sprintf_s( text, _countof(text), "E [%.2f, %.2f, %.2f], Speed : (%4.2fmph)", e.x, e.y, e.z, MS_TO_MPH(vel.Magnitude()) );
        else
            sprintf_s( text, _countof(text), "E [%.2f, %.2f, %.2f], V [%.2f, %.2f, %.2f](%4.2fkts), VB [%.2f, %.2f, %.2f]",
                e.x, e.y, e.z, vel, MS_TO_KTS(vel.Magnitude()), vb.x, vb.y, vb.z  );*/

        //_oglFont.RenderFont( 15, 105, text );

        //sprintf_s( text, _countof(text), "" );

        //MOJ_JEB
        /*if( _WorldSystem.focusedRigidBody() == &_airbusAircraft )
        {
            sprintf_s( text, _countof(text), "Steering Ang [%.2f] Vel [%4.2fkts] -- L[%4.2fkts]R[%4.2fkts], Engine L :%.1f: R :%.1f:",
                _airbusAircraft._front_wheel.spring().getSteeringAngle(), MS_TO_KTS(_airbusAircraft._front_wheel.spring().getWheelSpeed()), MS_TO_KTS(_airbusAircraft._left_wheel.spring().getWheelSpeed()), MS_TO_KTS(_airbusAircraft._right_wheel.spring().getWheelSpeed()),
                _airbusAircraft._left_engine._actual_thrust_percent, _airbusAircraft._right_engine._actual_thrust_percent
                );
        }

        if( _WorldSystem.focusedRigidBody() == &_audiA8 )
        {
            const HeightData & hd = _audiA8._leftFrontWheel.spring().getHeightData();
            sprintf_s( text, _countof(text), "sepDist : [%.8f]",
                _audiA8._leftFrontWheel.spring().separatorDistance() );
        }*/

        //_oglFont.RenderFont( 15, 135, text );

        const Vector3D& angVel = pRigidBody->angularVelocity();
        const Vector3D& cg = pRigidBody->cg();

        sprintf_s( text, _countof(text), "Ang V[%.2f,%.2f,%.2f] CG[%.2f,%.2f,%.2f]", angVel.x, angVel.y, angVel.z, cg.x, cg.y, cg.z );
        _fontRenderer.renderText( 15, 135, text );


        double dHeightOfGround(0.0);
        QuarternionD qPlane;

        HeightData data;
        if( _WorldSystem.getHeightFromPosition(cameraGPS, data ) )
        {
            Vector3F euler = MathSupport<float>::MakeEuler( data.qNormal() ) ;
            sprintf_s( text, _countof(text), "HeightAbovePlane :[%.8f], Euler : [%.2f,%.2f,%.2f]", data.Height(),euler.x, euler.y, euler.z );
        }
        else
            sprintf_s( text, _countof(text), "HeightAbovePlane :[NOT AVAILABLE]");

        _fontRenderer.renderText( 15, 150, text );

        sprintf_s(text, _countof(text), "View [%d]:%s: Zoom :%.2f:", _WorldSystem.curViewIdx(), _WorldSystem.getCameraDescription().c_str(), _camera.localView()->getZoom());
        _fontRenderer.renderText( 15, 165, text );

        Vector3D acceleration = pRigidBody->getForce() / pRigidBody->getMass();
        double gForce = 1+acceleration.y / pRigidBody->gravity().y;

        JSONRigidBody::STATE state = JSONRigidBody::STATE::NORMAL;
        state = pRigidBody->getState();

        sprintf_s(text, _countof(text), "G :[%.2f], FlightRec [%s]", gForce,
            state ==JSONRigidBody::STATE::NORMAL ? "Normal": (state ==JSONRigidBody::STATE::RECORDING ? "Recording": "Playback" ) );
        _fontRenderer.renderText( 15, 180, text );

        sprintf_s( text, _countof(text), "Camera-Object distance :%.2f: miles", _WorldSystem.focusedRigidBody()->getGPSLocation().distanceTo( _camera.localView()->getPosition() )*3.2808/5280 );
        _fontRenderer.renderText( 15, 195, text );

        if (state == JSONRigidBody::STATE::PLAYBACK)
        {
            sprintf_s(text, _countof(text), "Playback-> %3.2f/%3.0fsecs",
                                        pRigidBody->getFlightRecorder().timeSoFar(),
                                        pRigidBody->getFlightRecorder().totalTime()
                                            );
            _fontRenderer.renderText(15, 210, text);
        }

        /*auto *rigidBody = _WorldSystem.focusedRigidBody();

        if (rigidBody && rigidBody->getName().substr(0, 3) == "Air")
        {
            sprintf_s( text, _countof(text), "Airbus heights [%.2f][F:%.2f, L:%.2f,R:%.2f] ",
                                    _airbusAircraft.Height(),
                                    _airbusAircraft._front_wheel.Height(),
                                    _airbusAircraft._left_wheel.Height(),
                                    _airbusAircraft._right_wheel.Height() );

            _oglFont.RenderFont( 15, 195, text );
        }*/
    }

    _fontRenderer.endRender();
    OpenGLShaderProgram::useDefault();
    glDisable(GL_BLEND);
    p.Pop();
}

void SDLMainWindow::setupCameraOrientation()
{
    OpenGLMatrixStack & mat = OpenGLPipeline::Get(0).GetView();
    mat.Rotate(_camera.localView()->getOrientation());
}

void SDLMainWindow::setupCameraPosition(bool bReflection)
{
    OpenGLMatrixStack & mv = OpenGLPipeline::Get(0).GetView();

    GPSLocation pos = _camera.localView()->getPosition();

    if (bReflection)
    {
        mv.Translate(0, pos._height, 0);
        mv.Scale(1, -1, 1);
        mv.Translate(0, -pos._height, 0);
    }

    mv.TranslateLocation(pos);
}

void SDLMainWindow::processInputsForCamera()
{
    bool bShiftOn = GetAsyncKeyState(VK_SHIFT) < 0;
    bool bControlOn = GetAsyncKeyState(VK_CONTROL) < 0;

    float jump = (bShiftOn ? 100 : 2);

    if (bControlOn)
        jump /= 10;

    if( ::GetAsyncKeyState(VK_LEFT) < 0)
        _camera.incrOrientation(0, 0,  -(bShiftOn ? 5 : 0.5));

    if( ::GetAsyncKeyState(VK_RIGHT) < 0)
        _camera.incrOrientation(0, 0, (bShiftOn ? 5 : 0.5));

    if( ::GetAsyncKeyState(VK_UP) < 0)
        _camera.incrOrientation((bShiftOn ? 5 : 0.5), 0, 0);

    if( ::GetAsyncKeyState(VK_DOWN) < 0)
        _camera.incrOrientation(- (bShiftOn ? 5 : 0.5), 0, 0);

    if( ::GetAsyncKeyState('Z') < 0 || ::GetAsyncKeyState('X') < 0)
    {
        int iFactor = ::GetAsyncKeyState('Z') <0 ? -1 : 1;
        _camera.incrOrientation(0, iFactor * (bShiftOn ? jump / 10 : (bControlOn ? jump * 20 : jump)),0);
    }

    if( ::GetAsyncKeyState('Q') < 0)
        _camera.moveForwards(jump);

    if( ::GetAsyncKeyState('A') < 0)
        _camera.moveForwards(-jump);

    if( ::GetAsyncKeyState('C') < 0)
        _camera.moveForwards(jump * 2, -90);

    if( ::GetAsyncKeyState('V') < 0)
        _camera.moveForwards(jump * 2, 90);

    if( ::GetAsyncKeyState('W') < 0)
        _camera.moveUp(jump/4);

    if( ::GetAsyncKeyState('S') < 0)
        _camera.moveUp(-jump/4);

    if( ::GetAsyncKeyState(VK_SPACE) < 0)
        _camera.setZOrientation(0.0f);

    if (::GetAsyncKeyState(VK_OEM_MINUS) < 0 )
        _camera.incrZoom( -0.1f);

    if (::GetAsyncKeyState(VK_OEM_PLUS) < 0)
        _camera.incrZoom( 0.1f);
}

void SDLMainWindow::persistSettings(bool bSerialise)
{
    using namespace rapidjson;

    std::string strSettingsFilename = "lastpos.json";

    Document doc;
    Document::AllocatorType& a = doc.GetAllocator();

    if( bSerialise)
    {
        FILE *fPersistFile = NULL;
        fopen_s( &fPersistFile, strSettingsFilename.c_str(), "wb" );
        if(fPersistFile)
        {
            doc.SetObject();
            doc.AddMember("UserPolyLineView", Value(_bUserPolygonLineView), a);
            doc.AddMember("IsRunning", Value(isRunning()), a);
            doc.AddMember("global_fg_debug", Value(global_fg_debug), a);
            doc.AddMember("global_info", Value(global_info), a);
            doc.AddMember("global_force_lines_debug", Value((int)global_force_lines_debug), a);

            _WorldSystem.persistWriteState( doc );

            StringBuffer s;
            PrettyWriter<StringBuffer> writer(s);
            doc.Accept(writer);

            fwrite(s.GetString(), s.GetSize(), 1, fPersistFile);

            fclose(fPersistFile);
        }
    }
    else
    {
        std::ifstream json;
        json.open(strSettingsFilename);

        if( json.is_open())
        {
            std::string ret;
            char buffer[4096];
            while (json.read(buffer, sizeof(buffer)))
                ret.append(buffer, sizeof(buffer));
            ret.append(buffer, json.gcount());

            if( ret.length() > 0)
            {

                doc.Parse(ret.c_str());

                _bUserPolygonLineView = doc["UserPolyLineView"].GetBool();
                setRunning(doc["IsRunning"].GetBool());
                global_fg_debug = doc["global_fg_debug"].GetBool();
                global_info = doc["global_info"].GetBool();
                global_force_lines_debug = (__global_force_lines_debug)doc["global_force_lines_debug"].GetInt();

                _WorldSystem.persistReadState(doc);
            }
        }
    }

}

void SDLMainWindow::OnInitPolyMode()
{
    if( _bUserPolygonLineView )
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE );
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL );
}

void SDLMainWindow::OnRenderSkyBox()
{
    _renderer->camID = 0;
    _renderer->useProgram(_fontShaderProgram);
    _fontShaderProgram.sendUniform("textColor", 1,1,1,1);

    OpenGLPipeline& pipeline = OpenGLPipeline::Get(_renderer->camID);
    pipeline.bindMatrices(_renderer->progId());
    _renderer->bindTexture(&_skyDomeTexture);
    glDepthMask(GL_FALSE);
    _skyDome->Render(_renderer);
    glDepthMask(GL_TRUE);
    _renderer->unBindTexture();
}
