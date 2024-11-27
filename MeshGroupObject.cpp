#include "stdafx.h"
#include "OpenGLRenderer.h"
#include "MeshGroupObject.h"
#include "MeshSurfaceObject.h"
#include "MeshModelArgs.h"
#include <limits>
#include <iostream>

#include "MeshHelper.h"

MeshGroupObject::MeshGroupObject(std::string sName, std::string sParentName) :
    _BufferFlags(BufferFlags_None),
    _sParentName(sParentName),
    _sName(sName),
    //_PlaneData(_meshData.vertexData),
    _bMOISkip(false)
{

}

MeshSurfaceObject* MeshGroupObject::addSurface()
{
    _meshObjects.push_back(new MeshSurfaceObject);
    return _meshObjects.back();
}

void MeshGroupObject::calcMomentOfInertia(MassChannel &mc)
{
	if (_bMOISkip)
		return;

    for (MeshSurfaceObject* it : _meshObjects)
        it->calcMomentOfInertia(mc, _meshData.vertexPtr(), (unsigned short*)_meshData.indexPtr());
}

void MeshGroupObject::calcBoundingBox(MeshModelArgs* args)
{
    args->_BoundingBox = MeshHelper::calcBoundingBox((const Vector3F*) _meshData.vertexPtr(), _meshData.vertexSize());

    for (MeshSurfaceObject *it : _meshObjects)
	{
        if (_BufferFlags & BufferFlags_Index && _meshData.indexSize())
            it->calcBoundingBox(args, _meshData.vertexPtr(), (const unsigned short*) _meshData.indexPtr());
		else
            it->calcBoundingBox(args, _meshData.vertexPtr(), 0);
	}
}

void MeshGroupObject::setMOISkipFlag(bool bValue)
{
	_bMOISkip = bValue;
}

void MeshGroupObject::setVertexFlag()
{
    (int&)_BufferFlags |= BufferFlags_Vertex;
}

void MeshGroupObject::setColorFag()
{
    (int&)_BufferFlags |= BufferFlags_Color;
}

void MeshGroupObject::setNormalFlag()
{
	(int&)_BufferFlags |= BufferFlags_Normal;
}

void MeshGroupObject::setTexCoordFlag()
{
	(int&)_BufferFlags |= BufferFlags_Texture;
}

void MeshGroupObject::setIndexFlag()
{
	(int&)_BufferFlags |= BufferFlags_Index;
}

bool MeshGroupObject::hasVertexFlag() const
{
    return _BufferFlags & BufferFlags_Vertex;
}

bool MeshGroupObject::hasColorFlag() const
{
    return _BufferFlags & BufferFlags_Color;
}

bool MeshGroupObject::hasNormalFlag() const
{
    return _BufferFlags & BufferFlags_Normal;
}

bool MeshGroupObject::hasTexCoordFlag() const
{
    return _BufferFlags & BufferFlags_Texture;
}

bool MeshGroupObject::hasIndexFlag() const
{
    return _BufferFlags & BufferFlags_Index;
}

bool MeshGroupObject::BuildVertexBuffers(GLenum usage, bool bClearMesh)
{
    //std::cout << "Building OpenGL Buffers : " << _sParentName << ":" << _sName << std::endl;

    if (_BufferFlags & BufferFlags_Vertex && _meshData.vertexSize() == 0)
		return false;

    if( _BufferFlags & BufferFlags_Color && _meshData.colorSize() ==0)
        return false;

    if (_BufferFlags & BufferFlags_Normal && _meshData.normalSize() == 0)
		return false;

    if (_BufferFlags & BufferFlags_Texture && _meshData.textureSize() == 0)
		return false;

    if (_BufferFlags & BufferFlags_Index && _meshData.indexSize() == 0)
		return false;

	bool bOk = true;

    if (hasVertexFlag())
	{
        bOk &= _VertexBuffer.generateArrayBuffer();
        bOk &= _meshData.bufferVertex(_VertexBuffer, usage);
        constexpr unsigned int maxVertexCount = std::numeric_limits<unsigned short>::max();

        if( _meshData.vertexSize() > maxVertexCount)
            std::cout << ":" << _sName  << " : WARNING vertex count exceeded limit of " << maxVertexCount << std::endl;
	}

    if(hasColorFlag())
    {
        bOk &= _ColorBuffer.generateArrayBuffer();
        bOk &= _meshData.bufferIndex(_ColorBuffer, usage);
    }

    if (hasNormalFlag())
	{
        bOk &= _NormalBuffer.generateArrayBuffer();
        bOk &= _meshData.bufferNormal(_NormalBuffer, usage);
	}

    if (hasTexCoordFlag())
    {
        bOk &= _TexCoordBuffer.generateArrayBuffer();
        bOk &= _meshData.bufferTexture(_TexCoordBuffer, usage);
    }

    if (hasIndexFlag())
	{
        bOk &= _IndicesBuffer.generateElementArrayBuffer();
        bOk &= _meshData.bufferIndex(_IndicesBuffer, usage);
    }

    if( bClearMesh) _meshData.clear();

    OpenGLVertexBuffer::unBindArray();
    OpenGLVertexBuffer::unBindElementArray();

    return bOk;
}

void MeshGroupObject::setPlaneFlag()
{
    (int&)_BufferFlags |= BufferFlags_Plane;
}

std::string MeshGroupObject::getName() const
{
	return _sName;
}

size_t MeshGroupObject::getTextureCount() const
{
    return _meshData.textureSize();
}

bool MeshGroupObject::getHeightFromPosition(const GPSLocation & gpsLocation, HeightData & heightData) const
{
	// e.g. aircraft carrier.
	return false;
}

void MeshGroupObject::Render(Renderer* r, std::vector<OpenGLTexture2D*>& textureObjects)
{
    if( hasVertexFlag() )
        if( _VertexBuffer.getId() != 0)
            r->bindVertex( Renderer::Vertex, 3, &_VertexBuffer);
        else
            r->bindVertex( Renderer::Vertex, 3, _meshData.vertexPtr() );

    if( hasColorFlag() )
        if( _ColorBuffer.getId() != 0)
            r->bindVertex( Renderer::Color, 4, &_ColorBuffer);
        else
            r->bindVertex( Renderer::Color, 4, _meshData.colorPtr() );

    if( hasNormalFlag() )
        if( _NormalBuffer.getId() != 0)
            r->bindVertex( Renderer::Normal, 3, &_NormalBuffer);
        else
            r->bindVertex( Renderer::Normal, 3, _meshData.normalPtr() );

    if( hasTexCoordFlag())
        if( _TexCoordBuffer.getId() != 0)
            r->bindVertex( Renderer::TexCoord, 2, &_TexCoordBuffer);
        else
            r->bindVertex( Renderer::TexCoord, 2, _meshData.texturePtr() );

    if( hasIndexFlag() )
        if( _IndicesBuffer.getId() != 0)
            r->bindIndex(&_IndicesBuffer);
        else
            r->bindIndex(_meshData.indexPtr());

    r->setUseIndex( (_BufferFlags & BufferFlags_Index) != 0);

    for( MeshSurfaceObject* it : _meshObjects)
        it->Render(r, textureObjects);

    r->unBindBuffers();
}
