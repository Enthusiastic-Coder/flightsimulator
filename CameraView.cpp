#include "CameraView.h"

CameraView::GroupCameraView::~GroupCameraView()
{
    for( std::map<unsigned int, CameraView*>::iterator it = _cameraViews.begin(); it != _cameraViews.end(); ++it)
        if( it->second != 0)
            delete it->second;

    _cameraViews.clear();
}

CameraView::Manager *CameraView::Manager::get()
{
    static Manager manager;
    return &manager;
}

CameraView *CameraView::Manager::getView(void *thiz, unsigned int idx)
{
    return _groupView[thiz].getView(idx);
}

void CameraView::Manager::releaseView(void *thiz)
{
    _groupView.erase(thiz);
}

CameraView *CameraView::GroupCameraView::getView(unsigned int idx)
{
    CameraView* view = _cameraViews[idx];

    if( view == 0)
    {
        view = new CameraView;
        _cameraViews[idx] = view;
    }

    return view;
}

void CameraView::setDescription(std::string str)
{
    _description = str;
}

void CameraView::setPosition(Vector3D &loc)
{
    _location = loc;
}

void CameraView::setPosition(GPSLocation &loc)
{
    _location = loc;
}

void CameraView::setOrientation(Vector3F &orientation)
{
    _orientation = orientation;
}

void CameraView::setZoom(double zoom)
{
    _zoom = zoom;
}

void CameraView::setSmoothMode(bool bSmoothTransition)
{
    _bSmoothTransition = bSmoothTransition;
}

void CameraView::setShakingMode(bool bShaking)
{
    _bShakingMode = bShaking;
}

void CameraView::setXOrientation(float x)
{
    _orientation.x = x;
}

void CameraView::setYOrientation(float y)
{
    _orientation.y = y;
}

void CameraView::setZOrientation(float z)
{
    _orientation.z = z;
}

void CameraView::incrOrientation(float dx, float dy, float dz)
{
    _orientation.x += dx;
    _orientation.y += dy;
    _orientation.z += dz;
}

std::string CameraView::getDescription() const
{
    return _description;
}

GPSLocation CameraView::getPosition() const
{
    return _location;
}

Vector3F CameraView::getOrientation() const
{
    return _orientation;
}

double CameraView::getZoom() const
{
    return _zoom;
}

bool CameraView::getSmoothMode() const
{
    return _bSmoothTransition;
}

bool CameraView::getShakingMode() const
{
    return _bShakingMode;
}

void CameraView::incrZoom(double diff)
{
    _zoom += diff;
}

void CameraView::persistReadState(FILE *fPersistFile)
{
    fread(&_location,sizeof(GPSLocation), 1, fPersistFile);
    fread(&_orientation,sizeof(Vector3F), 1, fPersistFile);
    fread(&_zoom,sizeof(double), 1, fPersistFile);
}

void CameraView::persistWriteState(FILE *fPersistFile)
{
    fwrite(&_location,sizeof(GPSLocation), 1, fPersistFile);
    fwrite(&_orientation,sizeof(Vector3F), 1, fPersistFile);
    fwrite(&_zoom,sizeof(double), 1, fPersistFile);
}


