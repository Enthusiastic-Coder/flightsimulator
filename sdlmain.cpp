#include "SDLMainWindow.h"
#include <SDL_image.h>

int main(int, char *[])
{
    SDL_Init(SDL_INIT_VIDEO|SDL_INIT_JOYSTICK);

#ifdef ANDROID
    chdir( SDL_AndroidGetInternalStoragePath());
#endif

    IMG_Init(IMG_INIT_PNG);
    IMG_Init(IMG_INIT_JPG);

    SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute( SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute( SDL_GL_BUFFER_SIZE, 32);
    SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute( SDL_GL_STENCIL_SIZE, 8 );

    SDL_GL_SetSwapInterval(1);

    SDLMainWindow win;
    win.run("Flight Simulator");

    IMG_Quit();
    SDL_Quit();

    return 0;
}
