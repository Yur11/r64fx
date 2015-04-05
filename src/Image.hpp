#ifndef R64FX_IMAGE_HPP
#define R64FX_IMAGE_HPP

#ifdef USE_SDL2
#include <SDL2/SDL.h>
#endif//USE_SDL2

namespace r64fx{
    
class Image{
#ifdef USE_SDL2
    SDL_Surface* m_SDL_Surface = nullptr;
    
public:
    Image(SDL_Surface* surface) : m_SDL_Surface(surface) {}
    
    inline char* data() const { return (char*)m_SDL_Surface->pixels; }
    
    inline int width() const { return m_SDL_Surface->w; }
    
    inline int height() const { return m_SDL_Surface->h; }
    
    inline int channelCount() const { return m_SDL_Surface->format->BytesPerPixel; }
    
    inline bool isGood() const { return m_SDL_Surface != nullptr && width() > 0 && height() > 0 && channelCount() > 0; }
#endif//USE_SDL2
    
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
};
    
}//namespace r64fx

#endif//R64FX_IMAGE_HPP