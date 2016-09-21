#pragma once

#include <include_gl.h>
#include <map>

class OpenGLTexture2D;

class OpenGLTextureManager
{
public:
	~OpenGLTextureManager();

	static OpenGLTextureManager& get();

	struct MapEntry
	{
		MapEntry();
		size_t refCount;
		int minification;
		OpenGLTexture2D* texture;
		bool operator==(GLuint id) const;
	};

	void release(std::string filename);
	OpenGLTexture2D* getImage(std::string filename, int minification=GL_LINEAR);

private:
	OpenGLTextureManager();

	std::map<std::string, MapEntry*> _map;
};
