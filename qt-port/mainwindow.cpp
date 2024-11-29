#include "mainwindow.h"

#include <iostream>
#include <algorithm>

#include <QTime>
#include <QThread>

#include <QDebug>
#include <QPainter>
#include <QKeyEvent>
#include <QGuiApplication>
#include <QRect>
#include <QSize>
#include <QEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QCloseEvent>
#include <QDir>

#include <AndroidAssetsTransfer.inl>

MainWindow::MainWindow() :
    _bUserPolygonLineView(false),
    _showCursor(true),
    //_paused(false),
    _framecount(0),
    _view(Camera::Free),
    _chaseAngle(0.0),
    _chaseHeight(0.0),
    _chaseDistance(30.0),
    _soundDevice(0),
    _soundContext(0),
    _running(true),
    _dt(0.0)

{
    memset( _framerate, 0, sizeof(_framerate) );
}

MainWindow::~MainWindow()
{
}

bool MainWindow::event(QEvent *e)
{
    if( e->type() == QEvent::Close)
        onUnInitialise();

    return QOpenGLWindow::event(e);
}

void MainWindow::onUnInitialise()
{
    //KillTimer(_hWnd,TIMER_MOUSE_HIDE);

    FILE *fPersistFile = NULL;
    fopen_s( &fPersistFile, persistFilename(), "wb" );
    if(fPersistFile)
    {
        fwrite( &_camera, sizeof(_camera), 1, fPersistFile );
        fwrite( &_view, sizeof(_view), 1, fPersistFile );
        fwrite( &_bUserPolygonLineView, sizeof(_bUserPolygonLineView),1 , fPersistFile );
        bool bIsRunning = isRunning();
        fwrite( &bIsRunning, sizeof(bIsRunning), 1, fPersistFile );
        fwrite( &global_fg_debug, sizeof(global_fg_debug),1, fPersistFile );
        fwrite( &global_info, sizeof(global_info),1, fPersistFile );
        fwrite( &global_force_lines_debug, sizeof(global_force_lines_debug), 1, fPersistFile );
        fwrite( &global_cockpit_view, sizeof(global_cockpit_view),1, fPersistFile);
        fwrite( &_chaseAngle, sizeof(_chaseAngle),1 , fPersistFile );
        fwrite( &_chaseHeight, sizeof(_chaseHeight),1 , fPersistFile );
        fwrite( &_chaseDistance, sizeof(_chaseDistance),1 , fPersistFile );

        _WorldSystem.persistWriteState( fPersistFile );

        fclose(fPersistFile);
    }

    _WorldSystem.onUnintialise();
    //	_cameraManager.onUnInitialise();
    OnUnitSound();
    _timer.stop();
}

void MainWindow::setFramePerSecond( double fps)
{
    _fInverseFPS = 1.0 / std::max(1.0,fps);
    //_timer.setInterval(_fInverseFPS * 250);
}

void MainWindow::setRunning( bool bRunning )
{
    _running = bRunning;
    if(_running)
    {
        _timer.start();
        _eTimer.start();
        _lastTime = _eTimer.elapsed();
    }
    else
    {
        _timer.stop();
    }
}

bool MainWindow::isRunning()
{
    return _running;
}

double MainWindow::frameTime()
{
    return _dt;
}

void MainWindow::initializeGL()
{
    showFullScreen();
    copyAssets(false);

    HDC hdc = GetDC( (HWND)winId());

    try
    {
        if (!onCreateFrameBufferAndShaders())
            return;

        _WorldSystem.onInitialise();

#if defined LOCATED_AT_GIBRALTER
        _WorldSystem.addJSONBody(new BAAirbus320JSONRigidBody("AirbusA320:BAA320:A320_A_GIB"));
        _WorldSystem.addJSONBody(new BAAirbus320JSONRigidBody("AirbusA320:AirArabia:A320_B_GIB"));
        //_WorldSystem.addJSONBody(new BAAirbus380JSONRigidBody("AirbusA380:BAA380:A380_A_GIB"));
        //_WorldSystem.addJSONBody(new HarrierJSONRigidBody("Harrier:Harrier:Harrier_A_GIB"));
        _WorldSystem.addJSONBody(new AircraftCarrierJSONRigidBody("AircraftCarrier:Carrier_A_GIB"));
#else
        _WorldSystem.addJSONBody(new BAAirbus320JSONRigidBody("AirbusA320:BAA320:A320_A"));
        _WorldSystem.addJSONBody(new BAAirbus320JSONRigidBody("AirbusA320:BAA320:A320_B"));
        _WorldSystem.addJSONBody(new BAAirbus380JSONRigidBody("AirbusA380:BAA380:BA380_A"));
#endif
        //_WorldSystem.addJSONBody(new BAAirbus380JSONRigidBody("BAA380:BA380_B", &_rigidBodyCollection));
        _WorldSystem.addStaticJSONBody(new HeathrowTowerRigidBody);

        //_cameraManager.onInitialise();

        _WorldSystem.setLightFraction(0.9f);

        _pfdInstrument.Initialise(hdc);

        _WorldSystem.loadBodyRecorderedData();

        FILE *fPersistFile = NULL;
        fopen_s(&fPersistFile, persistFilename(), "rb");
        if (fPersistFile)
        {
            fread(&_camera, sizeof(_camera), 1, fPersistFile);
            fread(&_view, sizeof(_view), 1, fPersistFile);
            fread(&_bUserPolygonLineView, sizeof(_bUserPolygonLineView), 1, fPersistFile);

            bool bIsRunning(false);
            fread(&bIsRunning, sizeof(bIsRunning), 1, fPersistFile);
            setRunning(bIsRunning);
            fread(&global_fg_debug, sizeof(global_fg_debug), 1, fPersistFile);
            fread(&global_info, sizeof(global_info), 1, fPersistFile);
            fread(&global_force_lines_debug, sizeof(global_force_lines_debug), 1, fPersistFile);
            fread(&global_cockpit_view, sizeof(global_cockpit_view), 1, fPersistFile);
            fread(&_chaseAngle, sizeof(_chaseAngle), 1, fPersistFile);
            fread(&_chaseHeight, sizeof(_chaseHeight), 1, fPersistFile);
            fread(&_chaseDistance, sizeof(_chaseDistance), 1, fPersistFile);

            _WorldSystem.persistReadState(fPersistFile);

            fclose(fPersistFile);
        }

        OnInitPolyMode();

        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_NORMALIZE );
        //glEnable(GL_RESCALE_NORMAL);

        glEnable(GL_COLOR_MATERIAL);
        glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
        glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE );

        //_oglFont.CreateBitmapFont( hdc, "Tahoma", 16, FW_BOLD*0+FW_NORMAL*1 );
        _oglFont.CreateBitmapFont( hdc, "Verdana", 16, FW_BOLD*0+FW_NORMAL*1 );
        _oglFont.SetOffSet( 0, 30);

        glClearColor(0.0f,0.0f,0.0f, 1.0f);
        glClearColor(0.25f,0.25f,0.25f,1.0f);
        //glClearColor(0.05f, 0.05f, 0.85f, 1.0f);
        //glClearColor(3 / 255.0f, 166 / 255.0f, 249 / 255.0f, 1.0f);
        glClearColor(_skyColor.x, _skyColor.y, _skyColor.z, _skyColor.w);
        //glClearColor(1, 1, 1, 1);

        glClearStencil(0);

        setTitle("OpenGL Simulation by Mo" );

        //	_heathrowRadar.setPosition( GPSLocation(51.473206,-0.454542) );
        /*_runwayscene.setPosition(GPSLocation(51.46158,-0.459402)+ VectorD(120,0,-325) );
        _runwaylites.setPosition(GPSLocation(51.46158,-0.459402) + GPSLocation(0.0032569999999978450, -0.014778000000000013) );
        _runwaylites.setOrientation(0,90,0);
        _runwaylites2.setPosition(_runwaylites.getGPSLocation() + VectorD(2600,0,0));
        _runwaylites2.setOrientation(0,270,0);
        _londonTile.setPosition(_runwayscene.getGPSLocation() + VectorD(1000,0,-1000)  );
        _simpleCloud.setPosition(_runwayscene.getGPSLocation() +VectorD(0,100,0)  );
        _waterTile.setPosition( GPSLocation(51.46158,-0.459402) + VectorD(0,0,500 ));
    */
        //	_aircraftCarrier.setPosition( _runwayscene.getGPSLocation() + VectorD(0,-15,600 ));
        //	_aircraftCarrier.initSpeedAndPos();

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

        //	_harrierJumpJet.setPosition( _ringRoad.getGPSLocation() + VectorD(150,5,0) );
        //	_harrierJumpJet.setOrientation(0,0,0);

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

        OnInitSound();

    }
    catch (std::string str)
    {
        std::cout << str << " : Error";
    }

    connect(&_timer, &QTimer::timeout, this, &MainWindow::onTimeOut);

