#pragma once

#include <Quarternion.h>
#include <vector3.h>
#include <MathSupport.h>
#include <GPSLocation.h>

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
	ReferenceFrame()
	{
		_isLocal = true;
	}

	virtual void toggleFrame()
	{
		_isLocal = !_isLocal;
	}

	virtual void resetFrame()
	{
		_orientation.Reset();
		_euler.Reset();
	}

	void setOrientation(const QuarternionD &q)
	{
		_orientation = q;
		_orientation.Normalize();
		updateEuler();
	}

    void setEuler(const Vector3D& v )
    {
        setEuler( v.x, v.y, v.z);
    }

	void setEuler( double x, double y, double z)
	{
		setOrientationHelper(x, y, z);
		updateEuler();
	}

	const QuarternionD& getOrientation() const
	{
		return _orientation;
	}

	const Vector3D& getEuler() const
	{
		return _euler;
	}

	bool isLocalFrame() const
	{
		return _isLocal;
	}

	void setWorldFrame()
	{
		if( isLocalFrame() )
			toggleFrame();
	}

	void setBodyFrame()
	{
		if( !isLocalFrame() )
			toggleFrame();
	}

	Vector3D toLocalFrame( const Vector3D &v) const
	{
		return toFrame( getOrientationInLocalFrame(true), v );
	}

	Vector3D toNonLocalFrame( const Vector3D &v) const
	{
		return toFrame( getOrientationInLocalFrame(false), v );
	}

    virtual Vector3D toLocalTranslateFrame( const Vector3D &v) const =0;
    virtual Vector3D toNonLocalTranslateFrame( const Vector3D &v ) const=0;

protected:

	virtual void updateEuler()
	{
		_euler = MathSupport<double>::MakeEuler(getOrientation());
	}

	virtual void setOrientationHelper(double x, double y, double z)
	{
		_orientation = MathSupport<double>::MakeQ( x, y, z );
	}

	Vector3D toFrame(const QuarternionD & q, const Vector3D &v, const Vector3D &offset = Vector3D::Null() ) const
	{
		return QVRotate<>(q , v ) + offset;
	}

	QuarternionD getOrientationInLocalFrame( bool bToLocal ) const
	{
		return bToLocal ? ~getOrientation() : getOrientation();
	}

	QuarternionD _orientation;
	Vector3D _euler;

private:
	bool _isLocal;
};

/////////////////////////////////////////

class GPSRigidBodyReferenceFrame : public virtual ReferenceFrame
{
public:

////////////////////////////////////////////

    Vector3D toLocalGroundFrame( const Vector3D &v) const
    {
        return toFrame( ~_localOrientation, v );
    }

    Vector3D toNonLocalGroundFrame( const Vector3D &v) const
    {
        return toFrame( _localOrientation, v );
    }

    const QuarternionD& getGroundOrientation() const
    {
        return _localOrientation;
    }

    const QuarternionD& getQGps() const
    {
        return _gpsOrientation;
    }
/////////////////////////////////////////////////////


protected:

	virtual void resetFrame() override
	{
        ReferenceFrame::resetFrame();
        _localOrientation = QuarternionD();
        _gpsOrientation = QuarternionD();
	}


    QuarternionD _localOrientation;
    QuarternionD _gpsOrientation;

private:


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

