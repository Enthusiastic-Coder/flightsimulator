#include "SDLSurfaceHelper.h"

#include <SDL_image.h>
#include <iostream>

namespace SDL_SurfaceHelper
{
    inline Uint32 get_pixel32( SDL_Surface *surface, int x, int y )
    {
        //Convert the pixels to 32 bit
        Uint32 *pixels = (Uint32 *)surface->pixels;

        //Get the requested pixel
        return pixels[ ( y * surface->w ) + x ];
    }

    inline void put_pixel32( SDL_Surface *surface, int x, int y, Uint32 pixel )
    {
        //Convert the pixels to 32 bit
        Uint32 *pixels = (Uint32 *)surface->pixels;

        //Set the pixel
        pixels[ ( y * surface->w ) + x ] = pixel;
    }

    SDL_Surface *flip_surface( SDL_Surface *surface, int flags )
    {
        //Pointer to the soon to be flipped surface
        SDL_Surface *flipped = NULL;

        flipped = SDL_CreateRGBSurface( SDL_SWSURFACE, surface->w, surface->h, surface->format->BitsPerPixel, surface->format->Rmask, surface->format->Gmask, surface->format->Bmask, surface->format->Amask );

        //If the surface must be locked
        if( SDL_MUSTLOCK( surface ) )
        {
            //Lock the surface
            SDL_LockSurface( surface );
        }

         //Go through columns
        for( int x = 0, rx = flipped->w - 1; x < flipped->w; x++, rx-- )
        {
            //Go through rows
            for( int y = 0, ry = flipped->h - 1; y < flipped->h; y++, ry-- )
            {
             //Get pixel
                Uint32 pixel = get_pixel32( surface, x, y );

                //Copy pixel
                if( ( flags & FLIP_VERTICAL ) && ( flags & FLIP_HORIZONTAL ) )
                {
                    put_pixel32( flipped, rx, ry, pixel );
                }
                else if( flags & FLIP_HORIZONTAL )
                {
                    put_pixel32( flipped, rx, y, pixel );
                }
                else if( flags & FLIP_VERTICAL )
                {
                    put_pixel32( flipped, x, ry, pixel );
                }
            }
        }
        //Unlock surface
        if( SDL_MUSTLOCK( surface ) )
        {
            SDL_UnlockSurface( surface );
        }

        //Return flipped surface
        return flipped;
    }

    SDL_Surface *loadSDLSurface(std::string filename, bool bFlippedY)
    {
        SDL_Log("Loading :%s:", filename.c_str());
        SDL_Surface* surface0 = IMG_Load(filename.c_str());

        if (surface0 == NULL)
        {
            SDL_Log("%s was NOT loaded", filename.c_str());
            SDL_Log("Error :%s", SDL_GetError());
            return 0;
        }

        SDL_PixelFormat form = { 0, 0, 32, 4, 0, 0, 0x000000ff, 0x0000ff00,
            0x00ff0000, 0xff000000 };

        SDL_Surface* surface1 = SDL_ConvertSurface(surface0, &form, SDL_SWSURFACE);

        SDL_FreeSurface(surface0);

        if (surface1 == 0)
        {
            std::cout << filename << " was not loaded" << std::endl;
            return 0;
        }

        if( bFlippedY)
            surface1 = flip_surface(surface1, FLIP_VERTICAL);

        return surface1;
    }

}