#if defined _DEBUG
    setFramePerSecond(30);
#else
    setFramePerSecond(40);
#endif

    //SetTimer( _hWnd, TIMER_MOUSE_HIDE, 5000, NULL );


    glEnable(GL_DEPTH_TEST);
    glClearColor(0,0,1,1);
    ReleaseDC((HWND)winId(), hdc);
}

void MainWindow::OnInitialiseLighting()
{
    OpenGLPipeline::Get(0).Apply();

    float fLightingFraction = _WorldSystem.getLightFraction();

    GLfloat ambientcolor[] = { fLightingFraction, fLightingFraction, fLightingFraction, 1.0f };
    GLfloat speccolor[] = { fLightingFraction, fLightingFraction, fLightingFraction, 1 };

    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientcolor );
    glLightfv(GL_LIGHT0, GL_DIFFUSE, ambientcolor );
    glLightfv(GL_LIGHT0, GL_SPECULAR, speccolor );

    GLfloat lightposition[] = { -0.1,1, 0 ,0 };
    glLightfv(GL_LIGHT0, GL_POSITION, lightposition );
}


bool MainWindow::onCreateFrameBufferAndShaders()
{
    if (!_simpleShaderProgram.loadFiles("shaders/simpleShader.vert", "shaders/simpleShader.frag"))
    {
        std::cout << _simpleShaderProgram.getError() << " : simpleShader Failed";
        return false;
    }

    if (!_shadowShaderProgram.loadFiles("shaders/shadowShader.vert", "shaders/shadowShader.frag"))
    {
        std::cout << _shadowShaderProgram.getError() << " : shadowShader Failed";
        return false;
    }

    if (!_reflectionShaderProgram.loadFiles("shaders/reflectionShader.vert", "shaders/reflectionShader.frag"))
    {
        std::cout << _reflectionShaderProgram.getError() << " : reflectionShader Failed";
        return false;
    }

    if (!_waterShaderProgram.loadFiles("shaders/water.vert", "shaders/water.frag"))
    {
        std::cout << _waterShaderProgram.getError() <<  " : waterShader Failed";
        return false;
    }

    if (!_openGLFrameBuffer.generate())
    {
        std::cout << "Failed to be created. : FrameBuffer Failed";
        return false;
    }

    int f[3] = { 2,2, 4 };
    if (!_shadowMap1.generate(512 * f[0], 512 * f[0], true))
    {
        std::cout << "Failed to be created. : Depth Texure Buffer Failed";
        return false;
    }

    if (!_shadowMap2.generate(512 * f[1], 512 * f[1], true))
    {
        std::cout << "Failed to be created. : Depth Texure 2 Buffer Failed";
        return false;
    }

    if (!_shadowMap3.generate(512 * f[2], 512 * f[2], true))
    {
        std::cout << "Failed to be created. : Depth Texure 3 Buffer Failed";
        return false;
    }

    if (!_shadowMapTexture1.generate(512 * f[0], 512 * f[0], false))
    {
        std::cout << "Failed to be created. : Depth Color Texure Buffer Failed";
        return false;
    }

    if (!_shadowMapTexture2.generate(512 * f[1], 512 * f[1], false))
    {
        std::cout << "Failed to be created. : Depth Color Texure 2 Buffer Failed";
        return false;
    }

    if (!_shadowMapTexture3.generate(512 * f[2], 512 * f[2], false))
    {
        std::cout << "Failed to be created. : Depth Color Texure 3 Buffer Failed";
        return false;
    }

    _openGLFrameBuffer.bind();
    _openGLFrameBuffer.attachColorTexture2D(0, _shadowMapTexture1);

    if (_openGLFrameBuffer.checkFrameBufferStatusComplete() != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "FrameBuffer not complete.  : FrameBuffer Failed";
        return false;
    }

    _openGLFrameBuffer.unbind();

    QSize sz = size();

    if (!_depthRenderBuffer.generate(sz.width(), sz.height()))
    {
        std::cout << "Depth Render buffer not complete. : FrameBuffer Failed";
        return false;
    }

    if (!_reflectionTexture.generate(sz.width(), sz.height(), false))
    {
        std::cout << "Failed to be created. : Reflection Texure";
        return false;
    }

    _dUdVMap.setMinification(GL_LINEAR_MIPMAP_LINEAR);
    _dUdVMap.setWrapMode(GL_REPEAT);

    if (!_dUdVMap.Load(ROOT_APP_TEXTURES_DIRECTORY"dudvmap.bmp"))
    {
        std::cout << "Failed to be load. : dU/dV Texure";
        return false;
    }

    _normalMap.setMinification(GL_LINEAR_MIPMAP_LINEAR);
    _normalMap.setWrapMode(GL_REPEAT);

    if (!_normalMap.Load(ROOT_APP_TEXTURES_DIRECTORY"normalmap.bmp"))
    {
        std::cout << "Failed to be load. : Normal Texure";
        return false;
    }

    return true;
}

void MainWindow::OnInitSound()
{
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
}

void MainWindow::OnUnitSound()
{
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
}

const char* MainWindow::persistFilename() const
{
    return "lastGPSpos.bin";
}

void MainWindow::ensureCameraAboveGround()
{
    HeightData data;
    _WorldSystem.GetHeightFromPosition(_camera._location, data );
    float fMinHeight = 0.05f;

    if( data.Height() < fMinHeight )
        _camera._location._height = _camera._location._height - data.Height() + fMinHeight;
}

void MainWindow::resizeGL(int w, int h)
{
    _oglFont.OnSize(w,h);
    QOpenGLWindow::resizeGL(w, h);
}

void MainWindow::paintGL()
{
    if(!isExposed())
        return;

    QElapsedTimer t;
    t.start();

    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);

    //    QString str = (char*)glGetString(GL_VERSION);

    //    QPainter p(this);

    //    QFont font("Verdana", 12);
    //    QFontMetrics fm(font);
    //    p.setFont(font);
    //    p.setPen(Qt::white);
    //    p.drawText(0, ++lineNo * fm.height(), window != 0 ? window->title() : "None");
    //    p.drawText(0, ++lineNo * fm.height(), "Version : " + str);
    //    p.drawText(0, ++lineNo * fm.height(), "**********" );

    //    p.drawText(0, ++lineNo * fm.height(), "CurrentPath : " + QDir::currentPath());
    //    p.drawText(0, ++lineNo * fm.height(), "HomePath : " + QDir::homePath());
    //    p.drawText(0, ++lineNo * fm.height(), "applicationDirPath : " + QCoreApplication::applicationDirPath());

    //    p.drawText(0, ++lineNo * fm.height(), "applicationName : " + QCoreApplication::applicationName() );
    //    p.drawText(0, ++lineNo * fm.height(), "applicationFilePath : " + QCoreApplication::applicationFilePath() );

    //    p.drawText(0, ++lineNo* fm.height(), "**********" );


    //  p.beginNativePainting();
    glEnable(GL_DEPTH_TEST);
    OnRender();
    // p.endNativePainting();

    //std::cout << "PaintGL : " << t.elapsed() << std::endl;
}

