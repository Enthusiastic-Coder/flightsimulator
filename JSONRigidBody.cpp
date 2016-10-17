#include "StdAfx.h"
#include "JSONRigidBody.h"
#include "GSRigidBody.h"
#include "WorldSystem.h"
#include <MassChannel.h>
#include "RigidBodyMeshManager.h"
#include "OpenGLRenderer.h"
#include "OpenGLShaderProgram.h"

JSONRigidBody::JSONRigidBody(std::string sName, JSONRigidBody::Type typeFlags) :	
    _pMeshModel(0),
	_state(STATE::NORMAL), 
    _bShow(true),
    _bDrivingByMouse(false),
    _typeFlags(typeFlags),
    _cameraPositionProvider(ViewPosition::ViewPositionEnd-1),
    _chaseAngle(0.0f),
    _chaseHeight(0.0f),
    _chaseDistance(30.0f)
{
    std::string::size_type pos = sName.find(':');
    if (pos != std::string::npos)
    {
        _sName = sName.substr(0, pos);
        _sID = sName.substr(pos + 1);
    }
    else
    {
        _sName = sName;
        _sID = sName;
    }
}

MeshModel* JSONRigidBody::getMeshModel()
{
	return _pMeshModel;
}

const MassChannel & JSONRigidBody::getMassChannel() const
{
	return _massChannel;
}

void JSONRigidBody::setState(STATE state)
{
	_state = state;
	
	if( _state == STATE::RECORDING )
		_flightRecorder.reset();

	if( _state == STATE::PLAYBACK)
		_flightRecorder.moveFirst();
}

JSONRigidBody::STATE JSONRigidBody::getState()
{
	return _state;
}

void JSONRigidBody::setShowState(bool bShow)
{
	_bShow = bShow;
}
	
bool JSONRigidBody::getShowState() const
{
	return _bShow;
}

void JSONRigidBody::onPlayRecording(double dt)
{
	_flightRecorder.moveNext( dt );
	setEuler(_euler.x,_euler.y,_euler.z);
}

MeshModel* JSONRigidBody::loadMeshModel(std::string sMeshName, MassChannel &mc)
{
	MeshModel* model = RigidBodyMeshManager::get().getModel(sMeshName, mc);

	if (model == 0)
		return RigidBodyMeshManager::get().loadModel(sMeshName, mc);
	else
		return model;
}

void JSONRigidBody::onInitialise(WorldSystem* pWorldSystem)
{
	_mesh_pivot_vector.clear();

	_massChannel.setMass(getMass());
	if( _pMeshModel = loadMeshModel( getName(), _massChannel) )
	{
		MeshModel& meshModel = *_pMeshModel;
		_mesh_pivot_vector.resize( meshModel.size() );

		for( size_t i =0; i < _mesh_pivot_vector.size(); ++i )
			_mesh_pivot_vector[i] = _pivots[ meshModel[i]->getName() ];

		setInertiaMatrix(_massChannel.MOI().toDouble() );
		setCG(_massChannel.CG().toDouble());

		GPSLocation loc = getGPSLocation();
		float fHeight = loc._height;
		loc._height = 0.0f;

		HeightData hd;
        pWorldSystem->getHeightFromPosition(loc, hd);

		loc._height = fHeight - hd.Height();
		setPosition(loc);
	}
	else
		setInertiaMatrix( Matrix3x3D(	getMass(),	0,			0, 
										0,			getMass(),	0,
										0,			0,			getMass() ) );

	for( auto& it : _setList )
        it.second->onInitialise(pWorldSystem);

	for( auto& it : _endList)
        it->onInitialise(pWorldSystem);

	_flightRecorder.hookEuler( _euler );
	setRecorderHook( _flightRecorder );
}

void JSONRigidBody::setRecorderHook(FlightRecorder& a)
{
	GSRigidBody::setRecorderHook(a);

	for( auto& it : _setList )
		it.second->setRecorderHook(a);
}

void JSONRigidBody::clear()
{
	_setList.clear();
	buildMapList();
}

