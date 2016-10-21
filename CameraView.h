#ifndef __CAMERA_VIEW__
#define __CAMERA_VIEW__

#include <map>
#include <string>
#include <vector3.h>
#include <GPSLocation.h>
#include <interfaces.h>

class CameraView : public IFilePersist, public IRapidJsonPersist
{
private:
    class GroupCameraView
    {
    public:
        ~GroupCameraView();

        CameraView* getView(unsigned int idx);
    private:
        std::map<unsigned int,CameraView*> _cameraViews;
    };

public:

    class Manager
    {
    public:
        static Manager* get();

        CameraView* getView(void* thiz, unsigned int idx);
        void releaseView(void* thiz);

    private:
        Manager() { }
        std::map<void*,GroupCameraView> _groupView;
    };

    void setDescription(std::string str);
    void setPosition(const Vector3D& loc);
    void setPosition(const GPSLocation& loc);
    void setOrientation(const Vector3F& orientation);
    void setZoom(double zoom);
    void setSmoothMode(bool bSmoothTransition);
    void setShakingMode(bool bShaking);
    void setXOrientation(float x);
    void setYOrientation(float y);
    void setZOrientation(float z);

    void incrZoom(double diff);
    void incrOrientation(const Vector3F &v);
    void incrOrientation(float dx, float dy, float dz);

    std::string getDescription() const;
    GPSLocation getPosition() const;
    Vector3F getOrientation() const;
    double getZoom() const;
    bool getSmoothMode() const;
    bool getShakingMode() const;

// IFilePersist
    void persistReadState(FILE* fPersistFile) override;
    void persistWriteState(FILE* fPersistFile) override;

// IRapidJsonPersist
    void persistReadState(rapidjson::Document& doc) override;
    void persistWriteState(rapidjson::Document& doc) override;

private:
    std::string _description;
    GPSLocation _location;
    Vector3F _orientation;
    double _zoom = 1.0;
    bool _bSmoothTransition = true;
    bool _bShakingMode = false;
};


#endif
