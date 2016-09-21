#include "StdAfx.h"
#include <algorithm>
#include "JSONRigidBodyBuilder.h"
#include <iostream>

void JSONRigidBodyBuilder::trim2(std::string& str)
{
	std::string::size_type pos = str.find_last_not_of(' ');
	if(pos != std::string::npos) 
	{
		str.erase(pos + 1);
		pos = str.find_first_not_of(' ');
		if(pos != std::string::npos) 
			str.erase(0, pos);
	}
	else 
		str.erase(str.begin(), str.end());
}

void JSONRigidBodyBuilder::throwError( std::string title, std::string description )
{
    char buf[128]={};
    sprintf( buf, "%s : Line no [%d], Title [%s] > [%s]\n", _input_filename.c_str(), _line_count, title.c_str(), description.c_str() );
	_errorMsgs += buf;
}

void JSONRigidBodyBuilder::build( const std::string & sfilename )
{
	_errorMsgs.clear();

	if (!load(sfilename))
	{
		throwError("Failed to load", sfilename);
		return;
	}

	std::string sline;
	ForceGeneratorType object_type = ForceGeneratorType::Type_None;
	float element_ratio = 0.0f;

	while( _input_file.good() )
	{
		sline = parseLine();

		if (sline.length() == 0)
			continue;

		if (object_type >= ForceGeneratorType::Type_Aero && _pAero == NULL)
		{
			throwError(sline, "ElementType unexpected for aero");
			continue;
		}

		if( sline == "endAeroFG" )
		{
			if (object_type != ForceGeneratorType::Type_Aero)
			{
				throwError(sline, "Element Type Unexpected");
				continue;
			}

			object_type = ForceGeneratorType::Type_None;
			_pSection = nullptr;
			_pAero = nullptr;
		}
		else if (sline == "endAeroWheelSpringFG")
		{
			if (object_type != ForceGeneratorType::Type_AeroWheelSpringNormal)
			{
				throwError(sline, "Element Type Unexpected");
				continue;
			}

			_pAeroWheelSpring = nullptr;
			object_type = ForceGeneratorType::Type_None;
		}
		else if( sline == "endEngineFG" )
		{
			if (object_type != ForceGeneratorType::Type_AircraftEngine)
			{
				throwError(sline, "Element Type Unexpected");
				continue;
			}

			_pAircraftEngine = nullptr;
			object_type = ForceGeneratorType::Type_None;
		}
		else if (sline == "beginEngineFG")
		{
			if (object_type != ForceGeneratorType::Type_None)
			{
				throwError(sline, "Element Type Unexpected");
				continue;
			}

			GSForceGenerator *pFG = _pJsonBody->forceGenerator(parseLine());
			if (!pFG || !FG_TYPE_IS_AIRCRAFTENGINE(pFG->Type()))
			{
				throwError(sline, "Expected Aircraft Engine Force Generator");
				continue;
			}

			float max_thrust(0.0f);
			if (!parseNumber(max_thrust))
			{
				throwError(sline, "Expected max engine thrust in Newtons");
				continue;
			}

			if (max_thrust < FLT_EPSILON)
			{
				throwError(sline, "max engine thrust is not positive.");
				continue;
			}

			_pAircraftEngine = static_cast<AircraftEngineForceGenerator*>(pFG);
			object_type = ForceGeneratorType::Type_AircraftEngine;
			_pivotIndex = 0;
			_pAircraftEngine->setMaxThrust(max_thrust);
		}
		else if( sline == "beginAeroWheelSpringFG" )
		{
			if (object_type != ForceGeneratorType::Type_None)
			{
				throwError(sline, "Element Type Unexpected");
				continue;
			}

			GSForceGenerator *pFG = _pJsonBody->forceGenerator(parseLine());
			if (!pFG || !FG_TYPE_IS_AEROWHEELSPRINGNORMAL(pFG->Type()))
			{
				throwError(sline, "Expected Wheel Spring Force Generator");
				continue;
			}

			_pAeroWheelSpring = static_cast<AeroWheelSpringForceGenerator*>(pFG);
			object_type = ForceGeneratorType::Type_AeroWheelSpringNormal;
			_pivotIndex = 0;
		}
		else if( sline == "beginAeroFG" )
		{
			if (object_type != ForceGeneratorType::Type_None)
			{
				throwError(sline, "Element Type Unexpected");
				continue;
			}

			object_type = ForceGeneratorType::Type_Aero;
			std::vector<std::string> params = parseLineUsingSeparator(':');

			if (params.size() != 2)
			{
				throwError(sline, "Expected 2 arguments");
				continue;
			}

			GSForceGenerator *pFG = _pJsonBody->forceGenerator(params[0]);

			if (!pFG || !FG_TYPE_IS_AERO(pFG->Type()))
			{
				throwError(sline, "Expected Aerodynamic Force Generator");
				continue;
			}
			
			_pAero = static_cast<AeroForceGenerator*>(pFG);			

			if (!_pJsonBody)
			{
				throwError(sline, "RigidBody system is null");
				continue;
			}

			//_pJsonBody->add( params[0], _pAero, atol(params[1].c_str() ) );

			if (_pAero == nullptr)
			{
				throwError(sline, "Section is null");
				continue;
			}
		}
		else if( sline == "beginSection" )
		{
			if (object_type != ForceGeneratorType::Type_Aero)
			{
				throwError(sline, "Unexpected object type");
				continue;
			}

			object_type = ForceGeneratorType::Type_AeroSection;
			_pSection = _pAero->add( "section" );
		}
		else if( sline == "endSection" )
		{
			if (object_type != ForceGeneratorType::Type_AeroSection)
			{
				throwError(sline, "Unexpected object type");
				continue;
			}

			object_type = ForceGeneratorType::Type_Aero;
			_pSection = NULL;
		}
		else if( sline == "addPivot" )
		{
			std::string pivotName( parseLine() );

			if (object_type == ForceGeneratorType::Type_Aero || object_type == ForceGeneratorType::Type_AeroSection)
			{
				Vector3F rotate;
				if (!parseVector(rotate))
				{
					throwError(sline, "Failed to parse Rotate vector");
					continue;
				}

				Vector3F translate;
				if (!parseVector(translate))
				{
					throwError(sline, "Failed to parse Translate vector");
					continue;
				}

				float fDeflectionFactor(1.0f);
				if (!parseNumber(fDeflectionFactor))
				{
					throwError(sline, "Failed to parse Deflection Factor");
					continue;
				}

				std::vector<std::pair<std::string,std::string>> nameValueList;
				if (!parseNameValueArray(':', nameValueList))
				{
					throwError(sline, "Failed to parse Name Value List");
					continue;
				}

				AeroControlSurface *pcs(nullptr);
				AeroSectionForceGenerator::AeroPivotObject *po(nullptr);

				if( object_type == ForceGeneratorType::Type_Aero  )
				{
					_pAero->setControlSurface0(pcs = new AeroControlSurface(Vector3F(1,0,0)));
					_pAero->addPivotObject( po = new AeroSectionForceGenerator::AeroPivotObject(pcs) );
				}
				else
				{
					pcs = _pSection->controlSurfaceN( _pSection->controlSurfaceCount()-1 );
					_pSection->addPivotObject( po = new AeroSectionForceGenerator::AeroPivotObject(pcs, _pivotIndex++, _pSection->parent()->getPivotObject(0)) );
				}

				if (!_pJsonBody->pivots().add(pivotName, po))
				{
					throwError(sline, "Pivot failed to be added");
					continue;
				}
				
				po->setPivot( translate, rotate, nameValueList, fDeflectionFactor );
			}
			else if( object_type == ForceGeneratorType::Type_AeroWheelSpringNormal )
			{
				Vector4F translate;
				if (!parseVector(translate))
				{
					throwError(sline, "Failed to parse Translate Rotation svector");
					continue;
				}

				Vector3F steering;
				if (!parseVector(steering))
				{
					throwError(sline, "Failed to parse Translate Steering svector");
					continue;
				}

				bool bSuspensionOnly = Vector4F::Null() == translate;
				int idx = int(translate.w);
				bool bNoRotation = idx == 0;
					
				auto* pPO = new WheelSpringForceGenerator::WheelSpringPivotObject(_pAeroWheelSpring->spring(), bSuspensionOnly, bNoRotation);
				pPO->setPivot( Vector3F(translate.x, translate.y, translate.z), steering );

				_pAeroWheelSpring->addPivotObject( pPO );

				if (!_pJsonBody->pivots().add(pivotName, pPO))
				{
					throwError(sline, "Pivot failed to be added");
					continue;
				}

				_pivotIndex++;
			}
			else if( object_type == ForceGeneratorType::Type_AircraftEngine )
			{
				if (_pivotIndex > 0)
				{
					throwError(sline, "Only 1 pivot per aircraft engine");
					continue;
				}

				auto * po = new AircraftEngineForceGenerator::AircraftEnginePivotObject(_pAircraftEngine);
				_pAircraftEngine->addPivotObject( po );

				if (!_pJsonBody->pivots().add(pivotName, po))
				{
					throwError(sline, "Pivot failed to be added");
					continue;
				}

				Vector3F rotate;
				if (!parseVector(rotate))
				{
					throwError(sline, "Failed to parse Rotate vector");
					continue;
				}

				Vector3F translate;
				if (!parseVector(translate))
				{
					throwError(sline, "Failed to parse Translate vector");
					continue;
				}

				po->setPivot( translate, rotate );
			}
			else
			{
				throwError(sline, "addPivot not in valid mode.");
				continue;
			}
		}
		else if( sline == "addControlSurface" )
		{
			if (object_type != ForceGeneratorType::Type_AeroSection)
			{
				throwError(sline, "only works in section mode.");
				continue;
			}

			Vector3F boundary[4];

			if (!parseQuad(boundary))
			{
				throwError(sline, "Failed to parse Quad");
				continue;
			}

			Vector3F vRotAxis( boundary[1] - boundary[0] );

			float distance, deflection;
			if (!parseUV(distance, deflection))
			{
				throwError(sline, "Failed to parse Adjuster Distance per Deflection");
				continue;
			}

			float d_d = 0.0f;
			if( deflection )
				d_d = distance / deflection;
			
			auto* pCS = new AeroControlSurface(vRotAxis, d_d);
			pCS->setBoundaryT(boundary);

			_pSection->addControlSurface(pCS);

			_pivotIndex = 0;
		}
		else if ( sline == "setSection" )
		{
			if (object_type != ForceGeneratorType::Type_AeroSection)
			{
				throwError(sline, "must be in section mode.");
				continue;
			}

			element_ratio = 0.0f;

			Vector3F boundary[4];
			if (!parseQuad(boundary))
			{
				throwError(sline, "Failed to parse boundary vector");
				continue;
			}

			_pSection->setBoundaryT( boundary );

			int slices(0);
			if (!parseNumber(slices))
			{
				throwError(sline, "Failed to parse slices Number");
				continue;
			}

			if (slices < 0.0f)
			{
				throwError(sline, "slices Number must be between 0.0 to 1.0");
				continue;
			}

			float fraction = 1.0f/slices;

			float u[4]={}, v[4]={};

			float cpU(0.0f), cpV(0.0f);

			if (!parseUV(cpU, cpV))
			{
				throwError(sline, "Failed to parse CP UV vector");
				continue;
			}

			for( int i=0; i < slices; i++ )
			{
				AeroSectionElementForceGenerator* pSectionElement = _pSection->add( "addElement");		

				u[0] = element_ratio + 0.001f;
				v[0] = 0.01f;

				u[1] = element_ratio + fraction - 0.001f;
				v[1] = v[0];

				u[2] = u[1];
				v[2] = 0.99f;

				u[3] = u[0];
				v[3] = v[2];

				pSectionElement->setBoundaryFromParentT( u, v );
				addSectionSubElements( pSectionElement, cpU, cpV );
				element_ratio += fraction;
			}
		}
		else
		{
			throwError( sline, "Unexpected keyword" );
			continue;
		}
	}

	for( auto& it : _pJsonBody->forceGeneratorMap() )
	{
		GSForceGenerator *pFG = it.second;

		if( pFG && FG_TYPE_IS_AERO(pFG->Type() ))
			static_cast<AeroForceGenerator*>(pFG)->calcAspectRatio();
	}

	if (_errorMsgs.length() > 0)
        std::cout << _errorMsgs << " : JSON config parse failed";
}

