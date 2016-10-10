#include "stdafx.h"
#include "OpenGLFrameBuffer.h"
#include "include_gl.h"

OpenGLFrameBuffer::OpenGLFrameBuffer() :
	_id(0),
	_target(0)
{
}

OpenGLFrameBuffer::~OpenGLFrameBuffer()
{
	if (_id)
	{
        glDeleteFramebuffers(1, &_id);
		_id = 0;
		_target = 0;
	}
}

OpenGLFrameBuffer::operator GLuint()
{
	return _id;
}

bool OpenGLFrameBuffer::generate(GLenum target)
{
    if( _id != 0)
        glDeleteFramebuffers(1, &_id);

    glGenFramebuffers(1, &_id);
	_target = target;
	return _id != 0;
}

void OpenGLFrameBuffer::bind()
{
    glBindFramebuffer(_target, _id);
}

void OpenGLFrameBuffer::attachColorTexture2D(int index, GLuint texture)
{
    glFramebufferTexture2D(_target, GL_COLOR_ATTACHMENT0 + index, GL_TEXTURE_2D, texture, 0);
}

void OpenGLFrameBuffer::attachDepthTexture2D(GLuint texture)
{
    glFramebufferTexture2D(_target, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture, 0);
}

void OpenGLFrameBuffer::attachDepthRenderBuffer(GLuint buffer)
{
    glFramebufferRenderbuffer(_target, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, buffer);
}

void OpenGLFrameBuffer::attachStencilRenderBuffer(GLuint buffer)
{
    glFramebufferRenderbuffer(_target, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, buffer);
}

GLenum OpenGLFrameBuffer::checkFrameBufferStatusComplete()
{
    return glCheckFramebufferStatus(_target);
}

void OpenGLFrameBuffer::unbind(GLenum target)
{
    glBindFramebuffer(target, 0);
}

