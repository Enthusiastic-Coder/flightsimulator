#include "stdafx.h"
#include "SDLGameLoop.h"
#include <include_gl.h>
#include <string>
#include <vector>
#include "SDL_syswm.h"
#include <algorithm>

void SDLGameLoop::onTimeOut()
{
    //onHandleAsyncKeys(SDL_GetKeyboardState(0), SDL_GetModState(), dt);
    onUpdate();

    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    onRender();
    SDL_GL_SwapWindow(_sdlWindow);
}

SDL_Window *SDLGameLoop::getWindow()
{
    return _sdlWindow;
}

SDLGameLoop::SDLGameLoop() :
    _dt(0.0f),
    _fInverseFPS(1/30.0f),
    _sdlWindow(0)
{
    _updateEventType = SDL_RegisterEvents(1);
}

std::vector<std::string> split(const std::string& s, char seperator)
{
    std::vector<std::string> output;

    std::string::size_type prev_pos = 0, pos = 0;

    while ((pos = s.find(seperator, pos)) != std::string::npos)
    {
        std::string substring(s.substr(prev_pos, pos - prev_pos));

        output.push_back(substring);

        prev_pos = ++pos;
    }

    output.push_back(s.substr(prev_pos, pos - prev_pos)); // Last word

    return output;
}

void writeOutResult()
{
    char* chVen = (char*)glGetString(GL_VENDOR);
    std::string vendor = chVen != 0 ? chVen : "";

    char* chRen = (char*)glGetString(GL_RENDERER);
    std::string renderer = chRen != 0 ? chRen : "";

    char* chVer = (char*)glGetString(GL_VERSION);
    std::string version = chVer != 0 ? chVer : "";

    SDL_Log("Vendor : %s", vendor.c_str());
    SDL_Log("Renderer : %s", renderer.c_str() );
    SDL_Log("Version : %s", version.c_str());

    int value = 0;
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &value);
    SDL_Log("SDL_GL_CONTEXT_MAJOR_VERSION : %d", value );

    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &value);
    SDL_Log("SDL_GL_CONTEXT_MINOR_VERSION : %d", value);

    SDL_GL_GetAttribute(SDL_GL_RED_SIZE, &value);
    SDL_Log("SDL_GL_RED_SIZE : %d", value);

    SDL_GL_GetAttribute(SDL_GL_GREEN_SIZE, &value);
    SDL_Log("SDL_GL_GREEN_SIZE : %d", value);

    SDL_GL_GetAttribute(SDL_GL_BLUE_SIZE, &value);
    SDL_Log("SDL_GL_BLUE_SIZE : %d", value);

    SDL_GL_GetAttribute(SDL_GL_ALPHA_SIZE, &value);
    SDL_Log("SDL_GL_ALPHA_SIZE : %d", value);

    SDL_GL_GetAttribute(SDL_GL_BUFFER_SIZE, &value);
    SDL_Log("SDL_GL_BUFFER_SIZE : %d", value);

    SDL_GL_GetAttribute(SDL_GL_DOUBLEBUFFER, &value);
    SDL_Log("SDL_GL_DOUBLEBUFFER : %d", value);

    SDL_GL_GetAttribute(SDL_GL_DEPTH_SIZE, &value);
    SDL_Log("SDL_GL_DEPTH_SIZE : %d", value);

    SDL_GL_GetAttribute(SDL_GL_STENCIL_SIZE, &value);
    SDL_Log("SDL_GL_STENCIL_SIZE : %d", value);

    SDL_GL_GetAttribute(SDL_GL_CONTEXT_EGL, &value);
    SDL_Log("SDL_GL_CONTEXT_EGL : %d", value);

    SDL_GL_GetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, &value);
    SDL_Log("SDL_GL_CONTEXT_PROFILE_MASK : %d", value);

    SDL_Log( "Current Video Driver : %s",  SDL_GetCurrentVideoDriver());

    SDL_Log( "SDL_GetNumVideoDisplays : %d", SDL_GetNumVideoDisplays());
    for (int i = 0; i < SDL_GetNumVideoDisplays(); ++i)
        SDL_Log( "SDL_GetDisplayName [] : %s", SDL_GetDisplayName(i) );

    SDL_Log("GL_ARB_ES2_compatibility : %d", SDL_GL_ExtensionSupported("GL_ARB_ES2_compatibility"));

    SDL_Log("-------------");

    char* chExts = (char*)glGetString(GL_EXTENSIONS);
    std::string exts = chExts != 0 ? chExts : "";

    std::vector<std::string> list = split(exts, ' ');
    for(std::string var : list)
    {
        SDL_Log("%s", var.c_str());;
    }

    SDL_Log("-------------");

    SDL_Log("There are %d joysticks available\n", SDL_NumJoysticks());

    for (int i = 0; i < SDL_NumJoysticks(); ++i)
        SDL_Log( "[%d -> %s", i, SDL_JoystickNameForIndex(i));
}

