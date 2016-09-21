#include "stdafx.h"
#include "SimpleSpringModel.h"

SimpleSpringModel::SimpleSpringModel()
	: _k(0.0f),
	_mass(0.0f),
	_mu(0.0f),
	_eqLen(0.0),
	_maxLen(0.0f),
	_bOk(false)
{
}

void SimpleSpringModel::setMass(double mass)
{
	_mass = mass;
}

void SimpleSpringModel::setMu(double mu)
{
	_mu = mu;
}

void SimpleSpringModel::setEquilibrium(double eqLen)
{
	_eqLen = eqLen;
}

void SimpleSpringModel::setMax(double maxLen)
{
	_maxLen = maxLen;
}

void SimpleSpringModel::calculate()
{
	_k = _mass * 9.08665 / _eqLen;
	_bOk = true;
}

bool SimpleSpringModel::isOk()
{
	return _bOk;
}