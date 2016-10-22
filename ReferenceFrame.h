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

//////////////////////////////////////////////////////

template<class T>
class RigidBodyReferenceFrame : public ReferenceFrame
{
public:
	RigidBodyReferenceFrame()
	{
		_pT = static_cast<T*>(this);
	}

	void setCG(const Vector3D& cg)
	{
		_cg = cg;
	}

	const Vector3D& cg() const
	{
		return _cg;
	}

    virtual void toggleFrame() override
    {
        ReferenceFrame::toggleFrame();
        _pT->onToggleFrame( getOrientationInLocalFrame(isLocalFrame()) );
    }

	virtual Vector3D toLocalTranslateFrame( const Vector3D &v) const 
	{
        return toFrame(getOrientationInLocalFrame(true), v - cg() - _pT->position(), cg() );
	}

	virtual Vector3D toNonLocalTranslateFrame( const Vector3D &v ) const 
	{
        return toFrame(getOrientationInLocalFrame(false), v-cg(), cg() + _pT->position() );
	}

    virtual double Height() const = 0;

private:
	Vector3D _cg;
	T* _pT;
};

template<class T, class Base>
class GPSRigidBodyReferenceFrame : public Base
{
public:
    GPSRigidBodyReferenceFrame()
	{
		_pT = static_cast<T*>(this);
	}

	void setPosition( double x, double y, double z)
	{
		_gpsLocation.setFromPosition( Vector3D(x,y,z) );
		Base::setPosition( x,y,z );
	}

	void setPosition( const GPSLocation& gpsLocation )
	{
		_gpsLocation = gpsLocation;
		Base::setPosition( _gpsLocation.position() );
	}

	const GPSLocation& getGPSLocation() const
	{
		return _gpsLocation;
	}

	const QuarternionD& getGroundOrientation() const
	{
		return _localOrientation;
	}

	const QuarternionD& getQGps() const
	{
        return _gpsOrientation;
	}

	virtual Vector3D toLocalTranslateFrame( const Vector3D &v) const override
	{
		return toTranslateFrame( true, v );
	}

	virtual Vector3D toNonLocalTranslateFrame( const Vector3D &v ) const override
	{
		return toTranslateFrame( false, v );
	}

    virtual double Height() const = 0;

	Vector3D toLocalGroundFrame( const Vector3D &v) const
	{
		return toFrame( ~_localOrientation, v );
	}

	Vector3D toNonLocalGroundFrame( const Vector3D &v) const
	{
		return toFrame( _localOrientation, v );
	}

protected:

	virtual void resetFrame() override
	{
		Base::resetFrame();
		_gpsLocation = GPSLocation();
		_localOrientation = QuarternionD();
        _gpsOrientation = QuarternionD();
	}

	virtual void setOrientationHelper(double x, double y, double z) override
	{
		_orientation = getGPSLocation().makeQ( x, y, z );
	}

	virtual void updateEuler() override
	{
        _gpsLocation = _pT->position();
        _gpsOrientation = _gpsLocation.makeQ();
        _localOrientation = ~_gpsOrientation * getOrientation();
		_euler = MathSupport<double>::MakeEuler(_localOrientation);
	}

	Vector3D toTranslateFrame(bool bToLocal, const Vector3D &v ) const
	{
		const QuarternionD& qLocal = getGroundOrientation();
		const QuarternionD& qGps = getQGps();

		if( bToLocal )
			return toFrame( ~qLocal, toFrame( ~qGps, v - _pT->position(), - _pT->cg() ),  _pT->cg() );
		else
			return toFrame( qGps, toFrame( qLocal, v - _pT->cg(), _pT->cg() ), _pT->position() );
	}

private:
	GPSLocation _gpsLocation;
	QuarternionD _localOrientation;
    QuarternionD _gpsOrientation;
	T* _pT;
};

///////////////////////////////////////////

template<class T>
class VirtualRigidBodyReferenceFrame : public RigidBodyReferenceFrame<T>
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