void MainWindow::OnRender()
{
    QElapsedTimer t;
    t.start();

    OpenGLPipeline& stk = OpenGLPipeline::Get(0);

    QSize sz = size();
    float cx = sz.width();
    float cy = sz.height();

    stk.Push();

    stk.GetView().LoadIdentity();
    stk.setViewZoom(_camera._zoom);

    global_cockpit_view = _view == Camera::CockpitForwards && false;
    float h = cx / 4;

    if( global_cockpit_view )
    {
        int width = cx;
        int height = cy - h;
        glViewport(0, h, width,height);
        stk.Push();
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        stk.GetProjection().LoadIdentity();
        stk.GetProjection().SetPerspective(55.0f * height / width,  float(width) / height, 0.1f, 1000000);
        stk.Apply();
        stk.Pop();
        glMatrixMode(GL_MODELVIEW);
    }
    else
    {
        glMatrixMode(GL_PROJECTION);
        stk.GetProjection().LoadIdentity();
        stk.GetProjection().SetPerspective(90.0f * cy / cx, float(cx) / cy, 0.1f, 1000000);
        //stk.GetProjection().SetPerspective(70.0f * cy / cx , float(cx) / cy, 0.1f, 1000000);
        stk.Apply();
        glMatrixMode(GL_MODELVIEW);
    }

    //std::cout << "Render (A) : " << t.restart() << std::endl;

    setupCameraOrientation();
    _WorldSystem.setLightDirection(_lightDir.x, _lightDir.z);
    OnInitialiseLighting();
    OnRenderSkyBox();

    //std::cout << "Render (B) : " << t.restart() << std::endl;

    OnInitPolyMode();

    //std::cout << "Render (B.0) : " << t.restart() << std::endl;
    setupCameraPosition();
    //std::cout << "Render (B.1) : " << t.restart() << std::endl;

    RenderScene();
    //std::cout << "Render (B.2) : " << t.restart() << std::endl;
    RenderSlopes();

    //std::cout << "Render (C) : " << t.restart() << std::endl;

    glViewport(0, 0, cx, cy);

    if( global_cockpit_view )
    {
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
    }

    if( _view == Camera::CockpitForwards )
        _pfdInstrument.OnPaint();

    //std::cout << "Render (D) : " << t.restart() << std::endl;

    if (_WorldSystem.isUsingMouse())
        RenderMouseFlying(cx, cy);

    //std::cout << "Render (E) : " << t.restart() << std::endl;

    RenderDrivingPower(cx, cy);

    //std::cout << "Render (F) : " << t.restart() << std::endl;

    if (global_info)
        RenderInfo();
    else
        RenderFPS();

    //std::cout << "Render (G) : " << t.restart() << std::endl;

    //RenderTexture(_reflectionTexture, 0);
    //RenderTexture(_shadowMapTexture1, 0);
    //RenderTexture(_shadowMapTexture2, 1);
    //RenderTexture(_shadowMapTexture3, 2);


    stk.Pop();

}

