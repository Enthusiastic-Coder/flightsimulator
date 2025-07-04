#include "stdafx.h"
#include "CPPSourceCodeMeshModel.h"
#include "MeshModel.h"
#include "MeshGroupObject.h"
#include "MipMapTerrainMeshModelCollection.h"
#include <SDL_log.h>

bool CPPSourceCodeMeshModel::Build(std::string cppSourceFilename, bool bDelayBuffer)
{
    std::string rootDir = cppSourceFilename.substr(0, cppSourceFilename.find_last_of("."));

    std::string fname = rootDir.substr(rootDir.find_last_of("/\\") +1);

    std::ifstream inputSourceFile(rootDir + "/model.c");
    if (!inputSourceFile.is_open())
        return _bHasLoaded = false;

    for (size_t i = 0; i < fname.length(); ++i)
        fname[i] = tolower(fname[i]);

    setName(fname);

    std::vector<std::string> ignoreList;

    ignoreList.push_back("//");
    ignoreList.push_back("#include");
    ignoreList.push_back("void OGL_Set_Current_Material_Parameters");
    ignoreList.push_back("void OGL_Set_Current_Texture_Map");

    std::string sLine;
    sLine.reserve(1000);
    _tmpSubStr.reserve(1000);
    bool bHasMaterials(false);

    bool bOnTriangle(false);
    long lTriangleOffset(0L);
    long lTriangleCount(0L);
    MeshGroupObject* group = 0;
    MeshSurfaceObject* surface = 0;
    MeshObject* mesh = 0;

    while (!inputSourceFile.eof())
    {
        std::getline(inputSourceFile, sLine);

        #ifdef ANDROID
		if( sLine.length() != 0)
			sLine.erase(sLine.length()-1);
#endif
        trim2(sLine);

        bool doContinue(false);

        for (const auto &it : ignoreList)
        {
            std::string::size_type pos;

            if ((pos = sLine.find(it)) != std::string::npos)
            {
                if (pos == 0)
                    doContinue = true;
                else
                    sLine = sLine.substr(0, pos - 1);

                break;
            }
        }

        if (doContinue)
            continue;

        if (sLine.find("void") != std::string::npos)
        {
            std::getline(inputSourceFile, sLine);

#ifdef ANDROID
		if( sLine.length() != 0)
			sLine.erase(sLine.length()-1);
#endif

            std::string objectName = sLine.substr(0, sLine.find("("));
            group = addGroup(objectName);
        }

        if (sLine.find("VertexData[]") != std::string::npos)
        {
            ReadVertexData(inputSourceFile, 3, VECTOR_DATA_TYPE, reinterpret_cast<void*>(&group->_meshData));
            group->setVertexFlag();
        }

        if (sLine.find("ColorData[]") != std::string::npos)
        {
            ReadVertexData(inputSourceFile, 3, COLOR_DATA_TYPE, reinterpret_cast<void*>(&group->_meshData));
            group->setColorFag();
        }

        if (sLine.find("NormalData[]") != std::string::npos)
        {
            ReadVertexData(inputSourceFile, 3, NORMAL_DATA_TYPE, reinterpret_cast<void*>(&group->_meshData));
            group->setNormalFlag();
        }

        if (sLine.find("TexCoordData[]") != std::string::npos)
        {
            ReadVertexData(inputSourceFile, 2, TEXTURE_DATA_TYPE, reinterpret_cast<void*>(&group->_meshData));
            group->setTexCoordFlag();
        }

        if (sLine.find("Indices[]") != std::string::npos)
        {
            ReadVertexData(inputSourceFile, 1, INDEX_DATA_TYPE, reinterpret_cast<void*>(&group->_meshData));
            group->setIndexFlag();
        }

        if (sLine.find("'MOI_IGNORE';") != std::string::npos)
        {
            group->setMOISkipFlag(true);
        }

        if (sLine.find("OGL_Set_Current_Material_Parameters(") != std::string::npos)
        {
            std::string::size_type oldPos = sLine.find("(");
            std::string::size_type pos = sLine.find(",");

            surface = group->addSurface();

            surface->setShininess( subStrToFloat(sLine, oldPos + 1, pos - oldPos - 1));

            Vector3F f;
            pos = ParseVector(inputSourceFile, sLine, pos, f);
            surface->setDiffuse(f);

            ParseVector(inputSourceFile, sLine, pos, f);
            surface->setSpecular(f);

            bHasMaterials = true;
        }

        if (sLine.find("OGL_Set_Current_Texture_Map(") != std::string::npos)
        {
            if (!bHasMaterials)
            {
                Vector3F diffuseColor = surface->getDiffuse();
                Vector3F specularColor = surface->getSpecular();
                float shininessColor = surface->getShininess();

                surface = group->addSurface();

                surface->setDiffuse(diffuseColor);
                surface->setSpecular(specularColor);
                surface->setShininess(shininessColor);
            }

            std::string::size_type p1 = sLine.find("\"");
            std::string::size_type p2 = sLine.find("\"", p1 + 1);
            std::string textureName = sLine.substr(p1 + 1, p2 - p1 - 1);

            textureName = textureName.substr(textureName.find_last_of("/\\") + 1);

            if (textureName.length()) surface->setTextureIdx(getTextureIdx(rootDir, textureName));

            bHasMaterials = false;
        }

        if (sLine.find("glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, &Indices") != std::string::npos)
        {
            bHasMaterials = false;
            const auto& countOffset = GetCountOffSet(sLine);

            if (!bOnTriangle)
            {
                bOnTriangle = true;
                lTriangleCount = 0;
                lTriangleOffset = countOffset.second;

                mesh = surface->addMesh();
                mesh->setPrimitveType(GL_TRIANGLES);
            }

            lTriangleCount += countOffset.first;

            mesh->setCountOffset(lTriangleCount, lTriangleOffset);
        }
        else if (bOnTriangle)
            bOnTriangle = false;

        if (sLine.find("glDrawElements(GL_TRIANGLE_STRIP") != std::string::npos)
        {
             const auto& countOffset = GetCountOffSet(sLine);

            mesh = surface->addMesh();
            mesh->setPrimitveType(GL_TRIANGLE_STRIP);
            mesh->setCountOffset(countOffset.first, countOffset.second);
        }
    }

    calcBoundingBox();

    if (!bDelayBuffer)
        _bHasLoaded &= BuildVertexBuffers();

    return _bHasLoaded = true;
}

