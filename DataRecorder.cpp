/// This file belongs solely to Mohamed Jebara [jebaramo@gmail.com] and any use of this file
/// is strictly forbidden. At no such time has permission been granted for this file to be copied
/// used or released publicly to be used on any work or derivate work.
/// Any copy's on your machine must be deleted.
/// (C) Copyright 2014 Mohamed Jebara
#include "stdafx.h"
#include "DataRecorder.h"


DataRecorder::DataRecorder() : _blocksize(0), _data_iterator_ready(false)
{
	moveFirst();
}

DataRecorder::~DataRecorder()
{
	size_t size = _data.size();
	for( size_t i =0; i < size; i++ )
		_data[i].clean();
}

int DataRecorder::addDataRef( DataValue data )
{
	_data_schema.push_back(data);
	_blocksize += data.size();
	return _blocksize - data.size();
}

void DataRecorder::record(double dt)
{
	_time_so_far += dt;

    unsigned char* pBuffer = NULL;

	_data.push_back(DataRecord(_time_so_far, _blocksize ));
	pBuffer = _data.back().data();

	if( !_data_iterator_ready )
	{
		_data_iterator = _data.begin();
		_data_iterator_ready = true;
	}

	size_t pointer = 0.0f;
	size_t size = _data_schema.size();

	for( size_t i=0; i < size; i++ )
	{
		DataValue& ds = _data_schema[i];
		memcpy( pBuffer + pointer, ds.data(), ds.size() );
		pointer += ds.size();
	}
}

float DataRecorder::timeSoFar()
{
	return _time_so_far;
}

float DataRecorder::totalTime()
{
	if (_data.size() == 0)
		return 0.0f;

	return _data.rbegin()->timeSoFar();
}

void DataRecorder::reset()
{
	_time_so_far = 0.0f;
	_data.clear();
	_data_iterator = _data.begin();
	_data_iterator_ready = false;
}

bool DataRecorder::moveFirst()
{
	_time_so_far = 0.0f;
	_data_iterator = _data.begin();
	return _data_iterator_ready;
}

bool DataRecorder::moveNext(double dt)
{
	if( !_data_iterator_ready )
		return false;

	if( _data_iterator == _data.end() )
	{
		_data_iterator = _data.begin();
		_time_so_far = 0.0f;
		return false;
	}

	_time_so_far += dt;

	while( _data_iterator->timeSoFar() < _time_so_far  )
	{
		++ _data_iterator;

		if( _data_iterator == _data.end() )
		{
			--_data_iterator;
			moveNextHelper(_data_iterator);
			++_data_iterator;
			return true;
		}
	}

	if( _data_iterator != _data.begin() )
	{
		DataRecord *end = &*_data_iterator;
		--_data_iterator;
		DataRecord *start= &*_data_iterator;
		Extrapolate( start, end );
		++_data_iterator;
	}
	else
		moveNextHelper(_data_iterator);

	return true;
}

void DataRecorder::moveNextHelper( std::vector<DataRecord>::iterator &it )
{
	size_t size = _data_schema.size();
	size_t pointer = 0.0f;

	for( size_t i=0; i < size; i++ )
	{
		DataValue& ds = _data_schema[i];
		memcpy( ds.data(), it->data() + pointer, ds.size() );
		pointer += ds.size();			
	}
}

void DataRecorder::Extrapolate(DataRecord *start, DataRecord *end)
{
	size_t size = _data_schema.size();
	size_t pointer = 0.0f;

	float fractionTimeHigh = _time_so_far - start->timeSoFar();
	float fractionTimeLow = end->timeSoFar() - start->timeSoFar();

	for( size_t i=0; i < size; i++ )
	{
		DataValue& ds = _data_schema[i];
		ExtrapolateHelper( fractionTimeHigh, fractionTimeLow, start->data() + pointer, end->data() + pointer, ds );
		pointer += ds.size();
	}
}

void DataRecorder::ExtrapolateHelper(float fractionHigh, float fractionLow, void* start, void *end, DataValue& ds)
{
	if( ds.Type == ds.__INT )
	{
		int iValue = fractionHigh * (*(int*)end - *(int*)start ) /fractionLow + *(int*)start;
		memcpy( ds.data(), &iValue, ds.size() );
	}
	else if( ds.Type == ds.__FLOAT )
	{
		float fValue = fractionHigh * (*(float*)end - *(float*)start ) /fractionLow + *(float*)start;
		memcpy( ds.data(), &fValue, ds.size() );
	}
	else if( ds.Type == ds.__DOUBLE )
	{
		double dValue = fractionHigh * (*(double*)end - *(double*)start ) /fractionLow + *(double*)start;
		memcpy( ds.data(), &dValue, ds.size() );
	}
	else
		memset( ds.data(), 0, ds.size() );
}

bool DataRecorder::save(std::string sfilename)
{
	if( _data_iterator_ready == false)
		return false;

	FILE *fOut = fopen( sfilename.c_str(), "wb" );
	if( fOut == NULL )
		return false;

	fwrite( &_blocksize, sizeof(_blocksize), 1, fOut);
	
	size_t size = _data.size();
	fwrite( &size , sizeof(size ), 1, fOut);

	for( size_t i=0; i < size;i++)
	{
		float fTimeSoFar = _data[i].timeSoFar();
		fwrite( &fTimeSoFar, sizeof(fTimeSoFar), 1, fOut );
        fwrite( _data[i].data(), _blocksize * sizeof(unsigned char), 1, fOut );
	}

	fclose(fOut);
	return true;
}

bool DataRecorder::load(std::string sfilename)
{
	FILE *fIn = fopen( sfilename.c_str(), "rb" );
	if( fIn == NULL )
		return false;

	int blocksize;

	fread( &blocksize, sizeof(blocksize), 1, fIn);

	if( blocksize != _blocksize ) //Not backward compatible
	{
		fclose(fIn);
		return false;
	}

	if( _blocksize == 0 )
	{
		fclose(fIn);
		return false;
	}
	size_t size = 0;
	fread( &size, sizeof(size), 1, fIn );

	if( size == 0 )
	{
		fclose(fIn);
		return false;
	}

	_data.clear();
	_data_iterator_ready = true;

	for( size_t i=0; i < size; i++ )
	{
		float fTimeSoFar = 0.0f;
		fread( &fTimeSoFar, sizeof(fTimeSoFar), 1, fIn );
		_data.emplace_back(std::move(DataRecord(fTimeSoFar, _blocksize, fIn )) );
	}
	fclose(fIn);
	return true;
}