void MainWindow::keyPressEvent(QKeyEvent *e)
{
    if( _WorldSystem.onKeyDown(e->key()))
        return;

    if( e->key() == Qt::Key_F11 )
        _WorldSystem.incrLightFraction(-0.01f);

    if( e->key() == Qt::Key_F12 )
        _WorldSystem.incrLightFraction(0.01f);

    if( e->key() == Qt::Key_R)
    {
        setRunning(!isRunning());

        if( !isRunning() )
            update();

        _lastTime = _eTimer.elapsed();
    }

    if( e->key() == Qt::Key_P)
    {
        _bUserPolygonLineView = !_bUserPolygonLineView;
        update();
    }

    if( e->key() == Qt::Key_D)
    {
        if( e->modifiers().testFlag(Qt::ShiftModifier))
            global_info = !global_info;
        else
            global_fg_debug = !global_fg_debug;

    }


    if( !isRunning() )
    {
        _camera.processSyncKeys(e->key(), 2);
        _camera.processAsyncKeys2();

        ensureCameraAboveGround();

        update();
    }

    if( e->key() == Qt::Key_F5)
    {
        /*_towTruck.initSpeedAndPos();
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

        GSRigidBody *pBody = _WorldSystem.focusedRigidBody();

        if( pBody )
            _camera._location = pBody->getGPSLocation() + VectorF(0,0,100);
    }

    if( e->key() == Qt::Key_F6)
    {
        if( e->modifiers().testFlag(Qt::ShiftModifier))
        {
            _view = (Camera::ViewPosition)((int)(_view)-1);

            if( _view == Camera::ViewPositionBegin )
                _view = (Camera::ViewPosition)(Camera::ViewPositionEnd-1);
        }
        else
        {
            _view = (Camera::ViewPosition)((int)(_view)+1);

            if( _view == Camera::ViewPositionEnd )
                _view = (Camera::ViewPosition)(Camera::ViewPositionBegin+1);
        }

        if (_WorldSystem.focusedRigidBody() && _WorldSystem.focusedRigidBody()->getID() == "A320_B")
        {
            if (_view == Camera::ViewPosition::CockpitForwards)
            {
                ALfloat source0Pos[] = { 0.0, 0.0, 1.0 };
                ALfloat source0Vel[] = { 0.0, 0.0, 0.0 };

                alListener3f(AL_POSITION, 0, 0, 0);
                alSourcefv(_soundEngineSource, AL_POSITION, source0Pos);
                alSourcefv(_soundEngineSource, AL_VELOCITY, source0Vel);
                alSourcef(_soundEngineSource, AL_GAIN, 0.15f);

                alSourcePlay(_soundEnvironmentSource);
                alSourceStop(_soundEngineSource);

                alSourcei(_soundEngineSource, AL_BUFFER, _soundInternalEngineBuffer);
                alSourcePlay(_soundEngineSource);
            }
            else
            {
                alSourceStop(_soundEnvironmentSource);
                alSourceStop(_soundEngineSource);

                alSourcef(_soundEngineSource, AL_GAIN, 1.0f);
                alSourcef(_soundEngineSource, AL_PITCH, 1.0f);
                alSourcei(_soundEngineSource, AL_BUFFER, _soundExternalEngineBuffer);
                alSourcePlay(_soundEngineSource);
            }
        }

        //getFocusModel()->setShowState( _view != Camera::CockpitForwards );
    }

    if( e->key() == Qt::Key_F8)
    {
        if( e->modifiers().testFlag(Qt::ShiftModifier))
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

    if( e->key() == Qt::Key_F10)
    {
        global_cockpit_view = !global_cockpit_view;
    }

    //		  if( key == VK_SHIFT )
    //		  _camera.toggleShift();


    if( e->key() == Qt::Key_Escape)
    {
        close();

    }

    QOpenGLWindow::keyPressEvent(e);
}

void MainWindow::wheelEvent(QWheelEvent *e)
{
    _WorldSystem.onMouseWheel(e->x(), e->y(), e->modifiers(), e->angleDelta().y());
}

void MainWindow::mouseMoveEvent(QMouseEvent *)
{
}

void MainWindow::mousePressEvent(QMouseEvent *e)
{
    Q_UNUSED(e);
    _WorldSystem.rigidBodyToggleUsingMouse();
}

void MainWindow::onTimeOut()
{
    if( QGuiApplication::focusWindow() == 0 && GetAsyncKeyState(VK_ESCAPE) < 0)
        SetFocus(HWND(winId()));

    qint64 elapsed = _eTimer.elapsed();
    long diffMs = elapsed -_lastTime;

    if( diffMs ==0)
        return;

    _dt = diffMs / 1000.0;

    //std::cout << "->" << diffMs << std::endl;

    if( diffMs >= _fInverseFPS*1000 )
    {
        //std::cout << "in" << std::endl;
        //    std::cout << QTime::currentTime().toString().toStdString() << " - " << (_dt*1000) << " :" << (_fInverseFPS*1000) << std::endl;

        _lastTime = elapsed;
        QElapsedTimer t;
        t.start();
        onNextFrame( _dt );
        //std::cout << "nextFrame : " << t.elapsed() << std::endl;
    }
}

void MainWindow::onNextFrame(double dt)
{
    _framerate[_framecount++] = 1/frameTime();

    if( _framecount >= FPS_RESOLUTION )
        _framecount = 0;

    //	_cameraManager.onUpdate(dt);

    _nUVOffset += g_WaterFlow * dt;

    _WorldSystem.getCamera()._location = _camera._location;

    if (QGuiApplication::focusWindow() !=0 )
    {
        _camera.processAsyncKeys();
        _camera.processAsyncKeys2();
    }

    if (_view == Camera::Chase || _view == Camera::ChaseRotate )
    {
        if( GetAsyncKeyState( 'C' ) < 0 )
        {
            _chaseAngle -= 50*dt;
        }

        if( GetAsyncKeyState( 'V' ) < 0 )
        {
            _chaseAngle += 50*dt;
        }

        if( GetAsyncKeyState( 'W' ) < 0 )
        {
            _chaseHeight += 50*dt;
        }

        if( GetAsyncKeyState( 'S' ) < 0 )
        {
            _chaseHeight -= 50*dt;
        }

        if( GetAsyncKeyState( VK_OEM_PERIOD ) < 0 )
        {
            _chaseDistance -= 50*dt;
            if( _chaseDistance < 1 )
                _chaseDistance = 1;
        }

        if( GetAsyncKeyState( VK_OEM_COMMA ) < 0 )
        {
            _chaseDistance += 50*dt;
        }
    }

    if( isRunning() )
    {
        ensureCameraAboveGround();
        _WorldSystem.update(dt);
        update();
    }

    bool bPlaneActive = _WorldSystem.focusedRigidBody()
            && _WorldSystem.focusedRigidBody()->getID() == "A320_B";

    BAAirbus320JSONRigidBody* pAir = (BAAirbus320JSONRigidBody*)_WorldSystem.focusedRigidBody();

    if (bPlaneActive)
    {
        HeightData data;
        _WorldSystem.GetHeightFromPosition(pAir->getLastPosition(), data);

        double dHeightBefore = data.Height() * 3.2808 - 2;

        if( _view == Camera::CockpitForwards )
        {
            _WorldSystem.GetHeightFromPosition(pAir->getGPSLocation(), data);
            double dHeightAfter = data.Height() * 3.2808 - 2;

            for( auto& it : _soundHeightCallOutBuffers )
            {
                if( dHeightBefore > it.first && dHeightAfter < it.first )
                {
                    alSourceStop(_soundHeightCallOutSource);
                    alSourcei(_soundHeightCallOutSource, AL_BUFFER,it.second);
                    alSourcePlay( _soundHeightCallOutSource );
                    break;
                }
            }
        }
        else
        {
            goto mylabel;
        }
    }
    else
    {
mylabel:
        auto* focus = _WorldSystem.focusedRigidBody();
        if (focus)
        {
            VectorF offset = _camera._location.offSetTo(focus->getGPSLocation());
            VectorD velocity = focus->toNonLocalGroundFrame(focus->velocityBody());

            QuarternionD qCamera = MathSupport<double>::MakeQ(_camera._orientation);
            offset = QVRotate(~qCamera.toFloat(), offset);
            velocity = QVRotate(~qCamera, velocity);

            if (_view == Camera::FreeLookingAtPlane)
                alSource3f(_soundEngineSource, AL_POSITION, 0, 0, offset.Magnitude());
            else if (_view == Camera::Chase || _view == Camera::ChaseRotate)
                alSource3f(_soundEngineSource, AL_POSITION, 0, 0, fabs(_chaseDistance));
            else
                alSource3f(_soundEngineSource, AL_POSITION, offset.x, offset.y, offset.z);

            alSource3f(_soundEngineSource, AL_VELOCITY, velocity.x, velocity.y, velocity.z);


        }
    }

    if (bPlaneActive && _view == Camera::CockpitForwards)
    {
        ALfloat pitchValue;

        if (pAir->_left_engine._actual_thrust_percent < 30)
            pitchValue = pAir->_left_engine._actual_thrust_percent / 30.0;
        else
            pitchValue = (pAir->_left_engine._actual_thrust_percent - 30) / 70.0 * 1.0 + 1.0;

        alSourcef(_soundEngineSource, AL_PITCH, pitchValue);
    }
    else
    {
        if (pAir)
        {
            ALfloat pitchValue;

            if (pAir->_left_engine._actual_thrust_percent < 30)
                pitchValue = pAir->_left_engine._actual_thrust_percent / 30.0;
            else
                pitchValue = (pAir->_left_engine._actual_thrust_percent - 30) / 120.0 * 1.0 + 1.0;

            alSourcef(_soundEngineSource, AL_PITCH, pitchValue);
        }
    }

    {
        JSONRigidBody *pRigidBody = _WorldSystem.focusedRigidBody();
        if( pRigidBody )
        {
            const VectorD& euler = pRigidBody->getEuler();
            const VectorD& pos = pRigidBody->position();
            const VectorD& vel = pRigidBody->velocity();

            _pfdInstrument.m_fPitch = -euler.x;
            _pfdInstrument.m_fBank = euler.z;
            _pfdInstrument.m_fHdg = euler.y;

            HeightData data;
            _pfdInstrument.m_fAlt = pRigidBody->Height() * 3.2808 - 2;

            if (_pfdInstrument.m_fAlt < 762.0f && _WorldSystem.GetHeightFromPosition(pRigidBody->getGPSLocation(), data))
                _pfdInstrument.m_fAlt = data.Height() * 3.2808 - 2;

            VectorD vWind = _WorldSystem.getWeather()->getWindFromPosition(VectorD());
            _pfdInstrument.m_fAirSpd = AirProperties::Airspeed((vel-vWind).Magnitude() * 3600.0/1000.0/1.60934/1.15, pRigidBody->Height(), 0 );
            _pfdInstrument.m_fVSI = vel * pos.Unit() * 3.2808 * 60.0;
        }
    }
}

void MainWindow::RenderReflection()
{
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    _openGLFrameBuffer.bind();
    _openGLFrameBuffer.attachDepthRenderBuffer(_depthRenderBuffer);
    _openGLFrameBuffer.attachColorTexture2D(0, _reflectionTexture);

    if (_openGLFrameBuffer.checkFrameBufferStatusComplete() != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "checkFrameBufferStatusComplete failed : RenderReflection";
        return;
    }

    //GLenum drawbuffers[] = { GL_COLOR_ATTACHMENT0 };
    //glDrawBuffers(1, drawbuffers);

    glViewport(0, 0, _reflectionTexture.width(), _reflectionTexture.height());

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);

    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    OpenGLMatrixStack &view = OpenGLPipeline::Get(0).GetView();
    view.Push();
    view.LoadIdentity();
    OpenGLPipeline::Get(0).setViewZoom(_camera._zoom);
    setupCameraOrientation();
    setupCameraPosition(true);
    glUseProgram(_reflectionShaderProgram);
    _WorldSystem.RenderTerrain(true, false , _reflectionShaderProgram, 0, 0, _camera._location, frameTime());
    _WorldSystem.RenderModels(false, _reflectionShaderProgram, 0, 0, _camera._location, frameTime());
    glUseProgram(0);

    view.Pop();

    _openGLFrameBuffer.attachColorTexture2D(0, 0);
    _openGLFrameBuffer.attachDepthRenderBuffer(0);
    _openGLFrameBuffer.unbind();
    glEnable(GL_BLEND);
}

void MainWindow::RenderDepthTextures(int camID, OpenGLTexture2D& shadowMap, OpenGLTexture2D& colorMap, int offset, int size)
{
    OpenGLMatrixStack &view = OpenGLPipeline::Get(camID).GetView();

    VectorF cam(_camera._orientation.x, 0, _camera._orientation.z);
    VectorF light(_lightDir.x, 0, _lightDir.z);
    cam.Normalize();
    light.Normalize();
    float fDot = fabs(cam * light);

    float hSize = size / 2.0f;//  +_WorldSystem.getLightTanAngle() * 500 * fDot * camID;

    OpenGLPipeline::Get(camID).GetProjection().LoadIdentity();
    OpenGLPipeline::Get(camID).GetProjection().SetOrthographic(-hSize, hSize, -hSize, hSize, 1, 1000);

    //go to light's positon
    view.LoadIdentity();
    view.Translate(0, 0, 500);

    view.Top().RotateX(_lightDir.x + 90);
    view.Top().RotateY(-_lightDir.z + 180);

    view.Top().RotateModel(_camera._orientation);
    view.Translate(0, 0, -(offset + hSize));
    view.Top().RotateView(_camera._orientation);
    view.TransRotateGPS(_camera._location);

    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    _openGLFrameBuffer.bind();
    _openGLFrameBuffer.attachDepthTexture2D(shadowMap);
    _openGLFrameBuffer.attachColorTexture2D(0, colorMap);

    if (_openGLFrameBuffer.checkFrameBufferStatusComplete() != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "checkFrameBufferStatusComplete failed : RenderDepthTextures";
        return;
    }

    //GLenum drawbuffers[] = { GL_COLOR_ATTACHMENT0 };
    //glDrawBuffers(1, drawbuffers);

    glViewport(0, 0, shadowMap.width(), shadowMap.height());
    glEnable(GL_DEPTH_TEST);

    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

    glUseProgram(_simpleShaderProgram);
    _WorldSystem.RenderTerrain(true, false, _simpleShaderProgram, camID, 0, _camera._location, frameTime());
    _WorldSystem.RenderModels(false, _simpleShaderProgram, camID, 0, _camera._location, frameTime());
    _WorldSystem.RenderTerrain(false, false, _simpleShaderProgram, camID, 0, _camera._location, frameTime());

    glDisable(GL_CULL_FACE);

    _openGLFrameBuffer.attachDepthTexture2D(0);
    _openGLFrameBuffer.attachColorTexture2D(0, 0);

    _openGLFrameBuffer.unbind();

    glUseProgram(0);
    glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
}

void MainWindow::sendDataToShader(GLuint shader, int slot, int width, int height)
{
    glUniform2f(glGetUniformLocation(shader, "dims"), 1.0f / width, 1.0f / height);
    glUniform1i(glGetUniformLocation(shader, "shadowMap1"), slot);
    glUniform1i(glGetUniformLocation(shader, "shadowMap2"), slot + 1);
    glUniform1i(glGetUniformLocation(shader, "shadowMap3"), slot + 2);
    glUniform1i(glGetUniformLocation(shader, "reflectionMap"), slot + 3);
    GLint eyeSlot = glGetUniformLocation(shader, "eyeLightDirection");
    Vector4F eyeLightDirection = _WorldSystem.getEyeLightDirection();
    glUniform3f(eyeSlot, eyeLightDirection.x, eyeLightDirection.y, eyeLightDirection.z);
}

void MainWindow::RenderScene()
{
    QElapsedTimer t;

    t.start();

    QSize sz = size();

    RenderReflection();

    //std::cout << "RS (A) : " << t.restart() << std::endl;

    RenderDepthTextures(1, _shadowMap1, _shadowMapTexture1, 0, 150);
    RenderDepthTextures(2, _shadowMap2, _shadowMapTexture2, 145, 500);
    RenderDepthTextures(3, _shadowMap3, _shadowMapTexture3, 495, 2000);

    //std::cout << "RS (B) : " << t.restart() << std::endl;

    int slot = 2;
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, _shadowMap1);

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

    glActiveTexture(GL_TEXTURE0);

    glUseProgram(_shadowShaderProgram);
    sendDataToShader(_shadowShaderProgram, slot, sz.width(), sz.height());

    //std::cout << "RS (C) : " << t.restart() << std::endl;

    _WorldSystem.RenderTerrain( true, false, _shadowShaderProgram, 0, 3, _camera._location, frameTime());

    //std::cout << "RS (D) : " << t.restart() << std::endl;


    _WorldSystem.RenderModels( false, _shadowShaderProgram, 0, 3, _camera._location, frameTime());

    //std::cout << "RS (E) : " << t.restart() << std::endl;

    glDisable(GL_BLEND);
    glUseProgram(_waterShaderProgram);
    sendDataToShader(_waterShaderProgram, slot, sz.width(), sz.height());

    // To do water shader parameters.
    glUniform1f(glGetUniformLocation(_waterShaderProgram, "nUVOffset"), _nUVOffset);
    glUniform1i(glGetUniformLocation(_waterShaderProgram, "normalMap"), slot + 4);
    glUniform1i(glGetUniformLocation(_waterShaderProgram, "dudvMap"), slot + 5);

    _WorldSystem.RenderTerrain( false, false, _waterShaderProgram, 0, 3, _camera._location, frameTime());

    //std::cout << "RS (F) : " << t.restart() << std::endl;


    glEnable(GL_BLEND);
    for (int i = slot; i <= slot + 5; ++i)
    {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    glActiveTexture(GL_TEXTURE0);
    glUseProgram(0);
    _WorldSystem.RenderModelForceGenerators(frameTime());
    //std::cout << "RS (G) : " << t.restart() << std::endl;

}

void MainWindow::RenderSlopes()
{
    float fLightingFraction = _WorldSystem.getLightFraction();
    glColor3f(fLightingFraction, fLightingFraction, fLightingFraction);
    RenderLeftSlope();
    RenderRightSlope();
}
void MainWindow::RenderDrivingPower(float cx, float cy)
{
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);

    glPushMatrix();
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, cx, cy, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);

    glLoadIdentity();
    glTranslatef(0, 0, -1);

    /////////////////////////////////////

    glBegin(GL_LINE_LOOP);
    glColor3f(1, 1, 1);
    glVertex2d(cx-50, 50);
    glVertex2d(cx-60, 50);
    glVertex2d(cx-60, cy - 50);
    glVertex2d(cx-60, cy - 50);
    glEnd();

    glBegin(GL_LINES);
    for (int i = 0; i <= 100; i += 10)
    {
        float y = 50 + i / 100.0f*(cy - 100);
        glVertex2f(cx - 60, y);
        glVertex2f(cx - 50, y);
    }
    glEnd();

    glBegin(GL_QUADS);

    float power = 0.0f;
    float powerActual = 0.0f;
    auto *focus = _WorldSystem.focusedRigidBody();
    if (focus)
    {
        focus->getPower(0);
        powerActual = focus->getPowerOutput(0);
    }
    int powerY = cy-(std::max(power,powerActual) * (cy - 100)/100.0f + 50);

    glVertex2d(cx - 50, powerY - 2);
    glVertex2d(cx - 60, powerY - 2);
    glVertex2d(cx - 60, powerY + 2);
    glVertex2d(cx - 50, powerY + 2);
    glEnd();



    /////////////////////////////////////

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

void MainWindow::RenderMouseFlying(float cx, float cy)
{
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);

    glPushMatrix();
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, cx, cy, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);

    glLoadIdentity();
    glTranslatef(0, 0, -1);
    float divide = 10.0f;

    glBegin(GL_LINE_LOOP);
    glColor3f(1, 1, 1);
    glVertex2f(cx / 2 - cx / divide, cy / 2 - cy / divide);
    glVertex2f(cx / 2 + cx / divide, cy / 2 - cy / divide);
    glVertex2f(cx / 2 + cx / divide, cy / 2 + cy / divide);
    glVertex2f(cx / 2 - cx / divide, cy / 2 + cy / divide);

    glEnd();

    glBegin(GL_LINES);
    glVertex2f(cx / 2 - 10, cy / 2);
    glVertex2f(cx / 2 + 10, cy / 2);
    glVertex2f(cx / 2, cy / 2 - 10);
    glVertex2f(cx / 2, cy / 2 + 10);
    glEnd();

    QPoint pt = QCursor::pos();
    pt = mapFromGlobal(pt);

    glBegin(GL_LINE_LOOP);
    glVertex2f(pt.x() - 5, pt.y() - 5);
    glVertex2f(pt.x() + 5, pt.y() - 5);
    glVertex2f(pt.x() + 5, pt.y() + 5);
    glVertex2f(pt.x() - 5, pt.y() + 5);
    glEnd();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

void MainWindow::RenderTexture(GLuint texID, int pos)
{
    int size = 30*2;
    OpenGLPipeline::Get(0).Push();
    OpenGLPipeline::Get(0).GetView().LoadIdentity();
    OpenGLPipeline::Get(0).GetModel().LoadIdentity();
    OpenGLPipeline::Get(0).GetModel().Translate(-100 + (size+2)*pos, -100, 0);
    OpenGLPipeline::Get(0).GetProjection().LoadIdentity();
    OpenGLPipeline::Get(0).GetProjection().SetOrthographic(-100, 100, -100, 100, -1, 1);
    OpenGLPipeline::Get(0).Apply();

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);

    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texID);

    glBegin(GL_QUADS);
    glNormal3f(0, 0, 1);
    //glColor4f(0.5f, 0.5f, 0.5f, 0.35f);
    //glColor4f(1.0f, 1.0f, 1.0f, 0.35f);
    glColor4f(1.0f, 1.0f, 1.0f, 0.99f);

    glTexCoord2f(0, 0);
    glVertex2f(0, 0);

    glTexCoord2f(1, 0);
    glVertex2f(size, 0);

    glTexCoord2f(1, 1);
    glVertex2f(size, size);

    glTexCoord2f(0, 1);
    glVertex2f(0, size);

    glEnd();

    glBegin(GL_LINE_LOOP);
    glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
    glVertex2f(0, 0);
    glVertex2f(size, 0);
    glVertex2f(size, size);
    glVertex2f(0, size);
    glEnd();


    OpenGLPipeline::Get(0).Pop();
    glBindTexture(GL_TEXTURE_2D, 0);

    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
}

void MainWindow::RenderFPS()
{
    OGLFontContainer _renderFont( _oglFont );
    float fps = 0.0f;
    for( int i=0; i < FPS_RESOLUTION; i++ )
        fps += _framerate[i];

    char text[256]="";
    sprintf_s( text, _countof(text), "%.0f", fps / FPS_RESOLUTION);
    _oglFont.RenderFont( 15, 5, text );
}

void MainWindow::RenderInfo()
{
    glColor4f(1.0f,1.0f,1.0f, 0.6);
    OGLFontContainer renderFont( _oglFont );
    Q_UNUSED(renderFont);

    float fps = 0.0f;

    if( isRunning() )
        for( int i=0; i < FPS_RESOLUTION; i++ )
            fps += _framerate[i];

    _oglFont.RenderFont( 15, 15, "3D Virtual World by Mo" );
    _oglFont.RenderFont( 15, 30, "------------------------------------------" );

    static bool bInitDone = false;
    static char *Version = (char*)glGetString(GL_VERSION);
    static char *Extensions = (char*)glGetString(GL_EXTENSIONS);
    //static char *WglExtensionsExt = (char*)wglGetExtensionsStringEXT();
    static char *Renderer = (char*)glGetString(GL_RENDERER);
    static char *Vendor = (char*)glGetString(GL_VENDOR);
    static char Extensions2[51200];

    if( !bInitDone )
    {
        strcpy_s( Extensions2, _countof(Extensions2), Extensions);
        //strcat_s(Extensions2, _countof(Extensions2), WglExtensionsExt );

        for( size_t i=0; i < strlen(Extensions2); i++ )
        {
            if( Extensions2[i] == ' ' )
                Extensions2[i] = '\n';
        }
        bInitDone = true;
        std::string extensionfile("extensions.txt");

        if( GetFileAttributesA( extensionfile.c_str() ) == -1 )
        {
            FILE *fExtensions = NULL;
            fopen_s( &fExtensions, extensionfile.c_str(), "wt" );

            if( fExtensions )
            {
                fprintf_s( fExtensions, "***Extensions2 :%s:*****", Extensions2 );
                fclose(fExtensions );
                ::ShellExecuteA( NULL, "open", extensionfile.c_str(), NULL, NULL, SW_NORMAL );
            }
        }
    }
    char text[1024]= {};
    JSONRigidBody *pRigidBody = _WorldSystem.focusedRigidBody();

    sprintf_s( text, _countof(text), "OpenGL [%s], Vendor [%s], Renderer[%s]", Version, Vendor, Renderer );
    _oglFont.RenderFont( 15, 45, text );

    std::string strFocus;
    if (pRigidBody)
        strFocus = pRigidBody->getID();
    else
        strFocus = "##NONE##";

    sprintf_s( text, _countof(text), "Object [%s], Running [%d]", strFocus.c_str(), isRunning() );
    _oglFont.RenderFont( 15, 60, text );

    GPSLocation cameraGPS(_camera._location);

    sprintf_s( text, _countof(text), "FPS :%d: Q: [%.2f,%.2f,%.2f]",
               int(fps / FPS_RESOLUTION),
               _camera._orientation.x,
               _camera._orientation.y,
               _camera._orientation.z);
    _oglFont.RenderFont( 15, 75, text );

    sprintf_s( text, _countof(text), "CameraPosition: [%.8f N, %.8f E] (%.2f)", cameraGPS._lat, cameraGPS._lng, cameraGPS._height );
    _oglFont.RenderFont( 15, 90, text );

    if( pRigidBody )
    {
        VectorD pos = pRigidBody->position();
        VectorD vel = pRigidBody->velocity();

        VectorD e = pRigidBody->getEuler();
        VectorD vb = pRigidBody->velocityBody();

        GPSLocation gpsLocation(pRigidBody->position());

        sprintf_s( text, _countof(text), "ModelPosition: [%.8f N, %.8f E] (%.2f)", gpsLocation._lat, gpsLocation._lng, gpsLocation._height );

        _oglFont.RenderFont( 15, 105, text );

        double velMag = vel.Magnitude();

        sprintf_s( text, _countof(text), "Vel: [%.2f,%.2f,%.2f] (Kts:%.2f)(Mph:%.2f)", vel.x, vel.y, vel.z, MS_TO_KTS(velMag), MS_TO_MPH(velMag) );
        _oglFont.RenderFont( 15, 120, text );

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

        const VectorD& angVel = pRigidBody->angularVelocity();
        const VectorD& cg = pRigidBody->cg();

        sprintf_s( text, _countof(text), "Ang V[%.2f,%.2f,%.2f] CG[%.2f,%.2f,%.2f]", angVel.x, angVel.y, angVel.z, cg.x, cg.y, cg.z );
        _oglFont.RenderFont( 15, 135, text );


        double dHeightOfGround(0.0);
        QuarternionD qPlane;

        HeightData data;
        if( _WorldSystem.GetHeightFromPosition(cameraGPS, data ) )
        {
            VectorF euler = MathSupport<float>::MakeEuler( data.qNormal() ) ;
            sprintf_s( text, _countof(text), "HeightAbovePlane :[%.8f], Euler : [%.2f,%.2f,%.2f]", data.Height(),euler.x, euler.y, euler.z );
        }
        else
            sprintf_s( text, _countof(text), "HeightAbovePlane :[NOT AVAILABLE]");

        _oglFont.RenderFont( 15, 150, text );

        sprintf_s(text, _countof(text), "View :%d: Zoom :%.2f: Cockpit :%s:", _view, _camera._zoom, global_cockpit_view ? "Y":"N");
        _oglFont.RenderFont( 15, 165, text );

        VectorD acceleration = pRigidBody->getForce() / pRigidBody->getMass();
        double gForce = 1+acceleration.y / pRigidBody->gravity().y;

        JSONRigidBody::STATE state = JSONRigidBody::STATE::NORMAL;
        state = pRigidBody->getState();

        sprintf_s(text, _countof(text), "G :[%.2f], FlightRec [%s]", gForce,
                  state ==JSONRigidBody::STATE::NORMAL ? "Normal": (state ==JSONRigidBody::STATE::RECORDING ? "Recording": "Playback" ) );
        _oglFont.RenderFont( 15, 180, text );

        sprintf_s( text, _countof(text), "Camera-Object distance :%.2f: miles", _WorldSystem.focusedRigidBody()->getGPSLocation().distanceTo( GPSLocation(_camera._location) )*3.2808/5280 );
        _oglFont.RenderFont( 15, 195, text );

        if (state == JSONRigidBody::STATE::PLAYBACK)
        {
            sprintf_s(text, _countof(text), "Playback-> %3.2f/%3.0fsecs",
                      pRigidBody->getFlightRecorder().timeSoFar(),
                      pRigidBody->getFlightRecorder().totalTime()
                      );
            _oglFont.RenderFont(15, 210, text);
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
}

void MainWindow::setupCameraOrientation()
{
    JSONRigidBody *pBody = _WorldSystem.focusedRigidBody();

    if (pBody == 0)
        return;

    OpenGLMatrixStack & mat = OpenGLPipeline::Get(0).GetView();

    if (_view == Camera::Free)
    {
        //No rotation needed
        _camera._orientation.z = 0;
    }
    else if( _view == Camera::FreeLookingAtPlane )
    {
        VectorD lookAt = _camera._location.offSetTo(pBody->getGPSLocation() + pBody->cg()).toDouble();
        _camera._orientation = MathSupport<double>::MakeEulerFromLookAt(lookAt);
    }
    else if (_view == Camera::Chase )
    {
        _camera._orientation.y = - _chaseAngle;
    }
    else if (_view == Camera::ChaseRotate)
    {
        _camera._orientation = pBody->getEuler();
        _camera._orientation.y += -_chaseAngle;
    }
    else if (_view == Camera::CockpitForwards
             || _view == Camera::PassengerLeftMiddle
             || _view == Camera::RightGearWheel)
    {
        _camera._orientation = pBody->getEuler();
    }

    mat.Rotate(_camera._orientation.x, _camera._orientation.y, _camera._orientation.z);
}

void MainWindow::setupCameraPosition(bool bReflection)
{
    VectorD position;
    VectorD cg;
    VectorD euler;

    JSONRigidBody *pBody = _WorldSystem.focusedRigidBody();

    if( pBody )
    {
        position = pBody->position();
        cg = pBody->cg();
        euler = pBody->getEuler();
    }

    OpenGLMatrixStack & mv = OpenGLPipeline::Get(0).GetView();

    if( _view == Camera::Free || _view == Camera::FreeLookingAtPlane)
    {

    }
    else if( _view == Camera::Chase || _view == Camera::ChaseRotate)
    {
        if (pBody)
            _camera._location = pBody->toNonLocalTranslateFrame(VectorD(0, _chaseHeight, _chaseDistance));
    }
    else if (_view == Camera::ChaseRotate)
    {
        if (pBody)
            _camera._location = pBody->toNonLocalTranslateFrame(VectorD(_chaseDistance, _chaseHeight, 0));
    }
    else if( _view == Camera::CockpitForwards )
    {
        if( pBody )
        {
            VectorD pos;

            if (pBody->getName() == "BAA380")
                pos = VectorD(0.0, 7.6, -32.72);
            else if(pBody->getName() == "Harrier")
                pos = VectorD(0, 2.7, -3.5);
            else
                pos = VectorD(-0.45, 4.80, -14.5);

            pos.z -= 1.5;

            _camera._location = pBody->toNonLocalTranslateFrame(pos);

            /*else if( pBody == &_audiA8 )
                mv.Top().Translate( 0.4, -1.20, 0.05);
            else if( pBody == &_trainA )
                mv.Top().Translate( 0.4, -1.05, 0.05);
            else
                mv.Top().Translate( 0, -5.0,  0 );*/
        }
    }
    else if( _view == Camera::PassengerLeftMiddle || _view == Camera::RightGearWheel)
    {

        if (pBody)
        {
            /*if( _towTruck.hasFocus() )
            {
                if( _view == Camera::PassengerLeftMiddle )
                    mv.Top().Translate( -0.66, -1.25, 2.7 );

                if( _view == Camera::RightGearWheel )
                    mv.Top().Translate( 0.66, -1.25, 2.7 );
            }
            else*/
            {
                VectorD pos;
                if (_view == Camera::PassengerLeftMiddle)
                    pos = VectorD(1.7, 4.0, 3.35);

                if (_view == Camera::RightGearWheel)
                    pos = VectorD(0, 0, 10);

                _camera._location = pBody->toNonLocalTranslateFrame(pos);
            }
        }
    }

    if (bReflection)
    {
        GPSLocation pos = _camera._location;

        mv.Translate(0, pos._height, 0);
        mv.Scale(1, -1, 1);
        mv.Translate(0, -pos._height, 0);
        mv.TransRotateGPS(pos);
    }
    else
        mv.TransRotateGPS(_camera._location);
}

