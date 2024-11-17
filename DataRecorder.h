/// This file belongs solely to Mohamed Jebara [jebaramo@gmail.com] and any use of this file
/// is strictly forbidden. At no such time has permission been granted for this file to be copied
/// used or released publicly to be used on any work or derivate work.
/// Any copy's on your machine must be deleted.
/// (C) Copyright 2014 Mohamed Jebara

#pragma once

#include <vector>
#include <jibbs/vector/vector3.h>
#include <jibbs/math/Quarternion.h>
#include <jibbs/gps/GPSLocation.h>
#include <jibbs/boundary/BoundaryHelperT.h>

template<typename TArchive>
class IDataRecordable
{
public:
	virtual void setRecorderHook(TArchive& a) = 0;

};

class DataRecorder
{
public:
	DataRecorder();
	virtual ~DataRecorder();

	struct DataValue
	{
		enum ValueType
		{
			__NONE = 0,
			__INT,
			__FLOAT,
			__DOUBLE
		};

		DataValue()
		{
			Value.fValue = NULL;
			Value.dValue = NULL;
			Value.iValue = NULL;
			Type = __NONE;
		}

		DataValue(const int &v)
		{
			Type = __INT;
			Value.iValue = const_cast<int*>(&v);
		}

		DataValue(const float &v)
		{
			Type = __FLOAT;
			Value.fValue = const_cast<float*>(&v);
		}

		DataValue(const double &v)
		{
			Type = __DOUBLE;
			Value.dValue = const_cast<double*>(&v);
		}

		union
		{
			int* iValue;
			float* fValue;
			double* dValue;
		} Value;
	
		ValueType Type;

        unsigned char* data()
		{
            return (unsigned char*)Value.iValue;
		}

		size_t size()
		{
			if( Type == __INT )
				return sizeof(int);

			if( Type == __FLOAT )
				return sizeof(float);

			if( Type == __DOUBLE )
				return sizeof(double);

			return 0;
		}
	};

	class DataRecord
	{
	public:
		DataRecord(float time_so_far, size_t size)
		{
			_time_so_far = time_so_far;
            _pdata = new unsigned char[size];
		}

		DataRecord( double time_so_far, size_t size, FILE *fIn )
		{
			_time_so_far = time_so_far;
            _pdata = new unsigned char[size];
            fread( _pdata, size * sizeof(unsigned char), 1, fIn );
		}

		void clean()
		{
			delete [] _pdata;
		}

        unsigned char* data(int offset=0)
		{
			return _pdata + offset;
		}
		
		double timeSoFar()
		{
			return _time_so_far;
		}

	private:
		double _time_so_far;
        unsigned char *_pdata;
	};
	
	template<class T>
    int addDataRef( const Vector3<T>& pdata )
	{
		int offset = addDataRef(pdata.x);
		addDataRef(pdata.y);
		addDataRef(pdata.z);
		return offset;
	}

	template<class T>
	int addDataRef( const Quarternion<T>& pdata )
	{
		int offset = addDataRef(pdata.n);
		addDataRef(pdata.v);
		return offset;
	}

	int addDataRef( const GPSLocation& pdata )
	{
		int offset = addDataRef(pdata._lat);
		addDataRef(pdata._lng);
		addDataRef(pdata._height);
		return offset;
	}

	int addDataRef( const PlaneF& pdata )
	{
		int offset = addDataRef(pdata.D);
		addDataRef(pdata.N);
		addDataRef(pdata.Q);
		return offset;
	}

	int addDataRef( const QuadPlaneBoundaryT& pBoundary )
	{
		int offset = addDataRef(pBoundary[0]);
		addDataRef(pBoundary[1]);
		addDataRef(pBoundary[2]);
		addDataRef(pBoundary[3]);
		addDataRef(pBoundary.getPlane());
		return offset;
	}

	int addDataRef( DataValue pdata );
	void record(double dt);
	virtual bool moveFirst();
	virtual bool moveNext(double dt);
	float timeSoFar();
	float totalTime();
	void reset();
	bool save(std::string sfilename);
	bool load(std::string sfilename);

protected:
	virtual void Extrapolate(DataRecord *start,DataRecord *end);

private:
	void doSerialise(double dt);
	bool doRetrieve(double dt);
	void moveNextHelper( std::vector<DataRecord>::iterator &it );
	void ExtrapolateHelper(float fractionHigh, float fractionLow, void* start, void *end, DataValue& ds);

private:
	float _time_so_far;
	std::vector<DataRecord> _data;
	std::vector<DataRecord>::iterator _data_iterator;
	std::vector<DataValue> _data_schema;
	size_t _blocksize;
	bool _data_iterator_ready;
};