void JSONRigidBody::reset()
{
	_totalForceCache.Reset();
	_totalTorqueCache.Reset();

	for( auto& it : _endList )
		it->reset();

	for( auto& it : _setList )
		it.second->reset();
}

void JSONRigidBody::addForceGenerator(std::string sName, GSForceGenerator* pGenerator, int timeresolution)
{
	_force_generator_map[sName] = pGenerator;

	if( timeresolution )
		_setList.insert( std::pair<int,GSForceGenerator*>(timeresolution,pGenerator) );
	else
		_endList.push_back(pGenerator);

	buildMapList();
}

void JSONRigidBody::onUpdateFinal(double dt)
{
	for(auto& it : _endList )
        it->onApplyForce( this, dt );

	update( dt );
	_hydraulics.update(dt);
}

void JSONRigidBody::onUpdateHelper(std::map<int, std::vector<GSForceGenerator*>>::iterator &it, double dt )
{
	if( it == _cookedMapList.end() )
	{
		onUpdateFinal(dt);
		return;
	}

	std::map<int, std::vector<GSForceGenerator*>>::iterator currentit = it;
    int divides = std::max(1.0f,ceilf(it->first * dt));
	dt /= divides;

	for( int i=0; i < divides; i++ )
	{
		push();

		for( auto& it2 : currentit->second )
			it2->onApplyForce( this, dt );
	
		++(it = currentit);

		if( it == _cookedMapList.end() )
		{
			onUpdateFinal(dt);
			
			if( i == divides-1 )
			{
				_totalForceCache = getForce();
				_totalTorqueCache = getTorque();
			}
		}
		else
			onUpdateHelper(it, dt);

		pop();
	}
}

void JSONRigidBody::onUpdate(double dt)
{
	// Iterate all force constraints to see which are touching the ground

	// Calculate full Torque magnitude require to bring rotation to halt.
	// Obtain rotation Mass = Torque.Magnitude() * dt / ( r ^ (w ^ r ) ).Magnitude() 

	// Go through all contacts for this body and adding to Mass Distributor
	// if contact made

	// For each contact compare Rotational Mass with distribute Mass 
	// _restingMass = MAX( Mr, Md )
	// Set float ConstraintForceGenerator::_restingMass
	
    setTorque(Vector3D());
    setForce(Vector3D());
	onUpdateHelper( _cookedMapList.begin(), dt );
	setForce( _totalForceCache );
	setTorque( _totalTorqueCache );

	if( _state == STATE::RECORDING )
	{
		_euler = getEuler();
		_flightRecorder.record( dt );
	}
}

void JSONRigidBody::onAfterUpdate(double dt)
{
	onForceGeneratorAction( &GSForceGenerator::onAfterUpdate, dt );
}

void JSONRigidBody::onBeforeUpdate(double dt)
{
	_lastGPSPosition = getGPSLocation();
	onForceGeneratorAction( &GSForceGenerator::onBeforeUpdate, dt );
}

void JSONRigidBody::onForceGeneratorAction( void (GSForceGenerator::*pActor)(Particle*,double), double dt )
{
	for( auto& it : _setList )
		(it.second->*pActor)( this, dt );
}

void JSONRigidBody::onForceGeneratorCheckActive()
{
	bool bBuildRequired = false;

	for( auto& it : _setList )
	{
		bool bIsActive = it.second->onCheckActive(this);

		if( it.second->isActive(this) != bIsActive )
		{
			bBuildRequired = true;
			it.second->setIsActive(bIsActive);
		}
	}

	if( bBuildRequired )
		buildMapList();
}

JSONRigidBody::Type JSONRigidBody::typeFlags() const
{
	return _typeFlags;
}

void JSONRigidBody::buildMapList()
{
	_cookedMapList.clear();

	for( auto& it : _setList )
	{
		if( it.second->isActive(this) )
			_cookedMapList[it.first].push_back(it.second);
	}
}

void JSONRigidBody::push()
{
	// Push Force, Torque;
	setWorldFrame();
	_stkForceTorque.push( std::make_pair(getForce(), getTorque() ) );
}