bool CPPSourceCodeMeshModel::saveMesh(std::string strFilename)
{
    std::ofstream outFile(strFilename, std::ios::ate | std::ios::binary );
    if( !outFile.is_open() )
        return false;

    BinaryWriteStream outStoreFile( outFile );
    Write(&outStoreFile);
    SDL_Log( "Mesh :%s: saved to obj file", strFilename.c_str());
    return true;
}

bool CPPSourceCodeMeshModel::loadMesh(std::string strFilename)
{
    std::ifstream inFile(strFilename, std::ios::binary );
    if( !inFile.is_open() )
        return false;

    BinaryReadStream inStoreFile( inFile );
    Read(&inStoreFile);

    SDL_Log( "Mesh :%s: loaded from obj file", strFilename.c_str());

    return true;
}

std::pair<unsigned int, unsigned int> CPPSourceCodeMeshModel::GetCountOffSet(const std::string& sLine)
{
    std::string::size_type p1 = sLine.find(",");
    std::string::size_type p2 = sLine.find(",", p1 + 1);

    std::string::size_type p3 = sLine.find("[");
    std::string::size_type p4 = sLine.find("]");

    return std::make_pair(subStrToLong(sLine, p1 + 1, p2 - p1 - 1), subStrToLong(sLine, p3 + 1, p4 - p3 - 1));
}

