#include "Image.hpp"
#include <assert.h>

#include <iostream>
using namespace std;

namespace r64fx{
    
void Image::calculateChannelIndices()
{
#ifdef USE_SDL2
    static const unsigned int masks[5] = {
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
        0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff,
#else
        0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000,
#endif
        0x0
    };
    
    auto format = m_SDL_Surface->format;
    
    if(channelCount() == 4 || channelCount() == 3)
    {
        for(int i=0; i<5; i++)
        {
            if(format->Rmask == masks[i])
                ri = i;
            else if(format->Gmask == masks[i])
                gi = i;
            else if(format->Bmask == masks[i])
                bi = i;
            else if(format->Amask == masks[i])
                ai = i;
        }
    }
    else
    {
        cerr << "Unsupported channel count " << channelCount() << "!\n";
        abort();
    }
#endif//USE_SDL2
}
    
    
Image::Image(int width, int height, int channel_count)
: surface_is_mine(true)
{
    if(width <= 0 || height <= 0 || (channel_count != 3 && channel_count != 4))
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
    
    if(m_SDL_Surface)
        calculateChannelIndices();
#endif//USE_SDL2
}


Image::~Image()
{
    if(surface_is_mine && isGood())
    {
#ifdef USE_SDL2
        SDL_FreeSurface(m_SDL_Surface);
#endif//USE_SDL2
    }
}

void Image::fill(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{    
    unsigned short data[5] = { 0, 0, 0, 0 };
    data[ri] = r;
    data[gi] = g;
    data[bi] = b;
    data[ai] = a;
    
    for(int y=0; y<height(); y++)
    {
        for(int x=0; x<width(); x++)
        {
            for(int c=0; c<channelCount(); c++)
            {
                at(x, y)[c] = data[c];
            }
        }
    }
}
    
}//namespace r64fx