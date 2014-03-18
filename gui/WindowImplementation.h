#ifndef R64FX_GUI_WINDOW_IMPLEMENTATION_H
#define R64FX_GUI_WINDOW_IMPLEMENTATION_H

#ifdef USE_SDL2

#include "SDL2Window.h"

namespace r64fx{

inline bool init_window_implementation()
{
    return SDL2Window::init();
}
    
inline Window* create_window(int w, int h, const char* name)
{
    return SDL2Window::create(w, h, name);
}

}//namespace r64fx

#else//
#error "No valid window implementation!"
#endif//USE_SDL2

#endif//R64FX_GUI_WINDOW_IMPLEMENTATION_H