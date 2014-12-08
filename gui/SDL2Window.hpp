#ifndef R64FX_GUI_SDL2_WINDOW_H
#define R64FX_GUI_SDL2_WINDOW_H

#ifdef USE_SDL2

#include <SDL2/SDL.h>
#include "Window.hpp"

namespace r64fx{
    
class SDL2_GL_RenderingContext;
    
class SDL2Window : public Window{
    SDL_Window* _window;
    SDL_GLContext _gl_context;
    
    Point<int> _recoreded_mouse_position;
    
    SDL2Window(RenderingContextId_t id, int width = 640, int height = 480, const char* title = "Window");
    
public:
    static SDL2Window* create(int width = 640, int height = 480, const char* title = "Window");
    
    virtual ~SDL2Window();
       
    void swapBuffers();
    
    inline SDL_Window* sdl_window() const { return _window; }
    
    virtual void makeCurrent();
    
    virtual void render();
    
    virtual Size<int> size();
    
//     virtual void updateGeometry();
    
    virtual void show();
    
    virtual void hide();
    
    virtual void warpMouse(int x, int y);
    
    virtual bool isShown();
    
    virtual bool isMaximized();

    virtual bool isMinimized();

    static bool init();
    
    static void cleanup();
    
    static void trackMouse(bool on);
    
    static bool shouldQuit();//Temporary
    
    static void processEvents();    
};
    
};


#endif//USE_SDL2

#endif//R64FX_GUI_SDL2_WINDOW_H