void JSONRigidBody::pop()
{
	// Pop Force, Torque;
    std::pair<Vector3D,Vector3D>& forceTorquePair = _stkForceTorque.top();
	setWorldFrame();
	setForce( forceTorquePair.first );
	setTorque( forceTorquePair.second );
	_stkForceTorque.pop();
}

void JSONRigidBody::Render(Renderer *r, bool bReflection, unsigned int shadowMapCount)
{
	if (getMeshModel() == 0)
	{
        OpenGLShaderProgram& progID = r->progId();
        r->useProgram(Renderer::nullShader);
        OpenGLPipeline& mat = OpenGLPipeline::Get(r->camID);
		mat.Push();
        mat.GetModel().TranslateLocation(getGPSLocation());
		if (bReflection)
			mat.GetModel().Scale(1, -1, 1);
        applyModelMatrix(mat.GetModel());
        mat.bindLegacyMatrices();
        renderMesh(r, shadowMapCount);//Imediate mode
		mat.Pop();
        r->useProgram(progID);
        return;
	}

    OpenGLPipeline& mat = OpenGLPipeline::Get(r->camID);
	mat.Push();
    mat.GetModel().TranslateLocation(getGPSLocation());
	if(bReflection)
		mat.GetModel().Scale(1, -1, 1);
	applyModelMatrix(mat.GetModel());
    mat.bindMatrices(r->progId());

	if( getMeshModel()->InFrustum( mat.GetFrustum() ) )
	{
        for (unsigned int shadowID = r->camID + 1; shadowID <= r->camID + shadowMapCount; ++shadowID)
		{
			OpenGLPipeline::Get(shadowID).Push();
            OpenGLPipeline::Get(shadowID).GetModel().TranslateLocation(getGPSLocation());
			applyModelMatrix(OpenGLPipeline::Get(shadowID).GetModel());
            OpenGLPipeline::Get(shadowID).ApplyLightBPMV(r->progId());
		}
		
		if (global_fg_debug)
            glEnable(GL_BLEND);

        renderMesh(r, shadowMapCount);
		
		if (global_fg_debug)
            glDisable(GL_BLEND);

        for (unsigned int shadowID = r->camID + 1; shadowID <= r->camID + shadowMapCount; ++shadowID)
		{
			OpenGLPipeline::Get(shadowID).Pop();
            OpenGLPipeline::Get(shadowID).ApplyLightBPMV(r->progId());
		}
	}
	
	mat.Pop();
}

void JSONRigidBody::renderForceGenerators(Renderer* r)
{
	if (getMeshModel() == 0)
		return;

    OpenGLPipeline& mat = OpenGLPipeline::Get(r->camID);
	mat.Push();
    mat.GetModel().TranslateLocation(getGPSLocation());
	applyModelMatrix(mat.GetModel());
    mat.bindMatrices(r->progId());

	if (getMeshModel()->InFrustum(mat.GetFrustum()))
	{
        //glDisable(GL_TEXTURE_2D);

		if (global_force_lines_debug == __global_force_lines_debug::force_lines_aero_force_and_wind_tunnel
			|| global_force_lines_debug == __global_force_lines_debug::force_lines_aero_wind_tunnel)
            drawWindTunnel(r->dt);

		if (global_force_lines_debug == __global_force_lines_debug::force_lines_aero_force ||
			global_force_lines_debug == __global_force_lines_debug::force_lines_aero_force_and_wind_tunnel)
		{
			//OpenGLPipeline::Get(0).Apply();

			glDisable(GL_DEPTH_TEST);

            for (const std::pair<int,GSForceGenerator*>& it : _setList)
                it.second->drawForceGenerator(this, r);

			glEnable(GL_DEPTH_TEST);
		}
	}

	mat.Pop();
}

