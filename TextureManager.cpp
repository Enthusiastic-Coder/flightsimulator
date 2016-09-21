#include "stdafx.h"
#include "TextureManager.h"

#include "OpenGLTexture2D.h"


OpenGLTextureManager::OpenGLTextureManager()
{
}

OpenGLTextureManager::~OpenGLTextureManager()
{
}

OpenGLTextureManager& OpenGLTextureManager::get()
{
	static OpenGLTextureManager m;
	return m;
}

void OpenGLTextureManager::release(std::string filename)
{
}

OpenGLTexture2D* OpenGLTextureManager::getImage(std::string filename, int minification)
{
    std::map<std::string, MapEntry*>::iterator it = _map.find(filename);

	MapEntry *me = 0;

    if (it != _map.end())
        me = it->second;

	if (me == 0)
	{
		std::unique_ptr<OpenGLTexture2D> tex(new OpenGLTexture2D);
		tex->setMinification(minification);

		if (tex->Load(filename))
		{
			me = new MapEntry;
			me->texture = tex.release();
			me->minification = minification;
			_map[filename] = me;
		}
	}

	if (me == 0)
		return 0;

	++me->refCount;
	return me->texture;
}

OpenGLTextureManager::MapEntry::MapEntry() :
	refCount(0),
	texture(0),
	minification(GL_NEAREST)
{
}

bool OpenGLTextureManager::MapEntry::operator==(GLuint id) const
{
	return texture->getId() == id;
}
