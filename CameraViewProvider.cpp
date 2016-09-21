#include "stdafx.h"
#include "CameraViewProvider.h"

CameraViewProvider::CameraViewProvider(int count)
{
    _maxViewCount = count;
}

bool CameraViewProvider::nextView()
{
    if( ++_viewIdx > _maxViewCount)
        _viewIdx = 0;
    return _viewIdx != 0;
}

bool CameraViewProvider::prevView()
{
    if( --_viewIdx < 0)
        _viewIdx = _maxViewCount;
    return _viewIdx != 0;
}

void CameraViewProvider::updateCameraView()
{
}

int CameraViewProvider::curViewIdx() const
{
    return _viewIdx;
}

CameraView *CameraViewProvider::getCameraView()
{
    return CameraView::Manager::get()->getView(this, curViewIdx());
}

std::string CameraViewProvider::getCameraDescription() const
{
    return CameraView::Manager::get()->getView(const_cast<CameraViewProvider*>(this), curViewIdx())->getDescription();
}

void CameraViewProvider::persistReadState(FILE *fPersistFile)
{
    for( int i = 1; i <= _maxViewCount; ++i)
        CameraView::Manager::get()->getView(this, i)->persistReadState(fPersistFile);

}

void CameraViewProvider::persistWriteState(FILE *fPersistFile)
{
    for( int i = 1; i <= _maxViewCount; ++i)
        CameraView::Manager::get()->getView(this, i)->persistWriteState(fPersistFile);

}
