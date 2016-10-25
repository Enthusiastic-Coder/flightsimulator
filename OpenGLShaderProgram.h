#pragma once

#include <map>
#include <include_gl.h>
#include <vector3.h>
#include <vector4.h>
#include <Matrix3x3.h>
#include <Matrix4x4.h>

class OpenGLShaderProgram
{
public:
	OpenGLShaderProgram();
	virtual ~OpenGLShaderProgram();

    OpenGLShaderProgram(const OpenGLShaderProgram& rhs) = delete;
    OpenGLShaderProgram& operator=(const OpenGLShaderProgram& rhs) = delete;

	void clear();
	bool loadFiles(std::string vertex, std::string fragment);

	void sendUniform(const std::string& name, int value);
	void sendUniform(const std::string& name, float value);
    void sendUniform(const std::string& name, float x, float y);
	void sendUniform(const std::string& name, float x, float y, float z);
    void sendUniform(const std::string& name, float x, float y, float z, float w);
    void sendUniform(const std::string& name, const Vector3F& v);
    void sendUniform(const std::string& name, const Vector4F& v);
    void sendUniform(const std::string& name, const Matrix3x3F& m);
    void sendUniform(const std::string& name, const Matrix4x4F& m);

	GLint getUniformLocation(const std::string& name);
    GLint getAttribLocation(const std::string& name);

    unsigned int getProgramID();
    unsigned int getVertexShaderID();
    unsigned int getFragmentShaderID();

	operator GLuint();

	void use();
	static void useDefault();

	std::string getError();
protected:
	GLuint createShader(GLenum type, std::string filename);

private:
    GLuint _programShaderID;
    GLuint _vertexShaderID;
    GLuint _fragmentShaderID;
	std::string _error;
    std::map<std::string, GLint> _uniforms;
    std::map<std::string, GLint> _attribs;
};
