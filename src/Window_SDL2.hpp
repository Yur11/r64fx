#ifndef R64FX_GUI_WINDOW_SDL2_H
#define R64FX_GUI_WINDOW_SDL2_H

#ifdef USE_SDL2

#include <SDL2/SDL.h>
#include "Window.hpp"

namespace r64fx{
    
class Window_SDL2 : public Window{
    SDL_Window* m_SDL_Window;
    
    Point<int> recoreded_mouse_position;
    
    Window_SDL2(int width = 640, int height = 480, const char* title = "Window");
    
public:
    static Window_SDL2* create(int width = 640, int height = 480, const char* title = "Window");
    
    virtual ~Window_SDL2();
       
    void swapBuffers();
    
    inline SDL_Window* sdl_window() const { return m_SDL_Window; }
    
    virtual void makeCurrent();
    
    virtual void render();
    
    virtual Size<int> size();
    
    virtual void show();
    
    virtual void hide();
    
    virtual void warpMouse(int x, int y);
    
    virtual bool isShown();
    
    virtual bool isMaximized();

    virtual bool isMinimized();
    
    virtual void turnIntoMenu();

    static bool init();
    
    static void cleanup();
    
    static void trackMouse(bool on);
    
    static bool shouldQuit();//Temporary
    
    static void processEvents();    
};
    
};


#endif//USE_SDL2

#endif//R64FX_GUI_WINDOW_SDL2_H