void CPPSourceCodeMeshModel::onReceiveData(DATA_TYPE token, float data[3], void* pData)
{
    if (token == VECTOR_DATA_TYPE)
        reinterpret_cast<meshData*>(pData)->addVertex(data[0], data[1], data[2]);

    if (token == COLOR_DATA_TYPE)
        reinterpret_cast<meshData*>(pData)->addColor(data[0], data[1], data[2], 1);

    if (token == NORMAL_DATA_TYPE)
        reinterpret_cast<meshData *>(pData)->addNormal(data[0], data[1], data[2]);

    if (token == TEXTURE_DATA_TYPE)
        reinterpret_cast<meshData*>(pData)->addTexture(data[0], data[1]);

    if (token == INDEX_DATA_TYPE)
        reinterpret_cast<meshData *>(pData)->addIndex(data[0]);
}

void CPPSourceCodeMeshModel::ReadVertexData(std::ifstream &stream, int dataCount, DATA_TYPE dataType, void* data)
{
    std::string sLine;
    sLine.reserve(1000);

    while (std::getline(stream, sLine))
    {
#ifdef ANDROID
		if( sLine.length() != 0)
			sLine.erase(sLine.length()-1);
#endif

        trim2(sLine);

        if (sLine.find("}") != std::string::npos)
            break;

        std::string::size_type oldPos(0);
        std::string::size_type pos(-1);

        int idx(0);
        float tmpData[3] = {};

        while ((pos = sLine.find(",", pos + 1)) != std::string::npos)
        {
            if (pos > 0)
            {
                tmpData[idx++] = subStrToFloat(sLine, oldPos, pos);
                if (idx == dataCount)
                {
                    idx = 0;
                    onReceiveData(dataType, tmpData, data);
                }
            }
            oldPos = pos + 1;
        }

        if (sLine[sLine.length() - 1] != ',')
        {
            tmpData[idx++] = subStrToFloat(sLine, oldPos);
            onReceiveData(dataType, tmpData, data);
        }
    }
}

std::string::size_type CPPSourceCodeMeshModel::ParseVector(std::ifstream & stream, const std::string& sLine, std::string::size_type pos, Vector3F & outV)
{
    std::string::size_type oldPos(pos);

    float data[3] = {};

    for (int i = 0; i < 3; ++i)
    {
        pos = sLine.find(",", pos + 1);
        data[i] = subStrToFloat(sLine, oldPos + 1, pos - oldPos - 1);
        oldPos = pos;
    }

    outV.x = data[0];
    outV.y = data[1];
    outV.z = data[2];

    return pos;
}

inline float CPPSourceCodeMeshModel::subStrToFloat(const std::string & str, std::string::size_type offset, std::string::size_type count)
{
    return atof((_tmpSubStr = str.substr(offset, count)).c_str());
}

inline long CPPSourceCodeMeshModel::subStrToLong(const std::string & str, std::string::size_type offset, std::string::size_type count)
{
    return atol((_tmpSubStr = str.substr(offset, count)).c_str());
}

/////////////////////////////////////////////////////////////////////////////