void JSONRigidBody::renderMesh(Renderer* r, unsigned int shadowMapCount)
{
    MeshModel* pModel = getMeshModel();
    if(pModel == 0)
		return;

    OpenGLPipeline& mat = OpenGLPipeline::Get(r->camID);

	for( size_t i = 0; i < _mesh_pivot_vector.size(); ++i )
	{
        GSForceGenerator::PivotObject* pPO = _mesh_pivot_vector[i];
		
		if( pPO )
		{
			mat.Push();
            pPO->applyTransform(r->camID);

            for (unsigned int shadowID = r->camID + 1; shadowID <= r->camID + shadowMapCount; ++shadowID)
			{
                OpenGLPipeline::Get(shadowID).Push();
				pPO->applyTransform(shadowID);
                OpenGLPipeline::Get(shadowID).ApplyLightBPMV(r->progId());
			}

            mat.bindMatrices(r->progId());
		}

        pModel->RenderIndex(r, i);

		if (pPO)
		{
            for (unsigned int shadowID = r->camID + 1; shadowID <= r->camID + shadowMapCount; ++shadowID)
			{
				OpenGLPipeline::Get(shadowID).Pop();
                OpenGLPipeline::Get(shadowID).ApplyLightBPMV(r->progId());
			}

			mat.Pop();
            mat.bindMatrices(r->progId());
		}
	}
}

void JSONRigidBody::applyModelMatrix(OpenGLMatrixStack& mv)
{
	mv.Translate(cg());
	mv.Rotate(getEuler());
	mv.Translate(-cg());
}

void JSONRigidBody::saveRecorder(std::string sfilename)
{
	_flightRecorder.save(sfilename);
}

bool JSONRigidBody::loadRecorder(std::string sfilename)
{
	return _flightRecorder.load(sfilename);
}

bool JSONRigidBody::onSyncKeyPress()
{
    return false;
}

bool JSONRigidBody::onMouseMove( int x, int y)
{
	return false;
}

bool JSONRigidBody::onMouseWheel(int wheelDelta)
{
    return false;
}

bool JSONRigidBody::nextView()
{
    return _cameraPositionProvider.nextView();
}

bool JSONRigidBody::prevView()
{
    return _cameraPositionProvider.prevView();
}

int JSONRigidBody::curViewIdx() const
{
    return _cameraPositionProvider.curViewIdx();
}

void JSONRigidBody::incrChaseAngle(float fDiff)
{
    if( _cameraPositionProvider.curViewIdx() == Chase ||  _cameraPositionProvider.curViewIdx() == ChaseRotate)
        _chaseAngle += fDiff;
}

void JSONRigidBody::incrChaseDistance(float fDiff)
{
    if( _cameraPositionProvider.curViewIdx() == Chase || _cameraPositionProvider.curViewIdx() == ChaseRotate)
    {
        _chaseDistance += fDiff;

        if( _chaseDistance < 1 )
            _chaseDistance = 1;
    }
}

void JSONRigidBody::incrChaseHeight(float fDiff)
{
    if( _cameraPositionProvider.curViewIdx() == Chase || _cameraPositionProvider.curViewIdx() == ChaseRotate)
    _chaseHeight += fDiff;
}

void JSONRigidBody::updateCameraView()
{
    CameraView* view = getCameraProvider()->getCameraView();
    view->setShakingMode(false);
    view->setSmoothMode(false);
    Vector3F viewOr = view->getOrientation();

    switch((ViewPosition)getCameraProvider()->curViewIdx())
    {
    case CockpitForwards:
        view->setPosition(toNonLocalTranslateFrame(Vector3D(0, 0, 0)));
        view->setOrientation(getEuler().toFloat());
        view->setDescription("Cockpit");
        break;
    case PassengerLeftMiddle:
        view->setPosition(toNonLocalTranslateFrame(Vector3D(1.7, 4.0, 3.35)));
        view->setOrientation(getEuler().toFloat());
        view->setDescription("LeftSeat");
        break;
    case RightGearWheel:
        view->setPosition(toNonLocalTranslateFrame(Vector3D(0, 0, 10)));
        view->setOrientation(getEuler().toFloat());
        view->setDescription("RightGear");
        break;
    case Chase:
    {
        Vector3D eul = getEuler();
        Vector3D rot = QVRotate( MathSupport<double>::MakeQ(0, eul.y-_chaseAngle, 0), Vector3D(0, _chaseHeight, _chaseDistance));
        view->setPosition(getGPSLocation()+rot);
        view->setOrientation(Vector3F(viewOr.x,eul.y-_chaseAngle,viewOr.z));
        view->setDescription("Chase");
        //view->setSmoothMode(true);
        break;
    }
    case ChaseRotate:
        Vector3D rot = QVRotate( MathSupport<double>::MakeQ(0, -_chaseAngle, 0), Vector3D(0, _chaseHeight, _chaseDistance));
        view->setPosition(getGPSLocation()+rot);
        view->setOrientation(Vector3F(viewOr.x,-_chaseAngle,viewOr.z));
        view->setDescription("ChaseRotate");
        //view->setSmoothMode(true);
        break;
    }

    view->setDescription( getID() + ":" +  getCameraProvider()->getCameraDescription());
}

