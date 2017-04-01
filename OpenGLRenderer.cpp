#include "stdafx.h"
#include "OpenGLRenderer.h"
#include "OpenGLPipeline.h"
#include "OpenGLVertexBuffer.h"
#include "OpenGLTexture2D.h"
#include "OpenGLShaderProgram.h"

OpenGLShaderProgram Renderer::nullShader;

Renderer::Renderer() :  _progId(&nullShader)
{
}

void Renderer::useProgram(const OpenGLShaderProgram& progId)
{
	progId.use();
    _progId = const_cast<OpenGLShaderProgram*>(&progId);
}

OpenGLShaderProgram& Renderer::progId()
{
    return *_progId;
}

void Renderer::Render()
{
    if (_bElementsIndex)
    {
        const unsigned short* ptr = 0;
        if( _indexBufferIsLocal )  ptr = _indexBufferPtr;
        glDrawElements(_primitiveType, _count, GL_UNSIGNED_SHORT,ptr+_offset);
    }
    else
        glDrawArrays(_primitiveType, _offset, _count);
}

void Renderer::bindMaterial(Vector3F& diffuseColor, Vector3F& specularColor, float shininess)
{
    Vector4F materialDiffuse = diffuseColor * fLightingFraction;
    Vector4F materialSpecular = specularColor * fLightingFraction;

    materialDiffuse.w = 1.0f;
    materialSpecular.w = 1.0f;

    OpenGLPipeline::ApplyMaterialToShader(progId(), materialDiffuse, materialDiffuse, materialSpecular, shininess);

    Vector4F lightDiffuse( fLightingFraction, fLightingFraction, fLightingFraction, 1.0f);

    OpenGLPipeline::ApplyLightToShader(progId(), lightDiffuse, lightDiffuse, lightDiffuse, shininess);
}

void Renderer::bindTexture(OpenGLTexture2D* tex)
{
    if (tex != 0)  (_pTexture2D = tex)->bind();
}

void Renderer::bindGenericBuffer(bool bLocal, int attribType, int attribSize, const float *buffer)
{
    int attribId = -1;

    if( attribType == Vertex)
        attribId = _vertexAttribId = OpenGLPipeline::GetVertexAttribId(progId());

    else if( attribType == Color)
        attribId = _colorAttribId = OpenGLPipeline::GetColorAttribId(progId());

    else if( attribType == Normal)
        attribId = _normalAttribId = OpenGLPipeline::GetNormalAttribId(progId());

    else if( attribType == TexCoord)
        attribId = _texCoordAttribId = OpenGLPipeline::GetTextureAttribId(progId());

    if( attribId == -1)
        return;

    glEnableVertexAttribArray(attribId);
    const float* ptr = 0;
    if( bLocal)
        ptr = buffer;
    else
        ((OpenGLVertexBuffer*)buffer)->bind();

    glVertexAttribPointer(attribId, attribSize, GL_FLOAT, GL_FALSE, 0, ptr);
}

void Renderer::unBindBuffers()
{  
    if( _vertexAttribId != -1 )
    {
        glDisableVertexAttribArray(_vertexAttribId);
        _vertexAttribId = -1;
    }

    if( _colorAttribId != -1 )
    {
        glDisableVertexAttribArray(_colorAttribId);
        _colorAttribId = -1;
    }

    if( _normalAttribId != -1 )
    {
        glDisableVertexAttribArray(_normalAttribId);
        _normalAttribId = -1;
    }
    if( _texCoordAttribId != -1 )
    {
        glDisableVertexAttribArray(_texCoordAttribId);
        _texCoordAttribId = -1;
    }

    OpenGLVertexBuffer::unBindArray();
    OpenGLVertexBuffer::unBindElementArray();
}

void Renderer::unBindTexture()
{
    if(_pTexture2D != 0)
    {
        OpenGLTexture2D::unBind();
        _pTexture2D = 0;
    }
}

void Renderer::bindVertex(VertexType attribType, int attribSize, const float *buffer)
{
    bindGenericBuffer(true, attribType, attribSize, buffer );
}

void Renderer::bindVertex(VertexType attribType, int attribSize, OpenGLVertexBuffer *buffer)
{
    bindGenericBuffer(false, attribType, attribSize, (const float*) buffer );
}

void Renderer::bindIndex(const unsigned short* buffer)
{
    _indexBufferPtr = buffer;
    _indexBufferIsLocal = true;
}

void Renderer::bindIndex(OpenGLVertexBuffer *buffer)
{
    _indexBufferIsLocal = false;
    _indexBufferPtr = 0;
    buffer->bind();
}

void Renderer::setVertexCountOffset(int count, int offSet)
{
    _count = count;
    _offset = offSet;
}

void Renderer::setUseIndex(bool bUseIndex)
{
    _bElementsIndex = bUseIndex;
}

void Renderer::setPrimitiveType(int type)
{
    _primitiveType = type;
}

void Renderer::clearPrimitive()
{
    _count =0;
    _offset =0;
    _primitiveType = 0;
}