void SimpleRunwayMeshModel::Build(float fHdg, float fWidth, float fLength, float fMaxDim)
{
    std::stringstream ss;
    ss << "runway:" << fHdg << "|width:" << fWidth << "|Length:" << fLength;

    setName(ss.str());
    MeshGroupObject* group = addGroup("SimpleRunwayMeshGroup");

    QuarternionF qHdg = MathSupport<float>::MakeQHeading(fHdg);

    int iWidthSteps = fWidth / fMaxDim;
    int iHeightSteps = fLength / fMaxDim;

    for (int z = 0; z < iHeightSteps; z++)
    {
        for (int x = 0; x < iWidthSteps; x++)
        {
            group->_meshData.addVertex(QVRotate(qHdg, Vector3F(x * fMaxDim, 0, -z * fMaxDim)));
            group->_meshData.addNormal(0, 1, 0);
            group->_meshData.addTexture(float(x) / (iWidthSteps - 1)*_kU, float(z) / (iHeightSteps - 1)*_kV);
        }
    }

    for (int z = 0; z < iHeightSteps - 1; z++)
    {
        for (int x = 0; x < iWidthSteps - 1; x++)
        {
            group->_meshData.addIndex(iWidthSteps * (z + 1) + x);
            group->_meshData.addIndex(iWidthSteps * z + x);
            group->_meshData.addIndex(iWidthSteps  * z + x + 1);

            group->_meshData.addIndex(iWidthSteps * (z + 1) + x);
            group->_meshData.addIndex(iWidthSteps  * z + x + 1);
            group->_meshData.addIndex(iWidthSteps  * (z + 1) + x + 1);
        }
    }

    group->setVertexFlag();
    group->setNormalFlag();
    group->setTexCoordFlag();
    group->setIndexFlag();

    std::string rootFolder(ROOT_APP_DIRECTORY);

    //addTextureFilename(rootFolder, "runway_strip.png", GL_NEAREST);
    //addTextureFilename(rootFolder, "airport_layout.png", GL_NEAREST);

    MeshSurfaceObject* surface = group->addSurface();

    surface->setDiffuse(Vector3F(0.85f, 0.85f, 0.85f));
    surface->setSpecular(Vector3F(0.9, 0.9, 0.9));
    surface->setShininess(32);
    surface->setTextureIdx(getTextureIdx(rootFolder, _textureName, GL_NEAREST_MIPMAP_NEAREST));

    MeshObject* mesh = surface->addMesh();
    mesh->setPrimitveType(GL_TRIANGLES);
    mesh->setCountOffset(group->_meshData.indexSize(), 0);

    calcBoundingBox();
}

void SimpleRunwayMeshModel::setTextureName(std::string strName)
{
    _textureName = strName;
}

void SimpleRunwayMeshModel::setTextureMag(float u, float v)
{
	_kU = u;
	_kV = v;
}

/////////////////
void SimplePlaneMeshModel::Build(int iWidth, int iHeight, float fInterval, unsigned char r, unsigned char g, unsigned char b, float fHdg)
{
    std::stringstream ss;
    ss << "SimplePlaneMeshModel|width:" << iWidth << "|height:" << iHeight << "|fInterval:" << fInterval;
    ss << "|Color:" << r << "," << g << "," << b;

    setName(ss.str());

    QuarternionF qHdg = MathSupport<float>::MakeQHeading(fHdg);

    MeshGroupObject* group = addGroup("Plane");
    group->setVertexFlag();
    group->setNormalFlag();
    group->setIndexFlag();

    for (int z = 0; z <= iHeight; z++)
    {
        for (int x = 0; x <= iWidth; x++)
        {
            group->_meshData.addVertex(QVRotate(qHdg, Vector3F(x*fInterval, 0, z*fInterval)));
            group->_meshData.addNormal(Vector3F(0, 1, 0));
        }
    }

    for (int z = 0; z < iHeight; z++)
    {
        for (int x = 0; x < iWidth; x++)
        {
            group->_meshData.addIndex((iHeight + 1)*z + x);
            group->_meshData.addIndex((iHeight + 1)*z + x + 1);
            group->_meshData.addIndex((iHeight + 1)*(z + 1) + x);

            group->_meshData.addIndex((iHeight + 1)*z + x + 1);
            group->_meshData.addIndex((iHeight + 1)*(z + 1) + x + 1);
            group->_meshData.addIndex((iHeight + 1)*(z + 1) + x);
        }
    }

    MeshSurfaceObject* surface = group->addSurface();

    surface->setDiffuse(Vector3F(r / 255.0f, g/ 255.0f, b / 255.0f));
    surface->setSpecular(Vector3F(0.5, 0.5, 0.5));
    surface->setShininess(32);

    MeshObject* mesh = surface->addMesh();
    mesh->setPrimitveType(GL_TRIANGLES);
    mesh->setCountOffset(group->_meshData.indexSize(), 0);

    calcBoundingBox();
}

