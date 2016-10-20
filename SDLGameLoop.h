#ifndef __SDLGAMELOOP_H__
#define __SDLGAMELOOP_H__

#include <SDL.h>
#include <SDLTimer.h>
#include "interfaces.h"
#include <include_gl.h>
#include <string>

class SDLGameLoop : public IScreenMouseInfo
{
public:
    SDLGameLoop();

    bool run(std::string strTitle);

    void setFramePerSecond(float fps);
    void setRunning( bool bRunning );
    bool isRunning();
    float frameTime();
    void update();
    void postQuit();

    virtual bool onInitialise() {return false;}
    virtual void onUnInitialise() {}
    virtual void onUpdate() {}
    virtual void onRender() {}
    virtual void onHandleAsyncKeys(const Uint8* keys, SDL_Keymod mod, float dt)
    {SDL_UNUSED(keys);SDL_UNUSED(mod);SDL_UNUSED(dt);}

    virtual void onKeyDown(SDL_KeyboardEvent* ) {}
    virtual void onKeyUp(SDL_KeyboardEvent* ) {}
    virtual void onMouseMotion(SDL_MouseMotionEvent* ) {}
    virtual void onMouseDown(SDL_MouseButtonEvent* ) {}
    virtual void onMouseUp(SDL_MouseButtonEvent* ) {}
    virtual void onMouseWheel(SDL_MouseWheelEvent * ) {}
    virtual void onFingerDown(SDL_TouchFingerEvent* ) {}
    virtual void onFingerMotion(SDL_TouchFingerEvent* ) {}
    virtual void onFingerUp(SDL_TouchFingerEvent* ) {}
    virtual void onSize(int width, int height) { glViewport( 0, 0, width, height ); }
    virtual void onMove(int x, int y) {SDL_UNUSED(x);SDL_UNUSED(y);}

    virtual void GetMousePos(int& x, int& y) const override;
    virtual void GetScreenDims(int& w, int &h) const override;
    virtual void GetScreenPos(int& x, int &y) const override;

private:
    bool onDispatchMessage(SDL_Event& event);
    void onTimeOut();

private:
    SDL_Window* _sdlWindow;
    SDL_GLContext _glContext;
    Uint32 _updateEventType;

    SDLTimer _timer;
    float _fInverseFPS;
    float _dt;
    bool _running;

protected:
    SDL_Window *getWindow();
};

#endif //__SDLGAMELOOP_H__
