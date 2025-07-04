#include "stdafx.h"
#include "JSONRigidBodyCollection.h"
#include "WorldSystem.h"
#include "RigidBodyMeshManager.h"
#include "OpenGLRenderer.h"
#include "HeightData.h"

JSONRigidBodyCollection::JSONRigidBodyCollection()
{
	_focusRigidBody = _bodyList.begin();
}

JSONRigidBodyCollection::~JSONRigidBodyCollection()
{
    unLoadAll();
}

void JSONRigidBodyCollection::clear()
{
	_bodyList.clear();
}

size_t JSONRigidBodyCollection::bodyCount()
{
	return _bodyList.size();
}

size_t JSONRigidBodyCollection::staticBodyCount()
{
	return _staticBodyList.size();
}

void JSONRigidBodyCollection::onInitialise()
{

}

void JSONRigidBodyCollection::unLoadAll()
{
    for( JSONRigidBody* it : _bodyList )
        delete it;

    _bodyList.clear();

    for( JSONRigidBody* it : _staticBodyList )
        delete it;

    _staticBodyList.clear();

    RigidBodyMeshManager::get().unLoadAll();
}

void JSONRigidBodyCollection::onUnInitialise()
{
    //unLoadAll();
}

void JSONRigidBodyCollection::addJSONBody( WorldSystem* pWorldSystem, JSONRigidBody* pRigidBody )
{
	MeshModel* m = pRigidBody->getMeshModel();
	if( m == nullptr)
		pRigidBody->setMeshModel(m);
    
	pRigidBody->onInitialise(pWorldSystem);
	_bodyList.push_back( pRigidBody );

	_focusRigidBody = _bodyList.end();
	--_focusRigidBody;
	nextFocusedRigidBody();
}

void JSONRigidBodyCollection::addStaticJSONBody(WorldSystem *pWorldSystem, JSONRigidBody* pRigidBody, MeshModel* model)
{
	_staticBodyList.push_back( pRigidBody );
	MeshModel* m = _staticBodyList.back()->getMeshModel();
	if( m == nullptr)
		_staticBodyList.back()->setMeshModel(model);

    _staticBodyList.back()->onInitialise(pWorldSystem);
}

JSONRigidBody* JSONRigidBodyCollection::createJSONBody( WorldSystem* pWorldSystem, std::string sName, JSONRigidBody::Type typeFlags )
{
    addJSONBody(pWorldSystem, new JSONRigidBody(sName, typeFlags) );
	return _bodyList.back();
}

JSONRigidBody* JSONRigidBodyCollection::createStaticJSONBody( WorldSystem* pWorldSystem, std::string sName, JSONRigidBody::Type typeFlags )
{
    addStaticJSONBody( pWorldSystem, new JSONRigidBody(sName, typeFlags) );
	return _staticBodyList.back();
}

bool JSONRigidBodyCollection::getHeightFromPosition( const GPSLocation& position, HeightData& heightData ) const
{
	std::vector<HeightData> planes;

    for( JSONRigidBody* it : _staticBodyList )
	{
		if( it->typeMask(JSONRigidBody::Type::PLANE) == false )
			continue;

		HeightData pe;
        if( it->getHeightFromPosition( position, pe ) )
		{
			planes.push_back( pe );
			break;
		}
	}

    for( JSONRigidBody* it : _bodyList )
	{
		if( it->typeMask( JSONRigidBody::Type::PLANE) == false )
			continue;

		HeightData pe;
        if( it->getHeightFromPosition( position, pe ) )
		{
			planes.push_back( pe );
			break;
		}
	}

	size_t idx = GetHeightIndexFromPlanes(planes);
	
	if( idx == -1 )
		return false;

	heightData = planes[idx];
	return true;
}

void JSONRigidBodyCollection::update( double dt )
{
    for( JSONRigidBody* it : _staticBodyList )
		it->onUpdate( dt );

    for (JSONRigidBody* it : _bodyList)
	{
		it->onForceGeneratorCheckActive();
		it->onBeforeUpdate(dt);
	}

    for (JSONRigidBody* it : _bodyList)
	{
		if (it->getState() == JSONRigidBody::STATE::PLAYBACK)
			it->onPlayRecording(dt);
		else
			it->onUpdate(dt);
    }

    for (JSONRigidBody* it : _bodyList)
		it->onAfterUpdate(dt);
}

void JSONRigidBodyCollection::Render(Renderer *args, bool bReflection, unsigned int shadowMapCount, double dt)
{
    for (JSONRigidBody* it : _bodyList)
		if (it->getShowState())
            it->Render(args, bReflection, shadowMapCount);

    for (JSONRigidBody* it : _staticBodyList)
		if (it->getShowState())
            it->Render(args, bReflection, shadowMapCount);
}

void JSONRigidBodyCollection::RenderForceGenerators(Renderer* r)
{
    for (JSONRigidBody* it : _bodyList)
		if (it->getShowState())
            it->renderForceGenerators(r);

    for (JSONRigidBody* it : _staticBodyList)
		if (it->getShowState())
            it->renderForceGenerators(r);
}

bool JSONRigidBodyCollection::onAsyncKeyPress(IScreenMouseInfo* scrn, float dt)
{
	JSONRigidBody* body = activeRigidBody();
	if(body ==0) return false;
	return body->onAsyncKeyPress(scrn, dt);
}

bool JSONRigidBodyCollection::onSyncKeyPress()
{
	JSONRigidBody* body = activeRigidBody();
	if(body ==0) return false;
	return body->onSyncKeyPress();
}