bool JSONRigidBodyBuilder::load( const std::string & sfilename )
{
	_input_filename = sfilename;
	_input_file.open( sfilename.c_str());
	return _input_file.is_open();
}

bool JSONRigidBodyBuilder::parseQuad(Vector3F boundary[4])
{
	for( int i=0; i < 4; i++ )
	{
		if( !parseVector(boundary[i]) )
			return false;
	}
	return true;
}

bool JSONRigidBodyBuilder::parseVector( Vector3F &v )
{
	std::vector<std::string> args;
	if( parseArguments(args ) != 3 )
		return false;

	v = Vector3F(std::atof(args[0].c_str()), std::atof(args[1].c_str()), std::atof(args[2].c_str()));
	return true;
}

bool JSONRigidBodyBuilder::parseVector(Vector4F &v)
{
	std::vector<std::string> args;
	if (parseArguments(args) < 3)
		return false;

	if (args.size() == 3)
	{
		char buf[16];
		sprintf(buf, "%d", (int)Vector4F::Null().w);
		args.push_back(buf);
	}

	v = Vector4F(	std::atof(args[0].c_str()), 
					std::atof(args[1].c_str()), 
					std::atof(args[2].c_str()), 
					std::atof(args[3].c_str()));
	return true;

}


bool JSONRigidBodyBuilder::parseUV( float& u, float& v )
{
	std::vector<std::string> args;
	if( parseArguments(args ) != 2 )
		return false;

	u = std::atof(args[0].c_str());
	v = std::atof(args[1].c_str());
	return true;
}

