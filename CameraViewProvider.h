#pragma once

#include "GPSLocation.h"
#include "vector3.h"
#include "interfaces.h"
#include "Camera.h"

class CameraViewProvider :
        public ICameraViewProvider,
        public IFilePersist
{
public:
    CameraViewProvider(int count);

//  ICameraPositionProvider
    bool nextView() override;
    bool prevView() override;
    int curViewIdx() const override;
    void updateCameraView() override;
    CameraView* getCameraView() override;
    std::string getCameraDescription() const override;

// IFilePersist
    void persistReadState(FILE* fPersistFile) override;
    void persistWriteState(FILE* fPersistFile) override;

private:
    int _viewIdx = 0;
    int _maxViewCount = 0;
};

