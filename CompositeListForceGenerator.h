#pragma once

#include "GSForceGenerator.h"

#include <vector>

template<class ParentContainerT, class ContainerT, class ParentT>
class CompositeListForceGenerator : public GSForceGenerator
{
public:
	CompositeListForceGenerator(ParentT *parent);
	~CompositeListForceGenerator();

	ParentT* parent();
	ContainerT* add(std::string id);
	ContainerT* element(size_t i);
	const ContainerT* element(size_t i) const;
	size_t size() const;
	
	void onInitialise(WorldSystem* pWorldSystem) override;
	void onBeforeUpdate( Particle *p, double dt) override;
	void onApplyForce( Particle *p, double dt ) override;
	void onAfterUpdate( Particle *p, double dt) override;
	void setRecorderHook(FlightRecorder& a) override;
    void drawForceGenerator(GSRigidBody* b, Renderer* args) override;
	
protected:
	virtual void onUpdateAverage();

	ParentT* _parent;
	ParentContainerT *_parentcontainer;
	std::vector<ContainerT*> _elements;
	ContainerT *_pNull;
	double _fAspectRatio;
};

#include "CompositeListForceGenerator.inl"
