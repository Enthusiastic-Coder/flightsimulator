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

OpenGLTexture2D* OpenGLTextureManager::getImage(std::string filename, int minification, int wrapMode)
{
    std::map<std::string, MapEntry*>::iterator it = _map.find(filename);

    MapEntry *me ={};

    if (it != _map.end())
        me = it->second;

    if (me == nullptr)
	{
        std::unique_ptr<OpenGLTexture2D> tex = std::make_unique<OpenGLTexture2D>();
		tex->setMinification(minification);
		tex->setWrapMode(wrapMode);

		if (tex->Load(filename))
		{
			me = new MapEntry;
			me->texture = tex.release();
			me->minification = minification;
			_map[filename] = me;
		}
	}

    if (me == nullptr)
    {
        return nullptr;
    }

	++me->refCount;
	return me->texture;
}

OpenGLTextureManager::MapEntry::MapEntry() :
	refCount(0),
    minification(GL_NEAREST),
    texture(0)
{
}

bool OpenGLTextureManager::MapEntry::operator==(GLuint id) const
{
	return texture->getId() == id;
}
