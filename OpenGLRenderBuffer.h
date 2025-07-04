#pragma once

#include <include_gl.h>

class OpenGLRenderBuffer
{
public:
	OpenGLRenderBuffer();
	virtual ~OpenGLRenderBuffer();

    OpenGLRenderBuffer(const OpenGLRenderBuffer& rhs) = delete;
    OpenGLRenderBuffer& operator=(const OpenGLRenderBuffer& rhs) = delete;

    operator GLuint();
    bool generateDepth(int width, int height, GLenum target = GL_RENDERBUFFER);
    bool generateStencil(int width, int height, GLenum target = GL_RENDERBUFFER);
    bool generateDepthStencil(int width, int height, GLenum target = GL_RENDERBUFFER);
	
	void bind();
    static void unbind(GLenum target = GL_RENDERBUFFER);

private:
    GLuint  _id;
    GLuint  _target;
};
