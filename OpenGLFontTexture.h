#pragma once

#include "OpenglTexture2d.h"
#include <MathSupport.h>
#include <string>

class OpenGLFontTexture
{
public:

    bool loadfile( std::string filename);
    void getUV(unsigned char ch, std::pair<float, float> outPts[]);
    OpenGLTexture2D& texture();
    MathSupport<int>::size getFontSize(unsigned char ch);
    MathSupport<int>::size getMaxFontSize();

private:
    struct chInfo
    {
        float U[4];
        float V[4];
        int w, h;
    };

    OpenGLTexture2D _fontTexture;
    MathSupport<int>::size _maxFontSize = {};
    std::vector<chInfo> _fontInfo;
};
