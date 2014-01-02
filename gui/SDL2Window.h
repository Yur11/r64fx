#ifndef R64FX_GUI_SDL2_WINDOW_H
#define R64FX_GUI_SDL2_WINDOW_H

#ifdef USE_SDL2

#include <SDL2/SDL.h>
#include "Window.h"

namespace r64fx{
    
class SDL2Window : public Window{
    SDL_Window* _window;
    SDL_GLContext _gl_context;
        
    Point<int> _recoreded_mouse_position;
    
public:
    SDL2Window(int width = 640, int height = 480, const char* title = "Window");
   ~SDL2Window();
   
    inline SDL_Window* sdl_window() const { return _window; }
    
    void makeCurrent();
    
    void swapBuffers();
    
    virtual Size<int> size();
    
    virtual void updateGeometry();
    
    virtual void warpMouse(int x, int y);
    
    static bool init();
    
    static void cleanup();
    
    static void trackMouse(bool on);
    
    static bool shouldQuit();//Temporary
    
    static void processEvents();
};
    
};


#endif//USE_SDL2

#endif//R64FX_GUI_SDL2_WINDOW_H