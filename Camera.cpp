#include "stdafx.h"
#include "Camera.h"
#include <algorithm>
#include "TurnDirection.h"

#include <Windows.h>

Camera::Camera() : _remoteView(0)
{
}

void Camera::processKey( double jump)
{
    if( _remoteView ==0 )
        return;

    bool bShiftOn = GetAsyncKeyState(VK_SHIFT) < 0;
    bool bControlOn = GetAsyncKeyState(VK_CONTROL) < 0;

    jump = (bShiftOn ? 100 : jump);

    if (bControlOn)
        jump /= 10;

    if( ::GetAsyncKeyState(VK_LEFT) < 0)
        _remoteView->incrOrientation(0, 0,  -(bShiftOn ? 5 : 0.5));

    if( ::GetAsyncKeyState(VK_RIGHT) < 0)
        _remoteView->incrOrientation(0, 0, (bShiftOn ? 5 : 0.5));

    if( ::GetAsyncKeyState(VK_UP) < 0)
        _remoteView->incrOrientation((bShiftOn ? 5 : 0.5), 0, 0);

    if( ::GetAsyncKeyState(VK_DOWN) < 0)
        _remoteView->incrOrientation(- (bShiftOn ? 5 : 0.5), 0, 0);

    if( ::GetAsyncKeyState('Z') < 0 || ::GetAsyncKeyState('X') < 0)
    {
        int iFactor = ::GetAsyncKeyState('Z') <0 ? -1 : 1;
        _remoteView->incrOrientation(0, iFactor * (bShiftOn ? jump / 10 : (bControlOn ? jump * 20 : jump)),0);

        Vector3F orientation = _remoteView->getOrientation();
        if( ::GetAsyncKeyState('Z') < 0 )
        {
            if (_remoteView->getOrientation().y < 0)
            {
                orientation.y += 360;
                _remoteView->incrOrientation(orientation);
            }
        }
        else
        {
            if (_remoteView->getOrientation().y > 359)
            {
                orientation.y = 360 - orientation.y;
                _remoteView->setOrientation(orientation);
            }
        }
    }

    if( ::GetAsyncKeyState('Q') < 0)
        moveForwards(jump);

    if( ::GetAsyncKeyState('A') < 0)
        moveForwards(-jump);

    if( ::GetAsyncKeyState('C') < 0)
        moveForwards(jump * 2, -90);

    if( ::GetAsyncKeyState('V') < 0)
        moveForwards(jump * 2, 90);

    if( ::GetAsyncKeyState('W') < 0)
    {
        GPSLocation loc = _remoteView->getPosition();
        loc._height += jump/4;
        _remoteView->setPosition(loc);
    }

    if( ::GetAsyncKeyState('S') < 0)
    {
        GPSLocation loc = _remoteView->getPosition();
        loc._height -= jump/4;
        _remoteView->setPosition(loc);
    }

    if( ::GetAsyncKeyState(VK_SPACE) < 0)
        _remoteView->setZOrientation(0.0f);

    if (::GetAsyncKeyState(VK_OEM_MINUS) < 0 )
        _remoteView->incrZoom( -0.1);

    if (::GetAsyncKeyState(VK_OEM_PLUS) < 0)
        _remoteView->incrZoom( 0.1);

    if (_remoteView->getZoom() < 1.0)
        _remoteView->setZoom( 1.0);
}

void Camera::moveForwards(double units, double diffang )
{
    if( _remoteView == 0)
        return;

    Vector3D velocityBody(diffang / 90 * units, 0, fabs(diffang) > DBL_EPSILON ? 0 : -units);

    Vector3D dP = QVRotate(_remoteView->getPosition().makeQ(_remoteView->getOrientation().toDouble()), velocityBody);
    Vector3D oldPosition = _remoteView->getPosition().position();
    _remoteView->setPosition( _remoteView->getPosition().position() + dP);

    if( _remoteView->getPosition().position().z <0 && oldPosition.z > 0)
        _remoteView->incrOrientation(0, 180, 0);
    else if (_remoteView->getPosition().position().z > 0 && oldPosition.z < 0)
        _remoteView->incrOrientation(0, -180, 0);

    fastForwardLocalView();
}

void Camera::moveUp(float fDist)
{
    if( _remoteView ==0)
        return;

    GPSLocation loc = _remoteView->getPosition();
    loc._height += fDist;
    _remoteView->setPosition(loc);
}

