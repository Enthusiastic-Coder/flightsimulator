#pragma once

#include "RigidBody.h"
#include <vector3.h>
#include "CameraView.h"

class Camera
{
public:
    Camera();

    void processKey( double jump = 1.0f);
    void moveForwards(double units, double diffang = 0.0f);

    void onUpdate( double dt);

    void setRemoteViewPtr(CameraView *view);
    CameraView *localView();
    CameraView* remoteView();
    void fastForwardLocalView();

private:
    CameraView* _remoteView;
    CameraView _localView;

    Vector3F _velocity;
    Vector3F _acceleration;
    Vector3F _cameraFocusOffsetVel;
    Vector3F _cameraFocusOffsetAccl;

};