void CircularRunwayMeshModel::Build(float fHeight, float fRadius, float fWidth, float fBank, float fMaxDim)
{
	std::stringstream ss;
	ss << "CRMM->";
	ss << "|fHeight:" << fHeight;
	ss << "|fRadius:" << fRadius;
	ss << "|fWidth:" << fWidth;
	ss << "|fBank:" << fBank;
	ss << "|fMaxDim:" << fMaxDim;

	_roadBoundaryLayout.clear();

	setName(ss.str());
	MeshGroupObject* group = addGroup("Group");

	const float fMaxWidth = fMaxDim / 4;
	const float totalDeg = 360.0f;
	float fLength = 2 * M_PI * fRadius;
	float fDiffHdg = totalDeg /fMaxDim;
	int iWidthSteps = fWidth / fMaxWidth;
	int iHeightSteps = fLength / fMaxDim;
	
	Vector3F p(-fRadius, fHeight, 0);
	Vector3F d(0, 0, -fMaxDim);
	Vector3F dSide(fMaxWidth,0,0);
	auto qForwards = MathSupport<float>::MakeQHeading(totalDeg / (iHeightSteps-1));
	
	for (int z = 0; z < iHeightSteps; z++)
	{
		auto qHdg = MathSupport<float>::MakeQHeading(z*totalDeg / (iHeightSteps-1));

		Vector3F pSide = p;

		for (int x = 0; x < iWidthSteps; x++)
		{	
			auto qBank = MathSupport<float>::MakeQBank(fBank* (1-float(x)/ (iWidthSteps-1)));
			Vector3F sideWaysShift = QVRotate(qHdg*qBank, dSide);

			group->_meshData.addVertex( pSide );
			group->_meshData.addNormal(0, 1, 0);
			group->_meshData.addTexture(float(x) / (iWidthSteps - 1) * fWidth/60.0f, float(z) / (iHeightSteps - 1) * fLength/2735);
		
			pSide += sideWaysShift;
		}

		d = QVRotate(qForwards, d);
		p += d;
	}

	QuadPlaneBoundaryT boundary;

	for (int z = 0; z < iHeightSteps - 1; z++)
	{
		for (int x = 0; x < iWidthSteps - 1; x++)
		{
			int idx[6];
			idx[0] = iWidthSteps * (z + 1) + x;
			idx[1] = iWidthSteps * z + x;
			idx[2] = iWidthSteps  * z + x + 1;

			idx[3] = idx[0];
			idx[4] = idx[2];
			idx[5] = iWidthSteps  * (z + 1) + x + 1;

			for (int i = 0; i < 6; ++i)
				group->_meshData.addIndex(idx[i]);

			boundary[0] = group->_meshData.getVertex(idx[0]);
			boundary[1] = group->_meshData.getVertex(idx[1]);
			boundary[2] = group->_meshData.getVertex(idx[2]);
			boundary[3] = group->_meshData.getVertex(idx[5]);

			_roadBoundaryLayout.push_back(boundary);
		}
	}

	group->setVertexFlag();
	group->setNormalFlag();
	group->setTexCoordFlag();
	group->setIndexFlag();

	std::string rootFolder(ROOT_APP_DIRECTORY);

	MeshSurfaceObject* surface = group->addSurface();

	surface->setDiffuse(Vector3F(0.95f, 0.95f, 0.95f));
	surface->setSpecular(Vector3F(0.9, 0.9, 0.9));
	surface->setShininess(32);
	surface->setTextureIdx(getTextureIdx(rootFolder, _textureName, GL_NEAREST));

	MeshObject* mesh = surface->addMesh();
	mesh->setPrimitveType(GL_TRIANGLES);
	mesh->setCountOffset(group->_meshData.indexSize(), 0);

	calcBoundingBox();
}

void CircularRunwayMeshModel::setTextureName(std::string strName)
{
	_textureName = strName;
}

const std::vector<QuadPlaneBoundaryT>& CircularRunwayMeshModel::getBoundary() const
{
	return _roadBoundaryLayout;
}
