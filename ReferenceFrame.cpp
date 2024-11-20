#include "stdafx.h"
#include "ReferenceFrame.h"

ReferenceFrame::ReferenceFrame()
{
    _isLocal = true;
}

void ReferenceFrame::toggleFrame()
{
    _isLocal = !_isLocal;
}

void ReferenceFrame::resetFrame()
{
    _orientation.Reset();
    _euler.Reset();
}

void ReferenceFrame::setOrientation(const QuarternionD &q)
{
    _orientation = q;
    _orientation.Normalize();
    updateEuler();
}

void ReferenceFrame::setEuler(const Vector3D &v)
{
    setEuler( v.x, v.y, v.z);
}

void ReferenceFrame::setEuler(double x, double y, double z)
{
    setOrientationHelper(x, y, z);
    updateEuler();
}

const QuarternionD &ReferenceFrame::getOrientation() const
{
    return _orientation;
}

const Vector3D &ReferenceFrame::getEuler() const
{
    return _euler;
}

bool ReferenceFrame::isLocalFrame() const
{
    return _isLocal;
}

void ReferenceFrame::setWorldFrame()
{
    if( isLocalFrame() )
        toggleFrame();
}

void ReferenceFrame::setBodyFrame()
{
    if( !isLocalFrame() )
        toggleFrame();
}

Vector3D ReferenceFrame::toLocalFrame(const Vector3D &v) const
{
    return toFrame( getOrientationInLocalFrame(true), v );
}

Vector3D ReferenceFrame::toNonLocalFrame(const Vector3D &v) const
{
    return toFrame( getOrientationInLocalFrame(false), v );
}

void ReferenceFrame::updateEuler()
{
    _euler = -MathSupport<double>::MakeEuler(getOrientation());
}

void ReferenceFrame::setOrientationHelper(double x, double y, double z)
{
    _orientation = MathSupport<double>::MakeQ( x, y, z );
}

Vector3D ReferenceFrame::toFrame(const QuarternionD &q, const Vector3D &v, const Vector3D &offset) const
{
    return QVRotate<>(q , v ) + offset;
}

QuarternionD ReferenceFrame::getOrientationInLocalFrame(bool bToLocal) const
{
    return bToLocal ? ~getOrientation() : getOrientation();
}

/////////////////////////////////////////////////////////////////////////////////

Vector3D GPSReferenceFrame::toLocalGroundFrame(const Vector3D &v) const
{
    return toFrame( ~_localOrientation, v );
}

Vector3D GPSReferenceFrame::toNonLocalGroundFrame(const Vector3D &v) const
{
    return toFrame( _localOrientation, v );
}

const QuarternionD &GPSReferenceFrame::getGroundOrientation() const
{
    return _localOrientation;
}

const QuarternionD &GPSReferenceFrame::getQGps() const
{
    return _gpsOrientation;
}

void GPSReferenceFrame::resetFrame()
{
    ReferenceFrame::resetFrame();
    _localOrientation = QuarternionD();
    _gpsOrientation = QuarternionD();
}

void GPSReferenceFrame::updateEuler()
{
    _localOrientation = ~_gpsOrientation * getOrientation();
    _euler = -MathSupport<double>::MakeEuler(_localOrientation);
}