void MainWindow::RenderLeftSlope()
{
    static double dGrad = -flatGradientFromGPSDiff(leftSlopeStart,leftSlopeEnd,LEFT_SLOPE_GRAD);
    static double dDist = leftSlopeEnd.distanceTo(leftSlopeStart);

    float fLightingFraction = _WorldSystem.getLightFraction();
    float fHeightOffGround = 40.0f;

    OpenGLPipeline::Get(0).Push();
    OpenGLPipeline::Get(0).GetModel().TransRotateGPS(leftSlopeStart);
    OpenGLPipeline::Get(0).Apply();

    glEnable(GL_TEXTURE_2D);
    _slopeGrass.bind();
    glBegin(GL_QUADS);
    glColor3f(fLightingFraction,fLightingFraction,fLightingFraction);
    glNormal3f(0,1,0);

    float fAdjust = 0.25f;

    glTexCoord2f( 0, 0 );
    glVertex3f(0, 0 * dGrad - fAdjust + fHeightOffGround, 140);

    glTexCoord2f( 0.1, 0 );
    glVertex3f(0, 0 * dGrad - fAdjust + fHeightOffGround, -160);

    glTexCoord2f( 0.1, 1 );
    glVertex3f(-dDist, dDist * dGrad - fAdjust + fHeightOffGround, -160);

    glTexCoord2f( 0, 1 );
    glVertex3f(-dDist, dDist * dGrad - fAdjust + fHeightOffGround, 140);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(fLightingFraction*0.1f, fLightingFraction*0.6f, fLightingFraction*0.1f);
    glNormal3f(0, 0, 1);

    glTexCoord2f(1, 0);
    glVertex3f(0, 0, -160);

    glTexCoord2f(1, 1);
    glVertex3f(0, fHeightOffGround, -160);

    glTexCoord2f(0,1);
    glVertex3f(0, fHeightOffGround, 140);

    glTexCoord2f(0, 0);
    glVertex3f(0, 0, 140);
    glEnd();

    _runway1.bind();
    glBegin(GL_QUADS);
    //glColor4f(0.0,0,0.6,0.4);
    glNormal3f(0,1,0);
    glColor3f(fLightingFraction,fLightingFraction,fLightingFraction);
    glTexCoord2f( 0, 0 );
    glVertex3f(0, 0 * dGrad + fHeightOffGround, 25);

    glTexCoord2f( 1, 0 );
    glVertex3f(0, 0 * dGrad + fHeightOffGround, -25);

    glTexCoord2f( 1, 1 );
    glVertex3f(-60, 60 * dGrad + fHeightOffGround, -25);

    glTexCoord2f( 0, 1 );
    glVertex3f(-60, 60 * dGrad + fHeightOffGround, 25);
    glEnd();

    _runway2.bind();
    glBegin(GL_QUADS);

    glTexCoord2f( 0, 0 );
    glVertex3f(-60, 60 * dGrad + fHeightOffGround, 25);

    glTexCoord2f( 1, 0 );
    glVertex3f(-60, 60 * dGrad + fHeightOffGround, -25);

    glTexCoord2f( 1, dDist/50 );
    glVertex3f(-dDist, dDist * dGrad + fHeightOffGround, -25);

    glTexCoord2f( 0, dDist/50 );
    glVertex3f(-dDist, dDist * dGrad + fHeightOffGround, 25);
    glEnd();
    glDisable(GL_TEXTURE_2D);

    OpenGLPipeline::Get(0).Pop();

}

