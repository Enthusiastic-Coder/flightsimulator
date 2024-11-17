#pragma once

#include <jibbs/math/Quarternion.h>
#include <jibbs/vector/vector3.h>
#include <jibbs/math/MathSupport.h>
#include <jibbs/gps/GPSLocation.h>

template<class U>
class VirtualType 
{
public:
	VirtualType( U *realProp) : _ptr(realProp), _real(*realProp) {}

	U& get()		{ return *_ptr; }
	U* operator->()	{ return _ptr; }

	void toVirtual(){ switchModes( _real, _virtual ); }
	void toReal()	{ switchModes( _virtual, _real ); }
	
protected:
	U* _ptr;
	U& _real;
	U _virtual;

	void switchModes( U& pFrom, U& pTo )
	{
		if( _ptr == &pFrom )		
		{
			pTo = pFrom;
			_ptr = &pTo;
		}
	}
};

class ReferenceFrame
{
public:
    ReferenceFrame();

    virtual void toggleFrame();
    virtual void resetFrame();
    void setOrientation(const QuarternionD &q);
    void setEuler(const Vector3D& v );
    void setEuler( double x, double y, double z);
    const QuarternionD& getOrientation() const;
    const Vector3D& getEuler() const;
    bool isLocalFrame() const;
    void setWorldFrame();
    void setBodyFrame();
    Vector3D toLocalFrame( const Vector3D &v) const;
    Vector3D toNonLocalFrame( const Vector3D &v) const;

    virtual Vector3D toLocalTranslateFrame( const Vector3D &v) const =0;
    virtual Vector3D toNonLocalTranslateFrame( const Vector3D &v ) const=0;

protected:

    virtual void updateEuler();
    virtual void setOrientationHelper(double x, double y, double z);
    Vector3D toFrame(const QuarternionD & q, const Vector3D &v, const Vector3D &offset = Vector3D::Null() ) const;
    QuarternionD getOrientationInLocalFrame( bool bToLocal ) const;

    QuarternionD _orientation;
    Vector3D _euler;

private:
	bool _isLocal;
};

/////////////////////////////////////////

class GPSReferenceFrame : public virtual ReferenceFrame
{
public:
    Vector3D toLocalGroundFrame( const Vector3D &v) const;
    Vector3D toNonLocalGroundFrame( const Vector3D &v) const;
    const QuarternionD& getGroundOrientation() const;
    const QuarternionD& getQGps() const;

protected:
    virtual void resetFrame() override;
    virtual void updateEuler() override;

    QuarternionD _localOrientation;
    QuarternionD _gpsOrientation;
};

///////////////////////////////////////////

template<class T>
class VirtualRigidBodyReferenceFrame : public ReferenceFrame
{
public:
	VirtualRigidBodyReferenceFrame()
		:_v_orientation(&_orientation )
	{
	}

	QuarternionD& getVOrientation() override
	{
		return _v_orientation.get();
	}

protected:
	VirtualType<QuarternionD> _v_orientation;

};

