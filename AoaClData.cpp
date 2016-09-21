#include "StdAfx.h"
#include <string>
#include <fstream>
#include "AoaClData.h"

using namespace std;

void AoaClData::reset()
{
	_aoa.clear();
}

bool AoaClData::load( std::string filename )
{
	reset();
	FILE *file = NULL;
	fopen_s( &file, filename.c_str(), "rt" );
	
	if( file == NULL )
		return false;

	int lineno = 0;

    char line[1024]="";
	
    const char* startstr = "-180.0";

	while( fgets( line, sizeof(line)/sizeof(line[0])-1, file ) )
	{
		if( strncmp( line, startstr, strlen(startstr) ) == 0 )
			break;
	}

	do
	{
		char *token = NULL;
		char *next_token = NULL;
		bool bEnd = false;
		token = strtok_s( line, " ", &next_token );

		if( atof( token ?token :"0") == 180.0f )	
			bEnd = true;

		int arg = 0;
		DATA data;
		double aoa =0.0f;

		while( token )
		{	
			( arg==0 ? aoa : arg==1 ? data.cl : arg==2 ? data.cd : data.cm ) = atof(token);
			token = strtok_s( NULL, " ", &next_token );
			arg++;
		}

		if( aoa < 180 )
		{
			for( int i=0; i < (( aoa < -20 || aoa >= 20 ) ? 10 : 1); i++ )
				_aoa.push_back(data);
		}
		else
			_aoa.push_back(data);

		if( bEnd ) break;

	} while( fgets( line, sizeof(line)/sizeof(line[0])-1, file ) );

	fclose( file );
	blend(&DATA::cl);
	blend(&DATA::cd);
	blend(&DATA::cm);
	return true;
}

void AoaClData::ClCdCm( double aoa, double &cl, double &cd, double &cm )
{
	if( aoa < 0 )
		aoa -= 0.00001f;
	else
		aoa += 0.00001f;

	int idx = int(aoa*10)+1800;

	if( idx < 0 || idx >= (int)_aoa.size())
	{
		cl = 0.0f;
		cd = 0.0f;
		cm = 0.0f;
		return;
	}

	DATA &data = _aoa[idx];

	if( idx == _aoa.size() -1 )
	{
		cl = data.cl;
		cd = data.cd;
		cm = data.cm;
		return;
	}

	DATA &data2 = _aoa[idx+1];

	double scale = (aoa - (idx-1800)/10.0f)/0.1f;

	cl = (data2.cl - data.cl) * scale + data.cl;
	cd = (data2.cd - data.cd) * scale + data.cd;
	cm = (data2.cm - data.cm) * scale + data.cm;
}

void AoaClData::output()
{
	ofstream out( "out.txt" );
	for( size_t i=0; i < _aoa.size(); i++ )
	{
		out << i << " : (" << (i/10.0f)-180.0f << ") " << _aoa[i].cl;
		out << "\t\t" << _aoa[i].cd << "\t\t" << _aoa[i].cm << endl;
	}

	out.close();
//	ShellExecuteA( NULL, "open", "out.txt", NULL, NULL, SW_NORMAL );
}

void AoaClData::blend(double DATA::*cx)
{
	int idx_start = 0;
	double cx_start = -999.0f;

	for( size_t i=0; i < _aoa.size(); i++ )
	{
		if( _aoa[i].*cx != cx_start )
		{
			if( i )
			{
				double grad = double(_aoa[i].*cx - cx_start) / (i - idx_start);
				for( size_t j = idx_start + 1; j < i; j++ )
					_aoa[j].*cx = grad * (j-idx_start) + _aoa[idx_start].*cx;
			}

			idx_start = i;
			cx_start = _aoa[i].*cx;
		}
	}
}

