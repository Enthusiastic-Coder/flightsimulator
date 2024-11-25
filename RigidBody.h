#pragma once

#include <jibbs/vector/vector3.h>
#include <jibbs/matrix/Matrix3x3.h>

class Particle
{
public:
	Particle();

	void setMass(double mass);
	double getMass() const;
	void setPosition( double x, double y, double z);
	void setPosition( const Vector3D &v );
	void setVelocity( double x, double y, double z );
	void setVelocity( const Vector3D &v );
	const Vector3D& velocity() const;
	const Vector3D& position() const;
	const Vector3D& getForce() const;
	void setForce( const Vector3D& force );
	virtual void applyForce( const Vector3D &r, const Vector3D &f );
	void applyForce(const Vector3D& f);
	virtual void update(double dt);
	virtual void reset();
	virtual bool isParticle();

protected:
	double _mass;
	Vector3D _position;
	Vector3D _velocity;
	Vector3D _force;
};


//////////////////////////////////////////////////////

#include "ReferenceFrame.h"

class RigidBody : public Particle, public virtual ReferenceFrame
{
public:

/// Initial Conditions
	void setInertiaMatrix(const Matrix3x3D& inertia);
    void setAngularVelocity( const Vector3D& v);
	void setAngularVelocity( double x, double y, double z);
	void setTorque( const Vector3D &rhs );

/// Simulation in RealTime
    virtual void reset() override;
    virtual void applyForce( const Vector3D &r, const Vector3D &f ) override;
    virtual void update(double dt) override;
////////////////////////////

    virtual double Height() const
    {
        return position().y;
    }

    void setCG(const Vector3D& cg)
    {
        _cg = cg;
    }

    const Vector3D& cg() const
    {
        return _cg;
    }

//// ReferenceFrame
    virtual void toggleFrame() override
    {
        ReferenceFrame::toggleFrame();
        onToggleFrame( getOrientationInLocalFrame(isLocalFrame()) );
    }

    virtual Vector3D toLocalTranslateFrame( const Vector3D &v) const override
    {
        return toFrame(getOrientationInLocalFrame(true), v - cg() - position(), cg() );
    }

    virtual Vector3D toNonLocalTranslateFrame( const Vector3D &v ) const override
    {
        return toFrame(getOrientationInLocalFrame(false), v-cg(), cg() + position() );
    }


/// State of Body
	const Vector3D& angularVelocity() const;
	virtual const Vector3D& getTorque() const;

	virtual bool isParticle() override;
	virtual void onToggleFrame(const QuarternionD& q);

private:
	Matrix3x3D _inertia;
	Matrix3x3D _inverse_inertia;
    Vector3D _cg;

protected:
	Vector3D _torque;
	Vector3D _angular_vel;
};

