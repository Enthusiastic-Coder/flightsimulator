
template<class ParentContainerT, class ContainerT, class ParentT>
CompositeListForceGenerator<ParentContainerT,ContainerT,ParentT>::CompositeListForceGenerator(ParentT *parent) :
	_parentcontainer(static_cast<ParentContainerT*>(this)),
	_parent( parent ),
	_fAspectRatio(0.0f)
{ 
	_pNull = new ContainerT(_parentcontainer);
}

template<class ParentContainerT, class ContainerT, class ParentT>
CompositeListForceGenerator<ParentContainerT,ContainerT,ParentT>::~CompositeListForceGenerator()
{
	for( size_t i=0; i < _elements.size(); i++ )
		delete _elements[i];

	if( _pNull )
	{
		delete _pNull;
		_pNull = nullptr;
	}
}

template<class ParentContainerT, class ContainerT, class ParentT>
ParentT* CompositeListForceGenerator<ParentContainerT,ContainerT,ParentT>::parent()
{
	return _parent;
}

template<class ParentContainerT, class ContainerT, class ParentT>
ContainerT* CompositeListForceGenerator<ParentContainerT,ContainerT,ParentT>::add(std::string id)
{
	ContainerT *u =	new ContainerT(_parentcontainer);
	return _elements.push_back(u), u;
}

template<class ParentContainerT, class ContainerT, class ParentT>
void CompositeListForceGenerator<ParentContainerT,ContainerT,ParentT>::onBeforeUpdate( Particle *p, double dt) 
{
	size_t count = _elements.size();

	for( size_t i=0; i < count; i++)
		_elements[i]->onBeforeUpdate(p, dt);
}

template<class ParentContainerT, class ContainerT, class ParentT>
void CompositeListForceGenerator<ParentContainerT,ContainerT,ParentT>::onApplyForce( Particle *p, double dt ) 
{
	for( size_t i=0; i < _elements.size(); i++)
		_elements[i]->onApplyForce(p, dt);

	onUpdateAverage();
}

template<class ParentContainerT, class ContainerT, class ParentT>
void CompositeListForceGenerator<ParentContainerT,ContainerT,ParentT>::onAfterUpdate( Particle *p, double dt) 
{
	for( size_t i=0; i < _elements.size(); i++)
		_elements[i]->onAfterUpdate(p, dt);
}

template<class ParentContainerT, class ContainerT, class ParentT>
void CompositeListForceGenerator<ParentContainerT,ContainerT,ParentT>::drawForceGenerator(GSRigidBody* b, Renderer* r)
{
	for( size_t i=0; i < _elements.size(); i++)
        _elements[i]->drawForceGenerator(b, r);
}

template<class ParentContainerT, class ContainerT, class ParentT>
ContainerT* CompositeListForceGenerator<ParentContainerT,ContainerT,ParentT>::element(size_t i)
{
	if( i < _elements.size() )
		return _elements[i];

	return _pNull;
}

template<class ParentContainerT, class ContainerT, class ParentT>
size_t CompositeListForceGenerator<ParentContainerT,ContainerT,ParentT>::size() const
{
	return _elements.size();
}

template<class ParentContainerT, class ContainerT, class ParentT>
void CompositeListForceGenerator<ParentContainerT,ContainerT,ParentT>::onInitialise(WorldSystem* pWorldSystem)
{
	GSForceGenerator::onInitialise(pWorldSystem);

	for( size_t i=0; i < _elements.size(); i++)
		_elements[i]->onInitialise(pWorldSystem);
}

template<class ParentContainerT, class ContainerT, class ParentT>
void CompositeListForceGenerator<ParentContainerT,ContainerT,ParentT>::setRecorderHook(FlightRecorder& a)
{
	GSForceGenerator::setRecorderHook(a);
	
	for( size_t i=0; i < _elements.size(); i++ )
		_elements[i]->setRecorderHook(a);
}


template<class ParentContainerT, class ContainerT, class ParentT>
void CompositeListForceGenerator<ParentContainerT,ContainerT,ParentT>::onUpdateAverage()
{
	size_t count = _elements.size();
	if( count == 0 ) return;

    _appliedForce = Vector3D();
    _contactPoint = Vector3D();

	for( size_t i=0; i < count; i++)
	{
		_appliedForce += _elements[i]->getAppliedForce();
		_contactPoint += _elements[i]->getContactPoint();
	}

	_contactPoint /= count;
	_appliedForce /= count;
}
