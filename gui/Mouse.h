#ifndef R64FX_GUI_MOUSE_H
#define R64FX_GUI_MOUSE_H

#include "geometry.h"

#ifdef USE_SDL2
#include <SDL2/SDL.h>
#else
#error 'No valid Mouse implementation available!'
#endif//USE_SDL2

namespace r64fx{
    
class Mouse{
#ifdef USE_SDL2
    static SDL_Cursor* smile_cursor;
#endif//USE_SDL2
    
public:
    struct Button{
        static const unsigned int None   = 0;
        static const unsigned int Left   = 1;
        static const unsigned int Middle = 1<<1;
        static const unsigned int Right  = 1<<2;
        static const unsigned int WheelUp = 1<<3;
        static const unsigned int WheelDown = 1<<4;
    };
    
#ifdef USE_SDL2
    static void init();
    
    static void cleanup();
    
    static void useDefaultCursor();
    
    inline static void useSmileCursor() { SDL_SetCursor(smile_cursor); }
    
    inline static unsigned int buttons()
    {
        int x, y;
        return SDL_GetMouseState(&x, &y);
    }
    
    inline static void setRelativeMode(bool on)
    {
        SDL_SetRelativeMouseMode((SDL_bool)on);
    }
    

    inline static Point<float> relativePosition()
    {
        int x, y;
        SDL_GetRelativeMouseState(&x, &y);
        return Point<float>(x, y);
    }
#endif//USE_SDL2
};
    
}//namespace r64fx

#endif//R64FX_GUI_MOUSE_H
