#include "stdafx.h"
#include "TextureLoader.h"

#include <QImage>
#include <QtLogging>

namespace TextureLoader
{
    unsigned int loadTexture(std::string filename, int minTex, int maxTex, int wrap, int* width, int* height)
    {
        QImage image(QString::fromStdString(filename));
        if (image.isNull())
        {
            qWarning("Could not load texture: %s", filename.c_str());
            return 0;
        }

        GLint maximumSize = 0;
        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maximumSize);
        if (maximumSize > 0 && qMax(image.width(), image.height()) > maximumSize)
            image = image.scaled(maximumSize, maximumSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);

        image = image.convertToFormat(QImage::Format_RGBA8888);
#if QT_VERSION >= QT_VERSION_CHECK(6, 9, 0)
        image.flip(Qt::Vertical);
#else
        image = image.mirrored();
#endif
        if (image.isNull())
            return 0;

        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, maxTex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minTex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width(), image.height(),
                     0, GL_RGBA, GL_UNSIGNED_BYTE, image.constBits());

        *width = image.width();
        *height = image.height();
        return texture;
    }
}
