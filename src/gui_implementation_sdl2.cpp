#include "gui_implementation_iface.hpp"
#include "paint_surface_implementation_iface.hpp"
#include "ProgramImplEventIface.hpp"

#include "WindowImplIface.hpp"
#include "Mouse.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>

#include <iostream>
#include <map>

using namespace std;

namespace r64fx{

namespace Impl{
    
// bool tracking_mouse = false;
// bool should_quit = false;
// unsigned int pressed_mouse_buttons = 0;
// 
//     
// Window_SDL2::Window_SDL2(int width, int height, const char* title)
// {
//     setEventCallback(Window_SDL2::processEvents);
//         
//     m_SDL_Window = SDL_CreateWindow(
//         title, 
//         SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
//         width, height, 
//         SDL_WINDOW_HIDDEN | SDL_WINDOW_RESIZABLE
//     );
//     
//     if(!m_SDL_Window)
//     {
//         cerr << "Failed to create SDL Window!\n";
//         abort();
//     }
// 
//     SDL_SetWindowData(m_SDL_Window, "window", this);    
// }
// 
// 
// Window_SDL2::~Window_SDL2()
// {
//     cout << "Window: " << this << " destroyed!\n";
//     
//     SDL_DestroyWindow(m_SDL_Window);
// }
// 
// 
// void Window_SDL2::swapBuffers()
// {
// }
// 
// 
// void Window_SDL2::flush()
// {
//     SDL_UpdateWindowSurface(m_SDL_Window);
// }
// 
// 
// Size<int> Window_SDL2::size()
// {
//     Size<int> s;
//     SDL_GetWindowSize(m_SDL_Window, &s.w, &s.h);
//     return s;
// }
// 
// 
// void Window_SDL2::resize(int w, int h)
// {
//     SDL_SetWindowSize(m_SDL_Window, w, h);
// }
// 
// 

// 
// void Window_SDL2::warpMouse(int x, int y)
// {
//     SDL_WarpMouseInWindow(m_SDL_Window, x, height() - y);
// }
// 
// 
// bool Window_SDL2::isShown()
// {
//     return SDL_GetWindowFlags(m_SDL_Window) & SDL_WINDOW_SHOWN;
// }
// 
// 
// bool Window_SDL2::isMaximized()
// {
//     return SDL_GetWindowFlags(m_SDL_Window) & SDL_WINDOW_MAXIMIZED;
// }
// 
// 
// bool Window_SDL2::isMinimized()
// {
//     return SDL_GetWindowFlags(m_SDL_Window) & SDL_WINDOW_MINIMIZED;
// }
// 
// 


WindowImplIface* get_event_window(SDL_Event* event)
{
    return (WindowImplIface*) SDL_GetWindowData(SDL_GetWindowFromID(event->key.windowID), "window");
}

unsigned int get_event_button(const SDL_MouseButtonEvent &event)
{
    unsigned int button;
    if(event.button == SDL_BUTTON_LEFT)
        button = Mouse::Button::Left;
    else if(event.button == SDL_BUTTON_MIDDLE)
        button = Mouse::Button::Middle;
    else if(event.button == SDL_BUTTON_RIGHT)
        button = Mouse::Button::Right;
    return button;
}


bool init()
{
    if(SDL_Init(SDL_INIT_VIDEO) == 0)
    {
        return true;
    }
    else
    {
        cerr << SDL_GetError() << "\n";
        return false;
    }
}


void cleanup()
{
    SDL_Quit();
}


bool init_window(WindowImplIface* window, int extra_flags)
{
   auto sdl_window = SDL_CreateWindow(
        "", 
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
        800, 600, 
        SDL_WINDOW_HIDDEN | SDL_WINDOW_RESIZABLE | extra_flags
    );
    
    if(!sdl_window)
    {
        cerr << "Failed to create SDL Window!\n";
        return false;
    }

    SDL_SetWindowData(sdl_window, "window", window);
    window->setImplData(sdl_window);
    
    return true;
}


bool init_window_gl3(WindowImplIface* window)
{
    return init_window(window, SDL_WINDOW_OPENGL);
}


bool init_window_normal(WindowImplIface* window)
{
    return init_window(window, 0);
}


void cleanup_window(WindowImplIface* window)
{
    auto sdl_window = (SDL_Window*)window->getImplData();
    SDL_DestroyWindow(sdl_window);
}


void show_window(WindowImplIface* window)
{
    auto sdl_window = (SDL_Window*)window->getImplData();
    SDL_ShowWindow(sdl_window);
}


void hide_window(WindowImplIface* window)
{
    auto sdl_window = (SDL_Window*)window->getImplData();
    SDL_HideWindow(sdl_window);
}


void resize_window(WindowImplIface* window, int w, int h)
{
    auto sdl_window = (SDL_Window*)window->getImplData();
    SDL_SetWindowSize(sdl_window, w, h);
}


void update_window_surface(WindowImplIface* window)
{
    auto sdl_window = (SDL_Window*)window->getImplData();
    SDL_UpdateWindowSurface(sdl_window);
}


void set_window_title(WindowImplIface* window, const char* title)
{
    auto sdl_window = (SDL_Window*)window->getImplData();
    SDL_SetWindowTitle(sdl_window, title);
}


const char* window_title(WindowImplIface* window)
{
    auto sdl_window = (SDL_Window*)window->getImplData();
    return SDL_GetWindowTitle(sdl_window);
}


void turn_into_menu(WindowImplIface* window)
{
    auto sdl_window = (SDL_Window*)window->getImplData();

    SDL_SysWMinfo info;
    SDL_VERSION(&info.version);

    if(!SDL_GetWindowWMInfo(sdl_window, &info))
    {
        cerr << "Failed to get window manager info!\n";
        return;
    }

    if(info.subsystem != SDL_SYSWM_X11)
    {
        cerr << "WM is not X11!\n";
        return;
    }

    auto x11_display = info.info.x11.display;
    auto x11_window = info.info.x11.window;

    auto atom_net_wm_window_type = XInternAtom(x11_display, "_NET_WM_WINDOW_TYPE", True);
    auto atom_net_wm_window_type_menu = XInternAtom(x11_display, "_NET_WM_WINDOW_TYPE_MENU", True);
    XChangeProperty(
        x11_display,
        x11_window,
        atom_net_wm_window_type,
        XA_ATOM,
        32,
        PropModeReplace,
        (unsigned char*)&atom_net_wm_window_type_menu,
        1
    );
    XFlush(x11_display);
}


void process_window_event(ProgramImplEventIface* program, WindowImplIface* window, SDL_WindowEvent* windowevent)
{
    switch(windowevent->event)
    {
        case SDL_WINDOWEVENT_SHOWN:
            break;
        case SDL_WINDOWEVENT_HIDDEN:
            break;
        case SDL_WINDOWEVENT_EXPOSED:
            break;
        case SDL_WINDOWEVENT_MOVED:
            break;
        case SDL_WINDOWEVENT_RESIZED:
        {
            program->initResizeEvent(window, windowevent->data1, windowevent->data2 );
            break;
        }
        case SDL_WINDOWEVENT_MINIMIZED:
            break;
        case SDL_WINDOWEVENT_MAXIMIZED:
            break;
        case SDL_WINDOWEVENT_RESTORED:
            break;
        case SDL_WINDOWEVENT_ENTER:
            break;
        case SDL_WINDOWEVENT_LEAVE:
            break;
        case SDL_WINDOWEVENT_FOCUS_GAINED:
            break;
        case SDL_WINDOWEVENT_FOCUS_LOST:
            break;
        case SDL_WINDOWEVENT_CLOSE:
        {
            program->initCloseEvent(window);
            break;
        }
        default:
            break;
    }
}


void process_some_events(ProgramImplEventIface* program)
{
    static SDL_Event event;
    while(SDL_PollEvent(&event))
    {
        switch(event.type)
        {            
            case SDL_MOUSEBUTTONDOWN:
            {
                program->initMousePressEvent(
                    get_event_window(&event),
                    event.button.x, event.button.y,
                    get_event_button(event.button)
                );
                break;
            }
            
            case SDL_MOUSEBUTTONUP:
            {
                program->initMouseReleaseEvent(
                    get_event_window(&event),
                    event.button.x, event.button.y,
                    get_event_button(event.button)
                );
                break;
            }
            
            case SDL_MOUSEMOTION:
            {
                program->initMouseMoveEvent(
                    get_event_window(&event),
                    event.button.x, event.button.y,
                    get_event_button(event.button)
                );
                break;
            }
            
            case SDL_KEYDOWN:
            {
                program->initKeyPressEvent(
                    get_event_window(&event), event.key.keysym.scancode
                );
                break;
            }
            
            case SDL_KEYUP:
            {
                program->initKeyReleaseEvent(
                    get_event_window(&event), event.key.keysym.scancode
                );
                break;
            }
            
            case SDL_WINDOWEVENT:
            {
                process_window_event(
                    program,
                    get_event_window(&event),
                    &event.window
                );
                break;
            }
        }
    }
}


unsigned int pressed_mouse_buttons()
{
    return 0;
}


unsigned int keyboard_modifiers()
{
    return 0;
}


Surface get_window_surface(WindowImplIface* window)
{
    auto sdl_window = (SDL_Window*)window->getImplData();
    SDL_GetWindowSurface(sdl_window);
}


void* pixels(Surface surface)
{
    auto sdl_surface = (SDL_Surface*) surface;
    return sdl_surface->pixels;
}


void get_surface_size(int &w, int &h, Surface surface)
{
    auto sdl_surface = (SDL_Surface*) surface;
    w = sdl_surface->w;
    h = sdl_surface->h;
}


int bytes_per_pixel(Surface surface)
{
    auto sdl_surface = (SDL_Surface*) surface;
    return sdl_surface->format->BytesPerPixel;
}


void get_channel_indices(Surface surface, int &r, int &g, int &b, int &a)
{
    static const unsigned int masks[5] = {
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
        0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff,
#else
        0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000,
#endif
        0x0
    };

    auto sdl_surface = (SDL_Surface*) surface;
    auto format = sdl_surface->format;

    if(format->BytesPerPixel == 4 || format->BytesPerPixel == 3)
    {
        for(int i=0; i<5; i++)
        {
            if(format->Rmask == masks[i])
                r = i;
            else if(format->Gmask == masks[i])
                g = i;
            else if(format->Bmask == masks[i])
                b = i;
            else if(format->Amask == masks[i])
                a = i;
        }
    }
    else
    {
        cerr << "Unsupported byte per pixel count " << format->BytesPerPixel << " !\n";
        abort();
    }
}

}//namespace Impl

}//namespace r64fx
