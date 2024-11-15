#include "stdafx.h"
#include "TextureLoader.h"
#include <include_gl.h>
#include <iostream>
#include "SDLSurfaceHelper.h"

namespace TextureLoader
{
#ifdef WANT_SDL_SDL_SURFACE
    unsigned int loadTexture(std::string filename,int minTex, int maxTex, int wrap, int* width, int* height)
    {
        SDL_Surface* surface = SDL_SurfaceHelper::loadSDLSurface(filename);

        if (surface == nullptr)
        {
            SDL_Log("loadTexture failed :%s: ", filename.c_str());
            return 0;
        }

        *width = surface->w;
        *height = surface->h;

        unsigned int openglID = loadTexture( surface, minTex, maxTex, wrap);
        SDL_FreeSurface(surface);
        return openglID;
    }

    unsigned int loadTexture(SDL_Surface *surface,int minTex, int maxTex, int wrap)
    {
        if( surface ==0 ) return 0;

        unsigned int num;
        glGenTextures(1, &num);

        glBindTexture(GL_TEXTURE_2D, num);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, maxTex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minTex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);

        return num;
    }
#endif

#ifdef WANT_QT_QIMAGE
#include <QImage>
    unsigned int loadTexture(std::string filename, int minTex, int maxTex, int wrap, int* width, int* height)
    {
        unsigned int num = -1;
        glGenTextures(1,&num);

        QImage img = QImage(QString(filename.c_str())).mirrored()
                        .convertToFormat(QImage::Format_RGBA8888);

        glBindTexture(GL_TEXTURE_2D,num);

        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,maxTex);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,minTex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA, img.width(), img.height(),
                        0, GL_RGBA, GL_UNSIGNED_BYTE, img.bits());

        *width = img.width();
        *height = img.height();
        return num;
    }
#endif

}
