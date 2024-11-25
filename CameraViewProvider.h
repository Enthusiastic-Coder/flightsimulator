#pragma once

#include <jibbs/gps/GPSLocation.h>
#include <jibbs/vector/vector3.h>
#include "interfaces.h"

class CameraViewProvider :
        public ICameraViewProvider,
        public IFilePersist,
        public IRapidJsonPersist
{
public:
    CameraViewProvider(int count);

    CameraView* getCameraView(unsigned int idx);

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

// IRapidJsonPersist
    void persistReadState(rapidjson::Document& doc) override;
    void persistWriteState(rapidjson::Document &doc) override;

private:
    int _viewIdx = 0;
    int _maxViewCount = 0;
};

