#ifndef __SDL_SURFACE_HELPER__
#define __SDL_SURFACE_HELPER__

#include <SDL.h>
#include <string>

namespace SDL_SurfaceHelper
{
#define FLIP_VERTICAL 0x1
#define FLIP_HORIZONTAL 0x2

    Uint32 get_pixel32( SDL_Surface *surface, int x, int y );
    void put_pixel32( SDL_Surface *surface, int x, int y, Uint32 pixel );
    SDL_Surface* loadSDLSurface(std::string filename, bool bFlippedY=true);
    SDL_Surface *flip_surface( SDL_Surface *surface, int flags );
}



#endif
