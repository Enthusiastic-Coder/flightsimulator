#pragma once

class SimpleSpringModel
{
public:
	SimpleSpringModel();

	void setMass(double mass);
	void setMu(double mu);
	void setEquilibrium(double eq);
	void setMax(double max);

	void calculate();
	bool isOk();

	double maxLen() { return _maxLen; }
	double K() { return _k; }
	double MU() { return _mu; }

private:
	double _k;
	double _mass;
	double _mu;
	double _eqLen;
	double _maxLen;
	bool _bOk;
};