void MainWindow::RenderRightSlope()
{
    static double dGrad = flatGradientFromGPSDiff(rightSlopeStart,rightSlopeEnd,RIGHT_SLOPE_GRAD);
    static double dDist = rightSlopeEnd.distanceTo(rightSlopeStart);

    OpenGLPipeline::Get(0).Push();
    OpenGLPipeline::Get(0).GetModel().TransRotateGPS(rightSlopeStart);
    OpenGLPipeline::Get(0).Apply();

    glEnable(GL_BLEND);
    glDepthMask(GL_FALSE);
    glBegin(GL_QUADS);
    glColor4f(0.0,0,0.6,0.4);
    glNormal3f(0,1,0);

    glVertex3f( 2, 2 * dGrad, 140 );
    glVertex3f( 2, 2 * dGrad, -160 );
    glVertex3f( dDist, dDist * dGrad, -160 );
    glVertex3f( dDist, dDist * dGrad, 140 );
    glEnd();
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
    OpenGLPipeline::Get(0).Pop();
}

void MainWindow::OnInitPolyMode()
{
    if( _bUserPolygonLineView )
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE );
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL );
}

void MainWindow::OnRenderSkyBox()
{
    float fLightingFraction = _bUserPolygonLineView ? 0.0f : _WorldSystem.getLightFraction();

    GLubyte sky_bright[] =
    {
        0.75f * _skyColor.x * fLightingFraction * 255,
        0.75f * _skyColor.y * fLightingFraction * 255,
        0.75f * _skyColor.z * fLightingFraction * 255
    };

    GLubyte sky_dim[] =
    {
        0.4f * _skyColor.x * fLightingFraction * 255,
        0.4f * _skyColor.y * fLightingFraction * 255,
        0.4f * _skyColor.z * fLightingFraction * 255
    };

    GLubyte ground_bright[] = { 0, 170*fLightingFraction, 0 };
    GLubyte ground_dim[] = { 0, 130*fLightingFraction, 0 };

    glDisable(GL_LIGHTING);
    glDepthMask(GL_FALSE);

    OpenGLPipeline::Get(0).Apply();

    if( !_bUserPolygonLineView )
    {
        ground_bright[0] = sky_bright[0];
        ground_bright[1] = sky_bright[1];
        ground_bright[2] = sky_bright[2];
    }

    float fDist(10.0f);

    glBegin(GL_QUADS);
    // SKY
    // FRONT
    glColor3ubv( sky_bright );
    glNormal3f( 0, 0, 1 );
    glVertex3f( -fDist, 0, -fDist);
    glVertex3f( fDist, 0, -fDist);

    glColor3ubv( sky_dim );
    glVertex3f( fDist, fDist*0.75f, -fDist);
    glVertex3f(-fDist, fDist*0.75f, -fDist);

    //// LEFT
    glColor3ubv( sky_bright );
    glNormal3f( 1, 0, 0 );
    glVertex3f( -fDist, 0, fDist);
    glVertex3f( -fDist, 0, -fDist);

    glColor3ubv( sky_dim );
    glVertex3f(-fDist, fDist*0.75f, -fDist);
    glVertex3f(-fDist, fDist*0.75f, fDist);

    //// RIGHT
    glColor3ubv( sky_bright );
    glNormal3f( -1, 0, 0 );
    glVertex3f( fDist, 0, -fDist);
    glVertex3f( fDist, 0, fDist);

    glColor3ubv( sky_dim );
    glVertex3f(fDist, fDist*0.75f, fDist);
    glVertex3f(fDist, fDist*0.75f, -fDist);

    //// BACK
    glColor3ubv( sky_bright );
    glNormal3f( 0, 0, -1 );
    glVertex3f( fDist, 0, fDist);
    glVertex3f( -fDist, 0, fDist);

    glColor3ubv( sky_dim );
    glVertex3f(-fDist, fDist*0.75f, fDist);
    glVertex3f(fDist, fDist*0.75f, fDist);

    //// TOP
    glColor3ubv( sky_dim );
    glNormal3f( 0, -1, 0 );
    glVertex3f(fDist, fDist*0.75f, fDist);
    glVertex3f(-fDist, fDist*0.75f, fDist);

    glVertex3f(-fDist, fDist*0.75f, -fDist);
    glVertex3f(fDist, fDist*0.75f, -fDist);

    // GROUND
    // FRONT
    glColor3ubv( ground_bright );
    glNormal3f( 0, 0, 1 );
    glVertex3f( fDist, 0, -fDist);
    glVertex3f( -fDist, 0, -fDist);

    glColor3ubv( ground_dim );
    glVertex3f(-fDist, -fDist*0.75f, -fDist);
    glVertex3f(fDist, -fDist*0.75f, -fDist);

    // LEFT
    glColor3ubv( ground_bright );
    glNormal3f( 1, 0, 0 );
    glVertex3f( -fDist, 0, -fDist);
    glVertex3f( -fDist, 0, fDist);

    glColor3ubv( ground_dim );
    glVertex3f(-fDist, -fDist*0.75f, fDist);
    glVertex3f(-fDist, -fDist*0.75f, -fDist);

    // RIGHT
    glColor3ubv( ground_bright );
    glNormal3f( -1, 0, 0 );
    glVertex3f( fDist, 0, fDist);
    glVertex3f( fDist, 0, -fDist);

    glColor3ubv( ground_dim );
    glVertex3f(fDist, -fDist*0.75f, -fDist);
    glVertex3f(fDist, -fDist*0.75f, fDist);

    // BACK
    glColor3ubv( ground_bright );
    glNormal3f( 0, 0, -1 );
    glVertex3f( -fDist, 0, fDist);
    glVertex3f( fDist, 0, fDist);

    glColor3ubv( ground_dim );
    glVertex3f(fDist, -fDist*0.75f, fDist);
    glVertex3f(-fDist, -fDist*0.75f, fDist);

    // BOTTOM
    glColor3ubv( ground_dim );
    glNormal3f( 0, 1, 0 );
    glVertex3f(-fDist, -fDist*0.75f, -fDist);
    glVertex3f(-fDist, -fDist*0.75f, fDist);

    glVertex3f(fDist, -fDist*0.75f, fDist);
    glVertex3f(fDist, -fDist*0.75f, -fDist);

    glEnd();

    glDepthMask(GL_TRUE);
    glEnable(GL_LIGHTING);
}

