#pragma once

#include "GSForceGenerator.h"
#include "SimpleSpringModel.h"

class ConstrainedSpringForceGenerator : public GSForceGenerator
{
public:
	FORCEGENERATOR_TYPE( Type_SpringConstrained )
	ConstrainedSpringForceGenerator();

	bool isMaster();

	void onBeforeUpdate( Particle *p, double dt) override;
	void onApplyForce( Particle *p, double dt ) override;

protected:
	virtual void onInitialise(WorldSystem* pWorldSystem) override;

private:
public:
	Vector3D _partnerPosition;
	Vector3D _partnerTotalVelocity;
	GSRigidBody* _pPartnerBody;
	Vector3D _pPartnerConnectionPt;
	SimpleSpringModel *_pSpring;
	bool _bMaster;
	double _distance;
};


template<int t_id,class TRigidBody, class TForceGenerator=ConstrainedSpringForceGenerator>
class ConstrainedSpring
{
public:
	template<int t_id2,class TRigidBody2,class TForceGenerator2>
	void setPartner(Vector3D connectionPt, TRigidBody2* pPartnerBody, Vector3D connectionPartnerPt, SimpleSpringModel* pSpring, bool isMaster=true)
	{
		_constrainedContact._bMaster = isMaster;
		_constrainedContact.setContactPoint( connectionPt );
		_constrainedContact._pPartnerBody = pPartnerBody;
		_constrainedContact._pPartnerConnectionPt = connectionPartnerPt;

		if( _constrainedContact._bMaster )
		{
			static_cast< ConstrainedSpring<t_id2,TRigidBody2,TForceGenerator2>*>(pPartnerBody)
				->setPartner<t_id,TRigidBody,TForceGenerator>( connectionPartnerPt, 
								static_cast<TRigidBody*>(this), 
								connectionPt, 
								pSpring, 
								!_constrainedContact._bMaster );

			pSpring->calculate();
		}

		_constrainedContact._pSpring = pSpring;
	}

	void Attach( JSONRigidBody *pJSONBody, float fps )
	{
		pJSONBody->addForceGenerator( "constrainedContact", &_constrainedContact, fps );
	}

protected:
	TForceGenerator _constrainedContact;
};

/////////////////////////////////////////////////////

template<class TRigidBody>
class WheelConstrainedSpringForceGenerator : public ConstrainedSpringForceGenerator
{
public:
	FORCEGENERATOR_TYPE(Type_WheelSpringConstrained)

	void onAfterUpdate( Particle *p, double dt) override
	{
		ConstrainedSpringForceGenerator::onAfterUpdate(p, dt);

		if( _pSpring == nullptr )
			return;

		if( _distance > _pSpring->maxLen())
			return;

		TRigidBody* pBody = static_cast<TRigidBody*>(p);
        pBody->_front_wheel.spring().setSteeringAngle(_pPartnerBody->toNonLocalGroundFrame( _pPartnerBody->getEuler() ).y - pBody->getEuler().y);
	}
};

/////////////////////////////////////////////////////

template<int t_id,class T>
class WheelConstrainedSpring : public ConstrainedSpring<t_id,T,WheelConstrainedSpringForceGenerator<T>>
{
public:
};
