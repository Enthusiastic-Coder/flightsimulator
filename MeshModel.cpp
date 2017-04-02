#include "stdafx.h"
#include "MeshModel.h"
#include "MeshGroupObject.h"
#include "TextureManager.h"
#include "OpenGLRenderer.h"

MeshModel::MeshModel(std::string sName) :
    _sName(sName)
{
    addNullTexture();
}

MeshGroupObject* MeshModel::addGroup(std::string name)
{
    _objects.push_back(factoryGroupObject(name, getName()));
    return _objects.back();
}

void MeshModel::addNullTexture()
{
    _textureObjects.push_back(new OpenGLTexture2D);
}

inline std::vector<OpenGLTexture2D*>& MeshModel::getTextureObjects()
{
    return _textureObjects;
}

int MeshModel::getTextureIdx(std::string sRootDir, std::string sFilename, int minification)
{
    std::transform(sFilename.begin(), sFilename.end(), sFilename.begin(), tolower);

    OpenGLTexture2D* texture = OpenGLTextureManager::get().getImage(sRootDir + "/Textures/" + sFilename, minification);

    std::map< std::string, int>::iterator it = _textureMapNames.find(sFilename);

    if (it == _textureMapNames.end())
    {
        size_t idx = _textureObjects.size();
        _textureMapNames[sFilename] = idx;
       _textureObjects.push_back(texture);
       return idx;
    }

    return it->second;
}

void MeshModel::Render(Renderer* r)
{
    for (size_t i = 0; i < _objects.size(); ++i)
        RenderIndex(r, i);
}

void MeshModel::RenderIndex(Renderer* r, size_t idx)
{
    MeshGroupObject* obj = _objects[idx];
    obj->Render(r, getTextureObjects());
}

size_t MeshModel::size() const
{
	return _objects.size();
}

MeshGroupObject* MeshModel::operator[](int idx)
{
    return _objects[idx];
}

BoundingBox MeshModel::getBoundingBox() const
{
    return _args._BoundingBox;
}

std::string MeshModel::getName() const
{
	return _sName;
}

float MeshModel::getArea()
{
    return _args._fTotalArea;
}

size_t MeshModel::getPolyCount()
{
    return _args._polyCount;
}

float MeshModel::getVolume()
{
    return _args._fTotalVol;
}

Vector3F MeshModel::getCentroidPt()
{
    return _args._centroidLocation;
}

void MeshModel::setName(std::string sName)
{
    _sName = sName;
}

MeshGroupObject *MeshModel::factoryGroupObject(std::string name, std::string parent)
{
    return new MeshGroupObject(name, parent);
}

bool MeshModel::InFrustum(const Frustum& f)
{
	return getBoundingBox().InFrustum(f);
}

bool MeshModel::BuildVertexBuffers(GLenum usage)
{
	bool bOk(true);

    for (MeshGroupObject* it : _objects)
        bOk &= it->BuildVertexBuffers(usage);

	return bOk;
}

void MeshModel::calcMomentOfInertia(MassChannel &mc)
{
    for (MeshGroupObject* it : _objects)
		it->calcMomentOfInertia(mc);

	mc.Distribute();

    for (MeshGroupObject* it : _objects)
		it->calcMomentOfInertia(mc);
}

void MeshModel::calcBoundingBox()
{
	Vector3F minV, maxV;

    _args._centroidLocation.Reset();
    BoundingBox& bb = _args._BoundingBox;

    for (MeshGroupObject* it : _objects)
	{
        it->calcBoundingBox(&_args);

        if (it == *_objects.begin())
		{
			minV = bb.getMin();
			maxV = bb.getMax();
		}
		else
		{
            minV.x = std::min(minV.x, bb.getMin().x);
            minV.y = std::min(minV.y, bb.getMin().y);
            minV.z = std::min(minV.z, bb.getMin().z);

            maxV.x = std::max(maxV.x, bb.getMax().x);
            maxV.y = std::max(maxV.y, bb.getMax().y);
            maxV.z = std::max(maxV.z, bb.getMax().z);
		}
	}

    bb.setLimits(minV, maxV);

    MeshHelper::normalizeCentroid(_args._centroidLocation, _args._fTotalArea, _args._polyCount);
}


