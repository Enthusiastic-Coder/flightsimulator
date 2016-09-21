#include "stdafx.h"
#include "OpenGLRenderBuffer.h"
#include "include_gl.h"

OpenGLRenderBuffer::OpenGLRenderBuffer() :
	_id(0),
	_target(0)
{
}

OpenGLRenderBuffer::~OpenGLRenderBuffer()
{
	if (_id)
	{
        glDeleteRenderbuffers(1, &_id);
		_id = 0;
		_target = 0;
    }
}

OpenGLRenderBuffer::operator GLuint()
{
    return _id;
}

bool OpenGLRenderBuffer::generate(int width, int height, GLenum target)
{
    if( _id != 0)
        glDeleteRenderbuffers(1, &_id);

    glGenRenderbuffers(1, &_id);

	if (_id == 0)	return false;

	_target = target;

	bind();
    glRenderbufferStorage(_target, GL_DEPTH_COMPONENT, width, height);
	unbind();

	return _id != 0;
}

void OpenGLRenderBuffer::bind()
{
    glBindRenderbuffer(_target, _id);
}

void OpenGLRenderBuffer::unbind(GLenum target)
{
    glBindRenderbuffer(target, 0);
}
