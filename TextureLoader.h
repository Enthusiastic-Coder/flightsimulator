#pragma once

#include <string>

class SDL_Surface;

namespace TextureLoader
{
    unsigned int loadTexture(SDL_Surface* surface, int minTex, int maxTex, int wrap);
    unsigned int loadTexture(std::string filename, int minTex, int maxTex, int wrap, int *width, int *height);

};

