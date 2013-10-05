#ifndef R64FX_GUI_WINDOW_H
#define R64FX_GUI_WINDOW_H

#ifdef USE_SDL2
#include <SDL2/SDL.h>
#endif//USE_SDL2

#include "WindowBase.h"

namespace r64fx{
    
class Window : public WindowBase{
#ifdef USE_SDL2
    SDL_Window* _window;
    SDL_GLContext _gl_context;
#endif//USE_SDL2
        
    Point<int> _recoreded_mouse_position;
    
public:
    Window(int width = 640, int height = 480, const char* title = "Window");
   ~Window();
   
#ifdef USE_SDL2
    inline SDL_Window* sdl_window() const { return _window; }
#endif//USE_SDL2
    
    void makeCurrent();
    
    void swapBuffers();
    
    virtual Size<int> size();
    
    virtual void updateMaxSize();
    
    virtual void updateGeometry();
    
    static bool init();
    
    static void cleanup();
    
    static void trackMouse(bool on);
    
    static bool shouldQuit();//Temporary
    
    static void processEvents();
};
    
};

#endif//R64FX_GUI_WINDOW_H