bool SDLGameLoop::run(std::string strTitle)
{
    SDL_Rect rect;
    SDL_GetDisplayBounds(0, &rect);

#ifdef _DEBUG
    _sdlWindow = SDL_CreateWindow(strTitle.c_str(),
                                  SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                  rect.w*0.5, rect.h*0.5,
                                  SDL_WINDOW_OPENGL|SDL_WINDOW_SHOWN|SDL_WINDOW_RESIZABLE);
#else
    _sdlWindow = SDL_CreateWindow(strTitle.c_str(),
                                  SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                  0, 0,
                                  SDL_WINDOW_OPENGL|SDL_WINDOW_SHOWN|SDL_WINDOW_FULLSCREEN_DESKTOP);
#endif

    if(_sdlWindow==0)
    {
        SDL_Log("CreateWindow : %s", SDL_GetError());
        return false;
    }

    _glContext = SDL_GL_CreateContext(_sdlWindow);
    _running = true;

    int w, h;
    SDL_GetWindowSize(_sdlWindow, &w,&h);
    onSize(w,h);
    onInitialise();

    SDL_Log( "Current Video Driver : %s",  SDL_GetCurrentVideoDriver());

    writeOutResult();

    SDL_bool bScreenSaverEnabled = SDL_IsScreenSaverEnabled();

    if( bScreenSaverEnabled == SDL_TRUE)
        SDL_DisableScreenSaver();

    _timer.Start();
    onTimeOut();

    bool bQuit = false;
    SDL_Event event;

    while( !bQuit )
    {
        while(!_running && SDL_WaitEvent( &event )
              ||
              _running && SDL_PollEvent(&event) )
        {
            if( event.type == SDL_QUIT)
            {
                bQuit = true;
                break;
            }

            if( event.type == _updateEventType)
                break;

            onDispatchMessage(event);

            if( !isRunning())
                update();
        }

        _dt = _timer.Elapsed();

        if (_dt >= _fInverseFPS)
        {
            _timer.Restart();

            if( !isRunning())
                _dt = 0.0f;

            onTimeOut();
        }
    }

    onUnInitialise();

    if(bScreenSaverEnabled == SDL_TRUE)
        SDL_EnableScreenSaver();
    else
        SDL_DisableScreenSaver();

    SDL_GL_DeleteContext(_glContext);
    SDL_DestroyWindow(_sdlWindow);

    _sdlWindow = 0;

    return true;
}


bool SDLGameLoop::onDispatchMessage(SDL_Event& event)
{
    bool bHandled = false;

    switch( event.type)
    {
    case SDL_WINDOWEVENT:
        switch (event.window.event)
        {
        case SDL_WINDOWEVENT_SIZE_CHANGED:
            SDL_Log("Size_Changed to [%d,%d]",
                    event.window.data1, event.window.data2);
            onSize(event.window.data1, event.window.data2);
            break;
        case SDL_WINDOWEVENT_MOVED:
            SDL_Log("Moved to [%d,%d]",
                    event.window.data1, event.window.data2);
            break;
        case SDL_WINDOWEVENT_CLOSE:
            SDL_Log("Window Event Close");
            break;
        default:
            break;
        }

    case SDL_KEYDOWN:
        onKeyDown(&event.key);

        //event.key.keysym.mod
        switch(event.key.keysym.sym)
        {
        case SDLK_ESCAPE:
            postQuit();
            break;
        default:
            switch(event.key.keysym.scancode)
            {
            case SDL_SCANCODE_ESCAPE:
                postQuit();
                break;
            default:
                break;
            }
        }
        break;

     case SDL_KEYUP :
        onKeyUp(&event.key);
        break;
     case SDL_MOUSEMOTION :
        onMouseMotion(&event.motion);
        break;
     case SDL_MOUSEBUTTONDOWN :
        onMouseDown(&event.button);
        break;
     case SDL_MOUSEBUTTONUP :
        onMouseUp(&event.button);
        break;
     case SDL_MOUSEWHEEL :
        onMouseWheel(&event.wheel);
        break;
     case SDL_FINGERDOWN :
        onFingerDown(&event.tfinger);
        break;
     case SDL_FINGERUP:
        onFingerUp(&event.tfinger);
        break;
     case SDL_FINGERMOTION:
        onFingerMotion(&event.tfinger);
        break;
    default:
        break;
    }

    return bHandled;
}

void SDLGameLoop::setFramePerSecond( float fps)
{
    _fInverseFPS = 1.0f / std::max(1.0f,fps);
}

void SDLGameLoop::setRunning( bool bRunning )
{
    _running = bRunning;
    _timer.Start();
}

bool SDLGameLoop::isRunning()
{
    return _running;
}

float SDLGameLoop::frameTime()
{
    return _dt;
}

void SDLGameLoop::update()
{
    SDL_Event event;
    SDL_memset(&event, 0, sizeof(event)); /* or SDL_zero(event) */
    event.type = _updateEventType;
    event.user.code = 0;
    event.user.data1 = 0;
    event.user.data2 = 0;
    SDL_PushEvent(&event);
}

void SDLGameLoop::postQuit()
{
    SDL_Event e;
    e.type = SDL_QUIT;
    SDL_PushEvent(&e);
}

void SDLGameLoop::GetMousePos(int &x, int &y) const
{
    SDL_GetMouseState(&x, &y);
}

void SDLGameLoop::GetScreenDims(int &w, int &h) const
{
    SDL_GetWindowSize(_sdlWindow, &w, &h);
}

void SDLGameLoop::GetScreenPos(int &x, int &y) const
{
    SDL_GetWindowPosition(_sdlWindow, &x, &y);
}