CameraView *JSONRigidBody::getCameraView()
{
    return _cameraPositionProvider.getCameraView();
}

std::string JSONRigidBody::getCameraDescription() const
{
    return _cameraPositionProvider.getCameraDescription();
}

bool JSONRigidBody::getHeightFromPosition(const GPSLocation &position, HeightData &heightData) const
{
    return false;
}

void JSONRigidBody::persistReadState(FILE* fPersistFile)
{
    fread( (void*)&position(), sizeof(position()), 1, fPersistFile );
	fread( (void*)&velocity(), sizeof(velocity()), 1, fPersistFile );

	fread((void*) &getOrientation(), sizeof(getOrientation()), 1, fPersistFile );
	fread( (void*)&angularVelocity(), sizeof(angularVelocity()), 1, fPersistFile );
}

void JSONRigidBody::persistWriteState(FILE* fPersistFile)
{
	fwrite( (void*)&position(), sizeof(position()), 1, fPersistFile );
	fwrite((void*) &velocity(), sizeof(velocity()), 1, fPersistFile );

	fwrite( (void*)&getOrientation(), sizeof(getOrientation()), 1, fPersistFile );
	fwrite((void*) &angularVelocity(), sizeof(angularVelocity()), 1, fPersistFile );
}

bool JSONRigidBody::typeMask(Type t)
{
	return ((int)typeFlags() & (int)t) != 0;
}

std::string JSONRigidBody::getID()
{
	if (_sID.length() == 0)
		return getName();
	else
		return _sID;
}

std::string JSONRigidBody::getName()
{
    return _sName;
}

std::string JSONRigidBody::getPath()
{
    std::string path = "Models/";
    path.append(getName());
    path.append("/");
    return path;
}

Hydraulics& JSONRigidBody::hydraulics()
{
	return _hydraulics;
}

PivotObjects& JSONRigidBody::pivots()
{
	return _pivots;
}

GSForceGenerator* JSONRigidBody::forceGenerator(std::string& sName)
{
	return _force_generator_map[sName];
}

std::map<std::string, GSForceGenerator*>& JSONRigidBody::forceGeneratorMap()
{
	return _force_generator_map;
}

bool JSONRigidBody::isUsingMouse()
{
    return _bDrivingByMouse;
}

void JSONRigidBody::setUsingMouse(bool value)
{
    _bDrivingByMouse = value;
}

float JSONRigidBody::getPower(int engine)
{
	return 0.0f;
}

float JSONRigidBody::getPowerOutput(int engine)
{
    return 0.0f;
}

void JSONRigidBody::setPower(float value)
{
    value;
}

GPSLocation JSONRigidBody::getLastPosition()
{
	return _lastGPSPosition;
}

CameraViewProvider* JSONRigidBody::getCameraProvider()
{
    return &_cameraPositionProvider;
}

FlightRecorder & JSONRigidBody::getFlightRecorder()
{
	return _flightRecorder;
}

void JSONRigidBody::drawWindTunnel(double dt)
{
	//T* t = static_cast<T*>(this);
    //t->drawWindTunnelLines(dt);
}

bool JSONRigidBody::onAsyncKeyPress(IScreenMouseInfo *scrn, float dt)
{
    return false;
}
