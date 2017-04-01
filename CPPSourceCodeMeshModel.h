#pragma once


#include "MeshModel.h"
#include <fstream>

inline void trim2(std::string& str)
{
	std::string::size_type pos = str.find_last_not_of(" \t");
	if (pos != std::string::npos)
	{
		str.erase(pos + 1);

		pos = str.find_first_not_of(" \t");

		if (pos != std::string::npos)
			str.erase(0, pos);
	}
	else
		str = "";
}

class CPPSourceCodeMeshModel : public MeshModel
{
public:
	CPPSourceCodeMeshModel() : _bHasLoaded(false) {}

	bool hasLoaded() { return _bHasLoaded; }
	bool Build(std::string cppSourceFilename, bool bDelayBuffer = false);
    bool saveMesh(std::string strFilename);
    bool loadMesh(std::string strFilename);

private:

	enum DATA_TYPE
	{
		VECTOR_DATA_TYPE,
        COLOR_DATA_TYPE,
        NORMAL_DATA_TYPE,
		TEXTURE_DATA_TYPE,
		INDEX_DATA_TYPE,
	};

    void onReceiveData(DATA_TYPE token, float data[3], void* pData);
	void ReadVertexData(std::ifstream &stream, int dataCount, DATA_TYPE dataType, void* data);
	std::string::size_type ParseVector(std::ifstream & stream, const std::string& sLine, std::string::size_type pos, Vector3F & outV);
	std::pair<unsigned int, unsigned int> GetCountOffSet(const std::string& sLine);
	float subStrToFloat(const std::string & str, std::string::size_type offset, std::string::size_type count = std::string::npos);
	long subStrToLong(const std::string & str, std::string::size_type offset, std::string::size_type count = std::string::npos);
	std::string _tmpSubStr;

	bool _bHasLoaded;
};

class SimpleRunwayMeshModel : public MeshModel
{
public:
	void Build(float fHdg, float fWidth, float fLength, float fMaxDim);
	void setTextureName(std::string strName);
private:
	std::string _textureName;
};

class SimplePlaneMeshModel : public MeshModel
{
public:
	void Build(int iWidth, int iHeight, float fInterval, unsigned char r,
				unsigned char g, unsigned char b, float fHdg);
};

class CircularRunwayMeshModel : public MeshModel
{
public:
	void Build( float fHeight, float fRadius, float fWidth, float fBank, float fMaxDim);
	void setTextureName(std::string strName);

private:
	std::string _textureName;
};