bool JSONRigidBodyBuilder::parseNumber( float &f )
{
	std::vector<std::string> args;
	if( parseArguments(args ) != 1 )
		return false;

	f = std::atof(args[0].c_str());
	return true;
}

bool JSONRigidBodyBuilder::parseNumber( int &i )
{
	std::vector<std::string> args;
	if( parseArguments(args ) != 1 )
		return false;

	i = atol( args[0].c_str() ) ;
	return true;
}

bool JSONRigidBodyBuilder::parseNameValueArray(char sep, std::vector<std::pair<std::string,std::string>>& nameValueArr )
{
	if( sep == ' ' ) 
		return false;

	std::string sline = parseLine();
	std::string::size_type offset(0), prevOffset(0);
	bool bWantName(true);

	std::pair<std::string,std::string> nameValue;

	while( (offset = sline.find( bWantName ? sep : ' ', prevOffset ) ), prevOffset != std::string::npos )
	{
		std::string arg;
		
		arg = (offset == std::string::npos) ? sline.substr( prevOffset ) : sline.substr( prevOffset, offset-prevOffset );

		trim2(arg);

		if( bWantName )
		{
			nameValue.first = arg;
		}
		else
		{
			nameValue.second = arg;
			nameValueArr.push_back( nameValue );
		}

		if( offset == std::string::npos )
			break;

		bWantName = !bWantName;
		prevOffset = offset + 1;
	}

	return true;
}

