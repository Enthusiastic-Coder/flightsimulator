#pragma once

#include "MeshData.h"
#include <string>

class OpenGLFontTexture;
class Renderer;

class OpenGLFontMeshBuilder
{
public:
    OpenGLFontMeshBuilder();

    void selectFont(OpenGLFontTexture* texture);
    OpenGLFontTexture* getFontTexture();

    void clear();
    void add(int x, int y, std::string str, bool bHorizontal=true);
    void add(int x, int y, int dx, int dy, char ch);

    void beginRender(Renderer* r);
    void render(Renderer* r);
    void endRender(Renderer* r);

private:
    OpenGLFontTexture* _fontTexture;
    meshData _meshData;
};
