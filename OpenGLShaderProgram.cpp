#include "stdafx.h"
#include "OpenGLShaderProgram.h"
#include <string>
#include <fstream>
#include "include_gl.h"

bool readAll(std::string sFilename, std::string &text)
{
    std::ifstream inputFile;

    inputFile.open(sFilename, std::ifstream::in);

    if (inputFile.is_open() == false)
        return false;

    std::stringstream ss;

    ss << inputFile.rdbuf();
    text = ss.str();
    return true;
}


OpenGLShaderProgram::OpenGLShaderProgram() :
	_programShaderID(0),
	_vertexShaderID(0),
	_fragmentShaderID(0)
{
}

OpenGLShaderProgram::~OpenGLShaderProgram()
{
	clear();
}

void OpenGLShaderProgram::clear()
{
    glDeleteShader(_vertexShaderID);
	_vertexShaderID = 0;

    glDeleteShader(_fragmentShaderID);
	_fragmentShaderID = 0;

    glDeleteProgram(_programShaderID);
	_programShaderID = 0;

	_error = "";
}

void OpenGLShaderProgram::use() const
{
    glUseProgram(_programShaderID);
}

GLuint OpenGLShaderProgram::createShader(GLenum type, const char* charSource)
{
    char errBuffer[256] = {};
    GLuint shaderID = glCreateShader(type);
    glShaderSource(shaderID, 1, &charSource, 0);
    glCompileShader(shaderID);
    glGetShaderInfoLog(shaderID, sizeof(errBuffer), 0, errBuffer);
	_error.append(errBuffer);
    if( _error.length())  _error = std::string(charSource) + " : " + _error;
	return shaderID;
}

std::string OpenGLShaderProgram::loadCode(const std::string & filename)
{
	std::string strSource;
	readAll(filename, strSource);
#ifndef ANDROID
	strSource = "#define highp\n#define mediump\n#define lowp\n" + strSource;
#endif
	return strSource;
}

bool OpenGLShaderProgram::loadFiles(std::string vertexFilename, std::string fragmentFilename)
{
	_vertexName = vertexFilename;
	_fragmentName = fragmentFilename;
	return loadSrc(loadCode(vertexFilename), loadCode(fragmentFilename));
}

bool OpenGLShaderProgram::loadSrc(std::string vertex, std::string fragment)
{
	_vertexShaderID = createShader(GL_VERTEX_SHADER, vertex.c_str());
	_fragmentShaderID = createShader(GL_FRAGMENT_SHADER, fragment.c_str());
	_programShaderID = glCreateProgram();

	glAttachShader(_programShaderID, _vertexShaderID);
	glAttachShader(_programShaderID, _fragmentShaderID);
	glLinkProgram(_programShaderID);

	GLint link_ok = true;
	glGetProgramiv(_programShaderID, GL_LINK_STATUS, &link_ok);
	if (!link_ok)
	{
		char errBuffer[256] = {};
		glGetProgramInfoLog(_programShaderID, sizeof(errBuffer), 0, errBuffer);
		_error.append(errBuffer);
	}

	return link_ok;
}

void OpenGLShaderProgram::sendUniform(const std::string& name, int value)
{
    glUniform1i(getUniformLocation(name), value);
}

void OpenGLShaderProgram::sendUniform(const std::string& name, float value)
{
    glUniform1f(getUniformLocation(name), value);
}

void OpenGLShaderProgram::sendUniform(const std::string& name, float x, float y)
{
    glUniform2f(getUniformLocation(name), x, y);
}

void OpenGLShaderProgram::sendUniform(const std::string& name, float x, float y, float z)
{
    glUniform3f(getUniformLocation(name), x, y, z);
}

void OpenGLShaderProgram::sendUniform(const std::string& name, float x, float y, float z, float w)
{
    glUniform4f(getUniformLocation(name), x, y, z, w);
}

void OpenGLShaderProgram::sendUniform(const std::string& name, const Vector3F &v)
{
    glUniform3fv(getUniformLocation(name), 1, v.ptr());
}

void OpenGLShaderProgram::sendUniform(const std::string& name, const Vector4F &v)
{
    glUniform4fv(getUniformLocation(name), 1, v.ptr());
}

void OpenGLShaderProgram::sendUniform(const std::string& name, const Matrix3x3F &m)
{
#ifdef WIN32
    glUniformMatrix3fv(getUniformLocation(name),1, GL_TRUE, m.ptr());
#else
    glUniformMatrix3fv(getUniformLocation(name),1, GL_FALSE, m.Transpose().ptr());
#endif
}

void OpenGLShaderProgram::sendUniform(const std::string& name, const Matrix4x4F &m)
{
#ifdef WIN32
    glUniformMatrix4fv(getUniformLocation(name), 1, GL_TRUE, m.ptr());
#else
    glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, m.Transpose().ptr());
#endif
}

GLint OpenGLShaderProgram::getUniformLocation(const std::string& name)
{
    auto it = _uniforms.find(name);

    GLint id = -1;
    if( it == _uniforms.end())
    {
        id = glGetUniformLocation(_programShaderID, name.c_str());
        _uniforms[name] = id;
    }
    else
        id = it->second;

    return id;
}

GLint OpenGLShaderProgram::getAttribLocation(const std::string& name)
{
    auto it = _attribs.find(name);

    GLint id = -1;
    if( it == _attribs.end())
    {
        id = glGetAttribLocation(_programShaderID, name.c_str());
        _attribs[name] = id;
    }
    else
        id = it->second;

    return id;
}

unsigned int OpenGLShaderProgram::getProgramID()
{
	return _programShaderID;
}

OpenGLShaderProgram::operator GLuint()
{
	return getProgramID();
}

unsigned int OpenGLShaderProgram::getVertexShaderID()
{
	return _vertexShaderID;
}

unsigned int OpenGLShaderProgram::getFragmentShaderID()
{
    return _fragmentShaderID;
}

void OpenGLShaderProgram::useDefault()
{
    glUseProgram(0);
}

std::string OpenGLShaderProgram::getError() const
{
	return _error;
}

std::string OpenGLShaderProgram::getVertexName() const
{
	return _vertexName;
}

std::string OpenGLShaderProgram::getFragmentName() const
{
	return _fragmentName;
}
