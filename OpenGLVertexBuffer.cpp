#include "stdafx.h"
#include "include_gl.h"
#include "OpenGLVertexBuffer.h"

OpenGLVertexBuffer::OpenGLVertexBuffer()
{
	_id = 0;
	_target = 0;
}

OpenGLVertexBuffer::~OpenGLVertexBuffer(void)
{
	deleteBuffer();
}

void OpenGLVertexBuffer::deleteBuffer()
{
	if( _id )
	{
        glDeleteBuffers( 1, &_id );
		_id = 0;
		_target = 0;
    }
}

bool OpenGLVertexBuffer::generateArrayBuffer()
{
    return generateBuffer(GL_ARRAY_BUFFER);
}

bool OpenGLVertexBuffer::generateElementArrayBuffer()
{
    return generateBuffer(GL_ELEMENT_ARRAY_BUFFER);
}

bool OpenGLVertexBuffer::generateBuffer(GLuint target)
{
    if( _id != 0)
        glDeleteBuffers(1, &_id);

    glGenBuffers( 1, &_id );
	_target = target;

	return _id > 0;
}

bool OpenGLVertexBuffer::bufferData( int size, const GLvoid* data, unsigned int usage )
{
	bind();
    glBufferData(_target, size, data, usage );
	return glGetError() == 0;
}

OpenGLVertexBuffer::operator bool() const
{
	return _id != 0;
}


unsigned int OpenGLVertexBuffer::getId() const
{
    return _id;
}

void OpenGLVertexBuffer::bind()
{
    glBindBuffer( _target, _id );
}

void OpenGLVertexBuffer::unBindArray()
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void OpenGLVertexBuffer::unBindElementArray()
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
