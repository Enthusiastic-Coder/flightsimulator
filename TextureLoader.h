#pragma once

#include <string>

namespace TextureLoader
{
    unsigned int loadTexture(std::string filename, int minTex, int maxTex, int wrap, int *width, int *height);
};

