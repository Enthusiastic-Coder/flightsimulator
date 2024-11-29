#pragma once

#include <vector>
#include <string>

class AoaClData
{
public:
	bool load( std::string filename);

	void ClCdCm( double aoa, double &cl, double &cd, double &cm );
	void output();

private:
	struct DATA
	{
		DATA()
		{
			cl = cd = cm = 0.0;
		}

		double cl;
		double cd;
		double cm;
	};

	std::vector<DATA> _aoa;

protected:
	void reset();
	void blend(double DATA::*cx);

};
