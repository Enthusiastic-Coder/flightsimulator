#pragma once

#include "JSONRigidBodyCollection.h"
#include "AeroControlSurface.h"
#include "AeroSectionSubElementForceGenerator.h"
#include "AeroSectionElementForceGenerator.h"
#include "AeroSectionForceGenerator.h"
#include "AeroWheelSpringForceGenerator.h"
#include "AeroForceGenerator.h"
#include "AircraftEngineForceGenerator.h"
#include <fstream>

class JSONRigidBodyBuilder
{
	struct HotMoveableList
	{
		AeroControlSurface *movable;
		Vector3F sortboundary[4];
		bool bStart;
	};

	static bool HotMoveableListSort(const HotMoveableList& m1, const HotMoveableList& m2)
	{
		return	(m1.bStart ? m1.sortboundary[0].x : m1.sortboundary[1].x) <
				(m2.bStart ? m2.sortboundary[0].x : m2.sortboundary[1].x );
	}

public:
	JSONRigidBodyBuilder(JSONRigidBody *pJSONBody) :
		_pJsonBody(pJSONBody),
		_pAero(nullptr),
		_pSection(nullptr),
		_pAeroWheelSpring(nullptr),
		_pAircraftEngine(nullptr),
		_line_count(0),
		_pivotIndex(0)
	{ }

	void build( const std::string & sfilename );

protected:

	bool load( const std::string & sfilename );
	bool parseQuad(Vector3F boundary[4]);
	bool parseVector(Vector4F &v);
	bool parseVector( Vector3F &v );
	bool parseUV( float& u, float& v );
	bool parseNumber( float &f );
	bool parseNumber( int &i );
	bool parseNameValueArray(char sep, std::vector<std::pair<std::string,std::string>>& nameValueArr );
	int parseArguments( std::vector<std::string> &args );
	std::string parseLine();
	std::vector<std::string> parseLineUsingSeparator( char sep );

	void addSectionSubElements(AeroSectionElementForceGenerator *e, float cpU, float cpV);
	void addSubElementMoveableRefs( AeroSectionElementForceGenerator *e, float cpU, float cpV, float sbU[4], float sbV[4], std::set<AeroControlSurface*>& moveableSet );

private:
	void trim2(std::string& str);
	void throwError( std::string title, std::string description );
	
private:
	size_t _line_count;
	std::ifstream _input_file;
	std::string _input_filename;

	JSONRigidBody *_pJsonBody;
	AircraftEngineForceGenerator *_pAircraftEngine;
	AeroWheelSpringForceGenerator  *_pAeroWheelSpring;
	AeroForceGenerator * _pAero;
	AeroSectionForceGenerator * _pSection;
	unsigned int _pivotIndex;
	std::string _errorMsgs;
};


