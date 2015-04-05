#include "Image.hpp"

namespace r64fx{
    
Image::Image(int width, int height, int channel_count)
{
    if(width <= 0 || height <= 0 || (channel_count != 1 && channel_count != 3 && channel_count != 4))
        return;
    
#ifdef USE_SDL2
    
    unsigned int rmask, gmask, bmask, amask;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    rmask = 0xff000000;
    gmask = 0x00ff0000;
    bmask = 0x0000ff00;
    amask = 0x000000ff;
#else
    rmask = 0x000000ff;
    gmask = 0x0000ff00;
    bmask = 0x00ff0000;
    amask = 0xff000000;
#endif
    
    m_SDL_Surface = SDL_CreateRGBSurface(
        0, width, height, channel_count * 8, rmask, gmask, bmask, amask
    );
    
#endif//USE_SDL2
}

Image::~Image()
{
    if(isGood())
    {
#ifdef USE_SDL2
        SDL_FreeSurface(m_SDL_Surface);
#endif//USE_SDL2
    }
}
    
}//namespace r64fx