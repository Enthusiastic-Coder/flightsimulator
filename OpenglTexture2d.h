#ifndef OPENGLTEXTURE2D_H
#define OPENGLTEXTURE2D_H

#include <Serializable.h>
#include <include_gl.h>

class SDL_Surface;

class OpenGLTexture2D : public ISerializable
{
public:

    OpenGLTexture2D();
    ~OpenGLTexture2D(void);

    OpenGLTexture2D(const OpenGLTexture2D& rhs) = delete;
    OpenGLTexture2D& operator=(const OpenGLTexture2D& rhs) = delete;

    SERIALIZE_WRITE_BEGIN(1, 0)
        SERIALIZE_WRITE_ENTRY(_filename)
        SERIALIZE_WRITE_ENTRY(_minification)
        SERIALIZE_WRITE_ENTRY(_wrapMode)
        SERIALIZE_WRITE_END()

        SERIALIZE_READ_BEGIN(1, 0)
        std::string sFilename;
    SERIALIZE_READ_ENTRY(sFilename)
        SERIALIZE_READ_ENTRY(_minification)
        SERIALIZE_READ_ENTRY(_wrapMode)
        if( sFilename.length())
            Load(sFilename);
    SERIALIZE_READ_END()

    void clear();
    void setMinification(int i);
    void setMagnification(int i);
    void setWrapMode(int i);
    bool generate(int width, int height, bool isDepth);
    bool Load(std::string sfilename);
    GLuint getId() const;
    operator GLuint();
    operator bool();
    void bind();
    static void unBind();
    const std::string& filename() const;
    int width() const;
    int height() const;

    static unsigned int loadTexture(std::string filename, int minTex, int maxTex, int wrap, int *width, int *height);
    static unsigned int loadTexture(SDL_Surface* surface,int minTex, int maxTex, int wrap);
    static unsigned int createTexture(int w, int h, bool isDepth, int minTex, int maxTex, int wrap);

private:
    GLuint _texture_id = 0;
    std::string _filename;
    int _wrapMode = GL_REPEAT;
    int _minification = GL_LINEAR;
    int _magnification = GL_LINEAR;
    int _width = 0;
    int _height = 0;
};


#endif // OPENGLTEXTURE2D_H
