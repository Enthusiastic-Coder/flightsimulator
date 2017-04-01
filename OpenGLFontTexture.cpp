#include "stdafx.h"
#include "OpenGLFontTexture.h"
#include <fstream>

bool OpenGLFontTexture::loadfile(std::string filename)
{
    std::ifstream input(filename+".fnt", std::ios::binary|std::ios::in);
    if( !input.is_open())
        return false;

    input.read((char*)&_maxFontSize, sizeof(_maxFontSize));

    _fontInfo.resize(256);
    input.read((char*)&_fontInfo[0], sizeof(_fontInfo[0])*256);

    _fontTexture.setMagnification(GL_NEAREST);
    _fontTexture.setMinification(GL_NEAREST);
    return _fontTexture.Load(filename);
}

OpenGLTexture2D& OpenGLFontTexture::texture()
{
    return _fontTexture;
}

MathSupport<int>::size OpenGLFontTexture::getFontSize(unsigned char ch)
{
    if( _fontInfo.size()==0)
        return {0,0};

    chInfo& info = _fontInfo[ch];
    return { info.w, info.h};
}

MathSupport<int>::size OpenGLFontTexture::getMaxFontSize()
{
    return _maxFontSize;
}

void OpenGLFontTexture::getUV(unsigned char ch, std::pair<float, float> outPts[])
{
    if( _fontInfo.size()==0)
        return;

    chInfo& info = _fontInfo[ch];
    outPts[0].first = info.U[0];
    outPts[0].second = info.V[0];
    outPts[1].first = info.U[1];
    outPts[1].second = info.V[1];
    outPts[2].first = info.U[2];
    outPts[2].second = info.V[2];
    outPts[3].first = info.U[3];
    outPts[3].second = info.V[3];
}
