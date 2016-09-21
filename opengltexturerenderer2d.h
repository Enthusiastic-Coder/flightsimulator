#ifndef OPENGLTEXTURERENDERER2D_H
#define OPENGLTEXTURERENDERER2D_H

class Renderer;
class OpenGLTexture2D;

#include <vector4.h>

class OpenGLTextureRenderer2D
{
public:
    OpenGLTextureRenderer2D(Renderer* r);
    void setColorModulator(Vector4F colorModulator);
    void setScreenDims( int x, int y, int width, int height);

    int width() const;
    int height() const;

    void beginRender();
    void render(int x, int y, int dx=-1, int dy=-1);
    void renderLineBorder(int x, int y, int dx=-1, int dy=-1);
    void endRender();

private:
    Renderer* _renderer = 0;
    Vector4F _colorModulator = Vector4F(1,1,1,1);
    int _X;
    int _Y;
    int _cx;
    int _cy;
};

#endif // OPENGLTEXTURERENDERER2D_H
