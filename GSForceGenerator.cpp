#include "stdafx.h"
#include "GSForceGenerator.h"
#include "WorldSystem.h"

#define ARROW_L		0.2f

GSForceGenerator::GSForceGenerator(WorldSystem* pWorldSystem, float dForceLineLength) :
	_pWorldSystem(pWorldSystem),
	_dForceLinesLength(dForceLineLength)
{}


void GSForceGenerator::drawForceGenerator(GSRigidBody* pBody, Renderer* r)
{
    const Vector3D& pos = getContactPoint();
	//VectorD appliedF = getAppliedForce() / _dForceLinesScale;
	//if( appliedF.Magnitude() > 10 )		appliedF.Normalize() *= 10.0;

    Vector3D appliedF = getAppliedForce() / pBody->getMass() * 24.0;
    if (appliedF.Magnitude() > 10)
        appliedF = appliedF.Unit() * 10.0;

	//VectorD appliedF = getAppliedForce() / p->getMass() / 10.0 * 10.0 ;

    float vertices[] = {
        pos.x, pos.y, pos.z,
        pos.x, pos.y + appliedF.y, pos.z,

        pos.x, pos.y + appliedF.y, pos.z,
        pos.x + appliedF.x, pos.y + appliedF.y, pos.z,

        pos.x, pos.y + appliedF.y, pos.z,
        pos.x, pos.y + appliedF.y, pos.z + appliedF.z,

        pos.x - ARROW_L, pos.y, pos.z,
        pos.x + ARROW_L, pos.y, pos.z,

        pos.x, pos.y, pos.z - ARROW_L,
        pos.x, pos.y, pos.z + ARROW_L,

        pos.x, pos.y - ARROW_L, pos.z,
        pos.x, pos.y + ARROW_L, pos.z,

        pos.x, pos.y + appliedF.y, pos.z,
        pos.x - ARROW_L, pos.y + appliedF.y - ARROW_L, pos.z,

        pos.x, pos.y + appliedF.y, pos.z,
        pos.x + ARROW_L, pos.y + appliedF.y - ARROW_L, pos.z,

        pos.x, pos.y + appliedF.y, pos.z,
        pos.x + ARROW_L, pos.y + appliedF.y, pos.z - ARROW_L,

        pos.x, pos.y + appliedF.y, pos.z,
        pos.x + ARROW_L, pos.y + appliedF.y, pos.z + ARROW_L,
    };

    if(appliedF.z < 0)
    {
        _dynCol[0] = 0;
        _dynCol[1] = 1;
        _dynCol[2] = 0;
        _dynCol[3] = 1;
    }
    else
    {
        _dynCol[0] = 1;
        _dynCol[1] = 0;
        _dynCol[2] = 0;
        _dynCol[3] = 1;
    }

    float colors[] = {
        1, 1, 1, 1,
        1, 1, 1, 1,

        1, 0, 1, 1,
        1, 0, 1, 1,

        _dynCol[0], _dynCol[1], _dynCol[2], _dynCol[3],
        _dynCol[0], _dynCol[1], _dynCol[2], _dynCol[3],

        0, 1, 1, 1,
        0, 1, 1, 1,

        0, 1, 1, 1,
        0, 1, 1, 1,

        0, 1, 1, 1,
        0, 1, 1, 1,

        1, 1, 0, 1,
        1, 1, 0, 1,

        1, 1, 0, 1,
        1, 1, 0, 1,

        1, 1, 0, 1,
        1, 1, 0, 1,

        1, 1, 0, 1,
        1, 1, 0, 1,
    };

    r->bindVertex(Renderer::Vertex, 3, vertices);
    r->bindVertex(Renderer::Color, 4, colors);
    r->setPrimitiveType( GL_LINES);
    r->setUseIndex(false);
    r->setVertexCountOffset(indicesCount(vertices,3));

    r->Render();
    r->unBindBuffers();
}

void GSForceGenerator::onApplyForce(Particle *p, double dt)
{
    p;
    dt;

}

void GSForceGenerator::onInitialise(WorldSystem* pWorldSystem)
{
	_pWorldSystem = pWorldSystem;
}

void GSForceGenerator::setRecorderHook(FlightRecorder& a)
{
	a.addDataRef(_contactPoint);
	a.addDataRef(_appliedForce);
}

bool GSForceGenerator::onCheckActive(Particle *p)
{
	return isActive(p);
}

ISceneryEnvironment * GSForceGenerator::getEnv()
{
	return _pWorldSystem->getEnv();
}

IWeather * GSForceGenerator::getWeather()
{
	return _pWorldSystem->getWeather();
}

IJoystick * GSForceGenerator::getJoystick()
{
	return _pWorldSystem->getJoystick();
}

WorldSystem* GSForceGenerator::getWorld()
{
	return _pWorldSystem;
}

void GSForceGenerator::addPivotObject(PivotObject* po)
{
    _pivotObjects.push_back(po);
}

GSForceGenerator::PivotObject::PivotObject() :
	_pParent(nullptr)
{
}

GSForceGenerator::PivotObject::PivotObject(PivotObject* pParent)
	: _pParent(pParent)
{
}

GSForceGenerator::PivotObject* GSForceGenerator::PivotObject::getParent()
{
	return _pParent;
}

void GSForceGenerator::PivotObject::setPivot(Vector3F t, Vector3F r)
{
	_TRANSLATE = t;
	_ROTATION = r;
}

const Vector3F& GSForceGenerator::PivotObject::getT() const
{
	return _TRANSLATE;
}

const Vector3F& GSForceGenerator::PivotObject::getR() const
{
	return _ROTATION;
}

GSForceGenerator::PivotObject* GSForceGenerator::getPivotObject(size_t idx)
{
	if (_pivotObjects.size() == 0)
		return nullptr;

	if (idx >= _pivotObjects.size())
		return nullptr;

    return _pivotObjects[idx];
}

size_t GSForceGenerator::pivotObjectCount()
{
	return _pivotObjects.size();
}

void GSForceGenerator::setLengthForceLines(float scale)
{
	_dForceLinesLength = scale;
}

