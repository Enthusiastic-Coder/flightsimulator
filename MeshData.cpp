#include "stdafx.h"
#include "MeshData.h"

void meshData::clear() {
    _vertexData.clear();
    _normalData.clear();
    _texCoordData.clear();
    _indices.clear();
}

void meshData::addVertex(const Vector2F &v)
{
	_vertexData.push_back(Vector3F(v.x, v.z, 0));
}

void meshData::addVertex(float x, float y)
{
    addVertex(Vector2F(x,y));
}

void meshData::addVertex(const Vector3F &v)
{
    _vertexData.push_back(v);
}

void meshData::addNormal(const Vector3F &v)
{
    _normalData.push_back(v);
}

void meshData::addVertex(float x, float y, float z)
{
    addVertex(Vector3F(x,y,z));
}

void meshData::addColor(float r, float g, float b, float a)
{
    addColor(Vector4F(r,g,b,a));
}

void meshData::addNormal(float x, float y, float z)
{
    addNormal(Vector3F(x,y,z));
}

void meshData::addColor(const Vector4F &v)
{
    _colorData.push_back(v);
}

void meshData::addTexture(float U, float V)
{
    _texCoordData.push_back(std::make_pair(U,V));
}

void meshData::addIndex(unsigned short v)
{
    _indices.push_back(v);
}

void meshData::setVertexPtr(float arr[], size_t bytes)
{
    _vertexData.resize(bytes/sizeof(_vertexData[0]));
    memcpy(&*_vertexData.begin(), arr, bytes);
}

void meshData::setColorPtr(float arr[], size_t bytes)
{
    _colorData.resize(bytes/sizeof(_colorData[0]));
    memcpy(&*_colorData.begin(), arr, bytes);
}

void meshData::setNormalPtr(float arr[], size_t bytes)
{
    _normalData.resize(bytes/sizeof(_normalData[0]));
    memcpy(&*_normalData.begin(), arr, bytes);
}

void meshData::setTexturePtr(float arr[], size_t bytes)
{
    _texCoordData.resize(bytes/sizeof(_texCoordData[0]));
    memcpy(&*_texCoordData.begin(), arr, bytes);
}

void meshData::setIndexPtr(unsigned short arr[], size_t bytes)
{
    _indices.resize(bytes/sizeof(_indices[0]));
    memcpy(&*_indices.begin(), arr, bytes);
}

const float *meshData::vertexPtr(size_t offset) const
{
    if( offset >= _vertexData.size())
        return 0;

    return &_vertexData[offset].x;
}

const float *meshData::colorPtr(size_t offset) const
{
    if( offset >= _colorData.size())
        return 0;

    return &_colorData[offset].x;
}

const float *meshData::normalPtr(size_t offset) const
{
    if( offset >= _normalData.size() )
        return 0;

    return &_normalData[offset].x;
}

const float *meshData::texturePtr(size_t offset) const
{
    if( offset >= _texCoordData.size() )
        return 0;

    return &_texCoordData[offset].first;
}

const unsigned short *meshData::indexPtr(size_t offset) const
{
    if( offset >= _indices.size() )
        return 0;
    return &_indices[offset];
}

size_t meshData::vertexSize() const
{
    return _vertexData.size();
}

size_t meshData::colorSize() const
{
    return _colorData.size();
}

size_t meshData::normalSize() const
{
    return _normalData.size();
}

size_t meshData::textureSize() const
{
    return _texCoordData.size();
}

size_t meshData::indexSize() const
{
    return _indices.size();
}

Vector3F meshData::getVertex(size_t idx) const
{
    return _vertexData[idx];
}

Vector4F meshData::getColor(size_t idx) const
{
    return _colorData[idx];
}

Vector3F meshData::getNormal(size_t idx) const
{
    return _normalData[idx];
}

std::pair<float, float> meshData::getTexture(size_t idx) const
{
    return _texCoordData[idx];
}

unsigned short meshData::getIndex(size_t idx) const
{
    return _indices[idx];
}

void meshData::setVertex(size_t idx, Vector3F v)
{
    _vertexData[idx] = v;
}

void meshData::setColor(size_t idx, Vector4F v)
{
    _colorData[idx] = v;
}

void meshData::setNormal(size_t idx, Vector3F v)
{
    _normalData[idx] = v;
}

void meshData::setTexture(size_t idx, float U, float V)
{
    _texCoordData[idx] = std::make_pair(U,V);
}

bool meshData::bufferVertex(OpenGLVertexBuffer &buffer, int usage)
{
    return buffer.bufferData(sizeof(_vertexData[0]) * _vertexData.size(), &_vertexData[0], usage);
}

bool meshData::bufferColor(OpenGLVertexBuffer &buffer, int usage)
{
    return buffer.bufferData(sizeof(_colorData[0]) * _colorData.size(), &_colorData[0], usage);
}

bool meshData::bufferNormal(OpenGLVertexBuffer &buffer, int usage)
{
    return buffer.bufferData(sizeof(_normalData[0]) * _normalData.size(), &_normalData[0], usage);
}

bool meshData::bufferTexture(OpenGLVertexBuffer &buffer, int usage)
{
    return buffer.bufferData(sizeof(_texCoordData[0]) * _texCoordData.size(), &_texCoordData[0], usage);
}

bool meshData::bufferIndex(OpenGLVertexBuffer &buffer, int usage)
{
    return buffer.bufferData(sizeof(_indices[0]) * _indices.size(), &_indices[0], usage);
}