void Camera::incrOrientation(float x, float y, float z)
{
    if( _remoteView ==0)
        return;

    Vector3F orientation = _remoteView->getOrientation();

    _remoteView->incrOrientation(x, y, z);

    if (_remoteView->getOrientation().y < 0)
    {
        orientation.y += 360;
        _remoteView->incrOrientation(orientation);
    }
    if (_remoteView->getOrientation().y > 359)
    {
        orientation.y = 360 - orientation.y;
        _remoteView->setOrientation(orientation);
    }
}

void Camera::onUpdate(double dt)
{
    if( _remoteView == 0) //No remote destination so don't move local cameraview;
        return;

    if( !_remoteView->getSmoothMode())
    {
        fastForwardLocalView();
        return;
    }

    float dOr = 360.0f/4.0f;
    float distance = _localView.getPosition().distanceTo(_remoteView->getPosition());
    Vector3F localOrientation = localView()->getOrientation();

    Vector3F diffOrient;
    bool bInTraverseMode = distance > 1.0F;

    if( bInTraverseMode ) //Keep orientation in direction of travel
    {
        float bearing = _localView.getPosition().bearingTo( _remoteView->getPosition());
        diffOrient = Vector3F(0,bearing, 0) - localOrientation;
        diffOrient.y = TurnDirection::GetTurnDiff(localOrientation.y, bearing);
    }
    else
    {
        diffOrient = _remoteView->getOrientation() - localOrientation;
        diffOrient.y = TurnDirection::GetTurnDiff(localOrientation.y, _remoteView->getOrientation().y);
     }

    int focusRange = 10;
    float* cfrv = &_cameraFocusOffsetVel.x;
    float* cfra = &_cameraFocusOffsetAccl.x;
    for( int i=0; i < 3; ++i)
    {
//        float vX = *(&diffOrient.x +i);
//        float orX = std::min( std::abs( vX)* 10*dt, dOr * dt);

        if( !bInTraverseMode && _remoteView->getShakingMode())
        {
            cfra[i] += (std::rand() % (focusRange*2) - focusRange )/4.0f;

            if( cfra[i] < -focusRange)
                cfra[i] = -focusRange;

            if( cfra[i] > focusRange)
                cfra[i] = focusRange;

            cfrv[i] += cfra[i] * dt;

            if( cfrv[i] < -focusRange)
            {
                cfrv[i] = -focusRange;
                cfra[i] = 0;
            }

            if( cfrv[i] > focusRange)
            {
                cfrv[i] = focusRange;
                cfra[i] = 0;
            }

            (&localOrientation.x)[i] +=cfrv[i] * dt;
//            orX += cfrv[i] * dt;
        }

        float vX = *(&diffOrient.x +i);
        float orX = std::min( std::abs( vX)* 10*dt, dOr * dt);

        if( vX < 0)    orX = -orX;

        (&localOrientation.x)[i] += orX;
    }

    _localView.setOrientation(localOrientation);

    if( bInTraverseMode )
    {
        float velocity = std::min(distance*dt, 1000*dt);

        QuarternionF qF = MathSupport<float>::MakeQ(localOrientation);
        Vector3F person(0,0,-velocity);
        person= QVRotate( qF, person);

        GPSLocation gpsPerson = _localView.getPosition() + person;
        _localView.setPosition(gpsPerson);

        float verticalVelDiff = _remoteView->getPosition()._height - _localView.getPosition()._height;
        float verticalIncr = std::min( std::abs(verticalVelDiff)/2.0, 50 * dt);

        if( verticalVelDiff < 0)  verticalIncr = -verticalIncr;

        _localView.setPosition(_localView.getPosition() + Vector3F(0,verticalIncr,0));
    }
    else
        _localView.setPosition(_remoteView->getPosition());

    float zoomValue = _localView.getZoom();

    float diffZoom = 0;

    if( bInTraverseMode )
        diffZoom = 1 - _localView.getZoom();
    else
        diffZoom = _remoteView->getZoom() - _localView.getZoom();

    float incrZoom = std::min( std::abs(diffZoom * dt), 2.0f * dt);
    if( diffZoom < 0)
        incrZoom = -incrZoom;

    zoomValue += incrZoom;
    _localView.setZoom(zoomValue);
}

void Camera::setRemoteViewPtr(CameraView *view)
{
    _remoteView = view;
}

CameraView* Camera::localView()
{
    return &_localView;
}

CameraView *Camera::remoteView()
{
    return _remoteView;
}

void Camera::fastForwardLocalView()
{
    if( _remoteView != 0)
        _localView = *_remoteView;
}

