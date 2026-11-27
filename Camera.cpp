#include "stdafx.h"
#include "Camera.h"
#include <algorithm>
#include <cmath>
#include <limits>
#include "TurnDirection.h"

Camera::Camera() : _remoteView(0)
{
}

void Camera::moveForwards(double units, double diffang )
{
    if( _remoteView == 0)
        return;

    Vector3D velocityBody(diffang / 90 * units, 0, fabs(diffang) > std::numeric_limits<double>::epsilon() ? 0 : -units);

    Vector3D dP = QVRotate(_remoteView->getPosition().makeQOrientation(_remoteView->getOrientation().toDouble()), velocityBody);
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

	const Vector3F& locOrient = _localView.getOrientation();

	if (fabs(locOrient.y - orientation.y) > 45.0f)
		_localView.setOrientation(orientation);
}

void Camera::incrZoom(float fZoom)
{
    if( _remoteView ==0)
        return;

    _remoteView->incrZoom( fZoom);

    if (_remoteView->getZoom() < 1.0)
        _remoteView->setZoom( 1.0);
}

void Camera::setZOrientation(float z)
{
    if( _remoteView ==0)
        return;

    _remoteView->setZOrientation(z);
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

    if (_remoteView->getFixedTrackingMode())
    {
        // A fixed tripod head pans and tilts towards the aircraft without rolling.
        const double elapsed = std::clamp(dt, 0.0, 0.1);
        const double blend = -std::expm1(-elapsed / 0.18);
        auto orientation = _localView.getOrientation();
        const auto target = _remoteView->getOrientation();
        const auto follow = [blend, elapsed](double error) {
            return std::clamp(error * blend, -100.0 * elapsed, 100.0 * elapsed);
        };
        // Hold the pan near zenith, where a small track change can flip the bearing.
        const double panWeight = std::clamp((89.0 - std::abs(target.x)) / 9.0, 0.0, 1.0);
        orientation.x += follow(target.x - orientation.x);
        orientation.y += follow(panWeight * TurnDirection::GetTurnDiff(orientation.y, target.y));
        orientation.z = 0;
        _localView.setOrientation(orientation);
        _localView.setPosition(_remoteView->getPosition());
        _localView.setZoom(_localView.getZoom() + blend * (_remoteView->getZoom() - _localView.getZoom()));
        _bInTraverse = false;
        return;
    }

    float dOr = 360.0f/4.0f;
    float distance = _localView.getPosition().distanceTo(_remoteView->getPosition());
    Vector3F localOrientation = localView()->getOrientation();

    Vector3F diffOrient;
    bool _bInTraverse = distance > 1.0F;

    if( _bInTraverse ) //Keep orientation in direction of travel
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

    for( int i=0; i < 3; ++i)
    {
        float vX = *(&diffOrient.x +i);
        float orX = std::min( std::abs( vX)* 10*dt, dOr * dt);

        if( vX < 0)    orX = -orX;

        (&localOrientation.x)[i] += orX;
    }

    _localView.setOrientation(localOrientation);

    if( _bInTraverse )
    {
        float velocity = std::min(distance*dt, 1000*dt);

        QuarternionF qF = MathSupport<float>::MakeQOrientation(localOrientation);
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

    if( _bInTraverse )
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

bool Camera::inTraverseMode() const
{
    return _bInTraverse;
}