int JSONRigidBodyBuilder::parseArguments( std::vector<std::string> &args )
{
	std::string sline = parseLine();
	std::string arg;

	for( size_t i =0; i < sline.length(); i++ )
	{
		if( sline[i] == ' ' )
		{
			if( arg.size() )
			{
				args.push_back( arg );
				arg.clear();
			}
		}
		else
			arg += sline[i];
	}

	if( arg.length() )
		args.push_back( arg );

	return args.size();
}

std::string JSONRigidBodyBuilder::parseLine()
{
	std::string sline;

	while (!_input_file.eof())
	{
		getline(_input_file, sline);
		trim2(sline);
		_line_count++;

		if (sline.length() == 0)
			continue;

		if ((sline.length() > 1 && sline[0] == '/' && sline[1] == '/') || sline[0] == ';')
			continue;

		break;
	}

	return sline;
}

std::vector<std::string> JSONRigidBodyBuilder::parseLineUsingSeparator( char sep )
{
	std::vector<std::string> params;
	std::string sline = parseLine();

	std::string arg;
	for( size_t i=0; i < sline.length(); i++ )
	{
		if( sline[i] == sep )
		{
			params.push_back(arg);
			arg.clear();
		}
		else
			arg += sline[i];
	}

	if( arg.length() )
		params.push_back(arg);		

	return params;
}

