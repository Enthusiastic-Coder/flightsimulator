#include <include_gl.h>
#include <iostream>
#include "OpenGLTexture2D.h"
#include "TextureLoader.h"
#include <SDL_surface.h>
#include <SDL_log.h>

OpenGLTexture2D::OpenGLTexture2D()
{
}

OpenGLTexture2D::~OpenGLTexture2D()
{
    clear();
}

void OpenGLTexture2D::clear()
{
    if( _texture_id != 0)
    {
        glDeleteTextures(1, &_texture_id);
        _texture_id = 0;
        _filename = "";
    }
}

void OpenGLTexture2D::setMinification(int i)
{
    _minification = i;
}

void OpenGLTexture2D::setMagnification(int i)
{
    _magnification = i;
}

void OpenGLTexture2D::setWrapMode(int i)
{
    _wrapMode = i;
}

bool OpenGLTexture2D::generate(int width, int height, bool isDepth)
{
    if(_texture_id != 0)
        glDeleteTextures(1, &_texture_id);

    _texture_id = createTexture(width, height, isDepth, _minification, _magnification, _wrapMode);

    _width = width;
    _height = height;

    return glGetError() == GL_NO_ERROR;
}

unsigned int OpenGLTexture2D::loadTexture(std::string filename,int minTex, int maxTex, int wrap, int* width, int* height)
{
    unsigned int num = -1;
    num = TextureLoader::loadTexture(filename, minTex, maxTex, wrap, width, height);
    if( minTex != GL_LINEAR && minTex != GL_NEAREST)
        glGenerateMipmap(GL_TEXTURE_2D);
    return num;
}

unsigned int OpenGLTexture2D::loadTexture(SDL_Surface* surface,int minTex, int maxTex, int wrap)
{
    unsigned int num = -1;
    num = TextureLoader::loadTexture(surface, minTex, maxTex, wrap);
    if( minTex != GL_LINEAR && minTex != GL_NEAREST)
        glGenerateMipmap(GL_TEXTURE_2D);
    return num;
}

unsigned int OpenGLTexture2D::createTexture(int w,int h,bool isDepth, int minTex, int maxTex, int wrap)
{
   unsigned int textureId;
    glGenTextures(1,&textureId);
    glBindTexture(GL_TEXTURE_2D,textureId);

    if ( isDepth)
    {
#ifdef ANDROID
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, w, h, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, 0);
#else
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, w, h, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
#endif

    }
    else
    {
#ifdef ANDROID
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
#else
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_FLOAT, 0);
#endif
    }

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,maxTex);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,minTex);

    #ifdef ANDROID
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    #else
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    #endif

    int i = glGetError();
    if(i!=0)
    {
        SDL_Log( "(%s,%d) [%d]- Error happened while loading the texture: ", __FUNCTION__, __LINE__, i );
    }
    glBindTexture(GL_TEXTURE_2D,0);
    return textureId;
}

bool OpenGLTexture2D::Load(std::string sfilename)
{
    clear();
    _filename = sfilename;
    _texture_id = loadTexture(_filename, _minification,_magnification, _wrapMode, &_width, &_height);
    return _texture_id !=0;
}

GLuint OpenGLTexture2D::getId() const
{
    return _texture_id;
}

void OpenGLTexture2D::bind()
{
    glBindTexture(GL_TEXTURE_2D, _texture_id);
}

void OpenGLTexture2D::unBind()
{
    glBindTexture(GL_TEXTURE_2D, 0);
}

const std::string &OpenGLTexture2D::filename() const
{
    return _filename;
}

int OpenGLTexture2D::width() const
{
    return _width;
}

int OpenGLTexture2D::height() const
{
    return _height;
}

OpenGLTexture2D::operator bool()
{
    return _texture_id != 0;
}

OpenGLTexture2D::operator GLuint()
{
    return _texture_id;
}
