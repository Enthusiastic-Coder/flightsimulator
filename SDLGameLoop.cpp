#include "stdafx.h"
#include "SDLGameLoop.h"
#include <include_gl.h>
#include <string>
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

    SDL_SysWMinfo info;
    SDL_VERSION(&info.version); // initialize info structure with SDL version info

    _glContext = SDL_GL_CreateContext(_sdlWindow);
    _running = true;

    if(SDL_GetWindowWMInfo(_sdlWindow, &info))
    {
        _hWnd = info.info.win.window;
        int w, h;
        SDL_GetWindowSize(_sdlWindow, &w,&h);
        onSize(w,h);
        onInitialise(info.info.win.hdc);
    }

    SDL_Log( "Current Video Driver : %s",  SDL_GetCurrentVideoDriver());

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