void JSONRigidBodyBuilder::addSectionSubElements(AeroSectionElementForceGenerator *e, float cpU, float cpV)
{
	AeroSectionForceGenerator *pSectionForceGenerator = e->parent();
	std::vector<HotMoveableList> hotMList;
	
	Vector3F eb[4];
	e->getBoundaryT( eb );

	Vector3F sb[4];
	pSectionForceGenerator->getBoundaryT( sb );
	
	Vector3F eb_sb[4];
	for( size_t i=0; i < 4; i++ )
		eb_sb[i] = eb[i] - sb[0];

	EmptyQuadBoundaryT  elementboundary;
	elementboundary.setBoundaryT( eb_sb );

	for( size_t i=0; i < pSectionForceGenerator->controlSurfaceCount(); i++ )
	{
		Vector3F vec4[4];
		
		for( size_t j=0; j < 4; j++ )
			vec4[j] = pSectionForceGenerator->controlSurfaceN(i)->getBoundary(j) - pSectionForceGenerator->getBoundary(0);

		if( elementboundary.containsInX( vec4 ) || elementboundary.leftIntersectsInX( vec4 ) || elementboundary.rightIntersectsInX( vec4 ) )
		{
			//moveable sits within this element.

			for( int k=0;k < 2; k++ )
			{
				HotMoveableList moveable;
				moveable.movable = pSectionForceGenerator->controlSurfaceN(i);
				moveable.bStart = !k;

				for( size_t j=0; j < 4; j++) 
					moveable.sortboundary[j] = vec4[j];

				hotMList.push_back( moveable );
			}
		}
	}

	std::sort( hotMList.begin(), hotMList.end(), HotMoveableListSort );
	std::set<AeroControlSurface*> moveableSet;
	
	float uStart(0.0), uEnd(0.0), uMarker(0.0);
	float u1(0.0), u2(0.0), u3(0.0);
	float v1(0.0), v2(0.0), v3(0.0);

	for( std::vector<HotMoveableList>::iterator it = hotMList.begin(); it != hotMList.end(); ++it )
	{
		e->calcFractionT( it->movable->getBoundary(0), &u1, &v1 );
		e->calcFractionT( it->movable->getBoundary(1), &u2, &v2 );
		e->calcFractionT( it->movable->getBoundary(3), &u3, &v3 );

		if( it->bStart )
		{
			if( u1 > uMarker )
			{
				float sbU[4] = {}, sbV[4] = {};
				
				sbU[0] = uMarker;	sbV[0] = 0.01f;
				sbU[1] = u1;		sbV[1] = 0.01f;
				sbU[2] = u1;		sbV[2] = 0.99f;
				sbU[3] = uMarker;	sbV[3] = 0.99f;

				addSubElementMoveableRefs( e, cpU, cpV, sbU, sbV, moveableSet );

				if( u1 > 1.0 ) 
					u1 = 1.0f;

				uMarker = u1;
			}

			moveableSet.insert( it->movable );
		}
		else
		{
			float sbU[4] = {}, sbV[4] = {};

			if( u2 > 1.0 ) 
				u2 = 1.0;
				
			if( uMarker < u2 )
			{
				sbU[0] = uMarker;	sbV[0] = 0.01f;
				sbU[1] = u2;		sbV[1] = 0.01f;
				sbU[2] = u2;		sbV[2] = 0.99f;
				sbU[3] = uMarker;	sbV[3] = 0.99f;

				addSubElementMoveableRefs( e, cpU, cpV, sbU, sbV, moveableSet );
			}

			if( u1 < 1.0f )
			{			
				moveableSet.erase( it->movable );
				uMarker = u2;
			}
			else
				moveableSet.clear();
		}
	}

	if( uMarker < 1.0f )
	{
		float sbU[4] = {}, sbV[4] = {};
		
		sbU[0] = uMarker;	sbV[0] = 0.01f;
		sbU[1] = 0.99f;		sbV[1] = 0.01f;
		sbU[2] = 0.99f;		sbV[2] = 0.99f;
		sbU[3] = uMarker;	sbV[3] = 0.99f;

		addSubElementMoveableRefs( e, cpU, cpV, sbU, sbV, moveableSet );
	}
}

void JSONRigidBodyBuilder::addSubElementMoveableRefs( AeroSectionElementForceGenerator *e, float cpU, float cpV, float sbU[4], float sbV[4], std::set<AeroControlSurface*>& moveableSet )
{
	AeroSectionSubElementForceGenerator *psub = e->add( "subElement" );

	psub->setBoundaryFromParentT( sbU, sbV );
	psub->setContactPoint( psub->calcRealT( cpU, cpV).toDouble() );

	for( auto& itMov : moveableSet )
		psub->addControlSurfaceReference( itMov );
}
