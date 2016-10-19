#pragma once

#include "RigidBody.h"
#include <vector3.h>
#include "CameraView.h"

class Camera
{
public:
    Camera();

    void moveForwards(double units, double diffang = 0.0f);
    void moveUp(float fDist);

    void incrOrientation(float x, float y, float z);
    void incrZoom(float fZoom);

    void setZOrientation(float z);
    void setZoom(float fZoom);

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
