#pragma once

#include <include_gl.h>

class OpenGLFrameBuffer
{
public:
	OpenGLFrameBuffer();
	virtual ~OpenGLFrameBuffer();
    OpenGLFrameBuffer(const OpenGLFrameBuffer& rhs) = delete;
    OpenGLFrameBuffer& operator=(const OpenGLFrameBuffer& rhs) = delete;


	operator GLuint();
	bool generate(GLenum target = GL_FRAMEBUFFER);
	void bind();
	static void unbind(GLenum target = GL_FRAMEBUFFER);
	
	void attachColorTexture2D(int index, GLuint texture );
	void attachDepthTexture2D(GLuint texture);
	void attachDepthRenderBuffer(GLuint buffer);
	
	GLenum checkFrameBufferStatusComplete();

private:
	GLuint _id;
	GLenum _target;
};
