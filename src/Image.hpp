#ifndef R64FX_IMAGE_HPP
#define R64FX_IMAGE_HPP

#ifdef USE_SDL2
#include <SDL2/SDL.h>
#endif//USE_SDL2

namespace r64fx{
    
    
class Image{
    /* Channels indices. */
    unsigned short ri = 5;
    unsigned short gi = 5;
    unsigned short bi = 5;
    unsigned short ai = 5;
    
    void calculateChannelIndices();
    
    bool surface_is_mine;
    
#ifdef USE_SDL2
    SDL_Surface* m_SDL_Surface = nullptr;
    
public:
    Image(SDL_Surface* surface) 
    : surface_is_mine(false)
    , m_SDL_Surface(surface)
    {
        calculateChannelIndices();
    }
    
    inline char* data() const { return (char*)m_SDL_Surface->pixels; }
    
    inline int width() const { return m_SDL_Surface->w; }
    
    inline int height() const { return m_SDL_Surface->h; }
    
    inline int channelCount() const { return m_SDL_Surface->format->BytesPerPixel; }
    
    inline bool hasAlpha() const { return m_SDL_Surface->format->Amask; }
    
    inline bool isGood() const { return m_SDL_Surface != nullptr && width() > 0 && height() > 0 && channelCount() > 0; }
    
    inline SDL_Surface* sdl_surface() const { return m_SDL_Surface; }
#endif//USE_SDL2
    
private:
    Image(const Image&){}
    
public:
    Image(int width, int height, int channel_count);
    
    ~Image();
    
    inline char* at(int x, int y) const 
    { 
        return data() + y * width() * channelCount() + x * channelCount();
    }
    
    inline char* operator()(int x, int y) const
    {
        return at(x, y);
    }
    
    inline unsigned short r() const { return ri; }
    inline unsigned short g() const { return gi; }
    inline unsigned short b() const { return bi; }
    inline unsigned short a() const { return ai; }
  
    void fill(unsigned char r = 0, unsigned char g = 0, unsigned char b = 0, unsigned char a = 255);
};
    
}//namespace r64fx

#endif//R64FX_IMAGE_HPP