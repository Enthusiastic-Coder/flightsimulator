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

	void sendUniform(std::string name, int value);
	void sendUniform(std::string name, float value);
    void sendUniform(std::string name, float x, float y);
	void sendUniform(std::string name, float x, float y, float z);
    void sendUniform(std::string name, float x, float y, float z, float w);
    void sendUniform(std::string name, Vector3F& v);
    void sendUniform(std::string name, Vector4F& v);
    void sendUniform(std::string name, Matrix3x3F& m);
    void sendUniform(std::string name, Matrix4x4F& m);

	GLint getUniformLocation(std::string name);
    GLint getAttribLocation(std::string name);

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