bool JSONRigidBodyCollection::toggleUsingMouse()
{
    auto* focus = focusedRigidBody();
    if(focus != 0)
    {
        focus->setUsingMouse( !focusedRigidBody()->isUsingMouse() );
        return true;
    }

	return false;
}

bool JSONRigidBodyCollection::onMouseMove( int x, int y)
{
	JSONRigidBody* body = activeRigidBody();
	if( body == 0) return false;
    return body->onMouseMove(x, y );
}

bool JSONRigidBodyCollection::onMouseWheel(int wheelDelta)
{
	JSONRigidBody* body = activeRigidBody();
	if( body == 0) return false;
    return body->onMouseWheel( wheelDelta );
}

bool JSONRigidBodyCollection::nextView()
{
	JSONRigidBody* body = activeRigidBody();
	if( body == 0) return false;
    return body->nextView();
}

bool JSONRigidBodyCollection::prevView()
{
	JSONRigidBody* body = activeRigidBody();
	if( body == 0) return false;
	return body->prevView();
}

void JSONRigidBodyCollection::incrChaseAngle(float fDiff)
{
    JSONRigidBody* pBody = focusedRigidBody();
    if( pBody )
        pBody->incrChaseAngle(fDiff);
}

void JSONRigidBodyCollection::incrChaseDistance(float fDiff)
{
    JSONRigidBody* pBody = focusedRigidBody();
    if( pBody )
        pBody->incrChaseDistance(fDiff);
}

void JSONRigidBodyCollection::incrChaseHeight(float fDiff)
{
    JSONRigidBody* pBody = focusedRigidBody();
    if( pBody )
        pBody->incrChaseHeight(fDiff);
}

int JSONRigidBodyCollection::curViewIdx() const
{
	const JSONRigidBody* body = activeRigidBody();
	if( body == 0) return -1;
	return body->curViewIdx();
}

void JSONRigidBodyCollection::updateCameraView()
{
    JSONRigidBody* pBody = focusedRigidBody();
    if( pBody )
        pBody->updateCameraView();
}

CameraView* JSONRigidBodyCollection::getCameraView()
{
    JSONRigidBody* pBody = focusedRigidBody();
    if( pBody )
        return pBody->getCameraView();
    return 0;
}

std::string JSONRigidBodyCollection::getCameraDescription() const
{
    const JSONRigidBody* pBody = focusedRigidBody();
    if( pBody )
        return pBody->getCameraDescription();
    else
        return "Camera:NoBody";
}

void JSONRigidBodyCollection::persistReadState(FILE* fPersistFile)
{
    for (JSONRigidBody* it : _bodyList)
	{
		JSONRigidBody::STATE state;
		fread(&state, sizeof(state), 1, fPersistFile);
		it->setState(state);

		GPSLocation location;
		fread(&location, sizeof(location), 1, fPersistFile);
		it->setPosition(location);

		Vector3D euler = it->getEuler();
		fread(&euler, sizeof(euler), 1, fPersistFile);
		it->setEuler(euler.x, euler.y, euler.z);

        it->persistReadState(fPersistFile);
	}
}

void JSONRigidBodyCollection::persistWriteState(FILE* fPersistFile)
{
    for (JSONRigidBody* it : _bodyList)
	{
		JSONRigidBody::STATE state = it->getState();
		fwrite(&state, sizeof(state), 1, fPersistFile);

		GPSLocation location = it->getGPSLocation();
		fwrite(&location, sizeof(location), 1, fPersistFile);

		Vector3D euler = it->getEuler();
		fwrite(&euler, sizeof(euler), 1, fPersistFile);

        it->persistWriteState(fPersistFile);
    }
}

void JSONRigidBodyCollection::persistReadState(rapidjson::Document &doc)
{
    for (JSONRigidBody* it : _bodyList)
        it->persistReadState(doc);
}

void JSONRigidBodyCollection::persistWriteState(rapidjson::Document &doc)
{
    for (JSONRigidBody* it : _bodyList)
        it->persistWriteState(doc);
}

void JSONRigidBodyCollection::loadBodyRecorderedData()
{
    for (JSONRigidBody* it : _bodyList)
	{
		char filename[256] = {};
		sprintf(filename, "flightrecorded_%s.bin", it->getID().c_str());
		it->loadRecorder(filename);
	}
}

const JSONRigidBody* JSONRigidBodyCollection::activeRigidBody() const
{
	if(_focusRigidBody == _bodyList.end() )
			 return 0;

	 if( *_focusRigidBody == 0)
		 return 0;

	 if( !(*_focusRigidBody)->getShowState() )
		 return 0;

	 return *_focusRigidBody;
}

JSONRigidBody* JSONRigidBodyCollection::activeRigidBody()
{
	 return const_cast<JSONRigidBody*>(static_cast<const JSONRigidBodyCollection*>(this)->activeRigidBody());
}

JSONRigidBody *JSONRigidBodyCollection::focusedRigidBody()
{
	if (_focusRigidBody == _bodyList.end())
		return nullptr;

    return *_focusRigidBody;
}

const JSONRigidBody *JSONRigidBodyCollection::focusedRigidBody() const
{
    return *_focusRigidBody;
}

void JSONRigidBodyCollection::nextFocusedRigidBody()
{
	if (_focusRigidBody == _bodyList.end())
	{
		_focusRigidBody = _bodyList.begin();
	}
	else
	{
		++_focusRigidBody;

		if (_focusRigidBody == _bodyList.end())
			_focusRigidBody = _bodyList.begin();
	}
}

void JSONRigidBodyCollection::prevFocusedRigidBody()
{
	if (_focusRigidBody == _bodyList.begin())
		_focusRigidBody = _bodyList.end();
	
	--_focusRigidBody;
}

