#pragma once

#include <SerializableStdStream.h>
#include "OpenGLVertexBuffer.h"
#include <vector>
#include <jibbs/vector/vector2.h>
#include <jibbs/vector/vector3.h>
#include <jibbs/vector/vector4.h>

class meshData : public ISerializable
{
public:
    SERIALIZE_WRITE_BEGIN(1, 0)
        SERIALIZE_WRITE_VECTOR(_vertexData)
        SERIALIZE_WRITE_VECTOR(_colorData)
        SERIALIZE_WRITE_VECTOR(_normalData)
        SERIALIZE_WRITE_VECTOR_PAIR(_texCoordData)
        SERIALIZE_WRITE_VECTOR(_indices)
    SERIALIZE_WRITE_END()

    SERIALIZE_READ_BEGIN(1, 0)
        SERIALIZE_READ_VECTOR(_vertexData)
        SERIALIZE_READ_VECTOR(_colorData)
        SERIALIZE_READ_VECTOR(_normalData)
        SERIALIZE_READ_VECTOR_PAIR(_texCoordData)
        SERIALIZE_READ_VECTOR(_indices)
    SERIALIZE_READ_END()

    void clear();

    void addVertex(const Vector2F& v);
    void addVertex(float x, float y);

    void addVertex(const Vector3F& v);
    void addVertex(float x, float y, float z);

    void addNormal(const Vector3F& v);
    void addNormal(float x, float y, float z);

    void addColor(const Vector4F& v);
    void addColor(float r, float g, float b, float a);

    void addTexture(float U, float V);
    void addIndex(unsigned short v);

    void setVertex(size_t idx, Vector3F v);
    void setColor(size_t idx, Vector4F v);
    void setNormal(size_t idx, Vector3F v);
    void setTexture(size_t ix, float U, float V);

    void setVertexPtr(float[], size_t);
    void setColorPtr(float[], size_t);
    void setNormalPtr(float[], size_t);
    void setTexturePtr(float[], size_t);
    void setIndexPtr(unsigned short[], size_t);

    Vector3F getVertex(size_t idx) const;
    Vector4F getColor(size_t idx) const;
    Vector3F getNormal(size_t idx) const;
    std::pair<float,float> getTexture(size_t idx) const;
    unsigned short getIndex(size_t idx) const;

    const float* vertexPtr(size_t offset=0) const;
    const float* colorPtr(size_t offset=0) const;
    const float* normalPtr(size_t offset=0) const;
    const float* texturePtr(size_t offset=0) const;
    const unsigned short *indexPtr(size_t offset=0) const;

    size_t vertexSize() const;
    size_t colorSize() const;
    size_t normalSize() const;
    size_t textureSize() const;
    size_t indexSize() const;

    bool bufferVertex(OpenGLVertexBuffer& buffer, int usage);
    bool bufferColor(OpenGLVertexBuffer& buffer, int usage);
    bool bufferNormal(OpenGLVertexBuffer& buffer, int usage);
    bool bufferTexture(OpenGLVertexBuffer& buffer, int usage);
    bool bufferIndex(OpenGLVertexBuffer& buffer, int usage);

private:
    std::vector<Vector3F> _vertexData;
    std::vector<Vector4F> _colorData;
    std::vector<Vector3F> _normalData;
    std::vector<std::pair<float, float>> _texCoordData;
    std::vector<unsigned short> _indices;
};

