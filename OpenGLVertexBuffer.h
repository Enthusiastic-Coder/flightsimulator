#pragma once

#include <include_gl.h>

class OpenGLVertexBuffer
{
public:
	OpenGLVertexBuffer();
	~OpenGLVertexBuffer();

    OpenGLVertexBuffer(const OpenGLVertexBuffer& rhs) = delete;
    OpenGLVertexBuffer& operator=(const OpenGLVertexBuffer& rhs) = delete;

	void deleteBuffer();

    bool generateArrayBuffer();
    bool generateElementArrayBuffer();

    bool bufferData(int size, const void* data, unsigned int usage);
	operator bool() const;
    unsigned int getId() const;
    void bind();

    static void unBindArray();
    static void unBindElementArray();

protected:
    bool generateBuffer(GLuint target);

private:
    unsigned int _target;
    unsigned int _id;
};

