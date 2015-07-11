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

namespace Keyboard{
namespace Key{

int K0 = SDL_SCANCODE_0;
int K1 = SDL_SCANCODE_1;
int K2 = SDL_SCANCODE_2;
int K3 = SDL_SCANCODE_3;
int K4 = SDL_SCANCODE_4;
int K5 = SDL_SCANCODE_5;
int K6 = SDL_SCANCODE_6;
int K7 = SDL_SCANCODE_7;
int K8 = SDL_SCANCODE_8;
int K9 = SDL_SCANCODE_9;

int A = SDL_SCANCODE_A;
int B = SDL_SCANCODE_B;
int C = SDL_SCANCODE_C;
int D = SDL_SCANCODE_D;
int E = SDL_SCANCODE_E;
int F = SDL_SCANCODE_F;
int G = SDL_SCANCODE_G;
int H = SDL_SCANCODE_H;
int I = SDL_SCANCODE_I;
int J = SDL_SCANCODE_J;
int K = SDL_SCANCODE_K;
int L = SDL_SCANCODE_L;
int M = SDL_SCANCODE_M;
int N = SDL_SCANCODE_N;
int O = SDL_SCANCODE_O;
int P = SDL_SCANCODE_P;
int Q = SDL_SCANCODE_Q;
int R = SDL_SCANCODE_R;
int S = SDL_SCANCODE_S;
int T = SDL_SCANCODE_T;
int U = SDL_SCANCODE_U;
int V = SDL_SCANCODE_V;
int W = SDL_SCANCODE_W;
int X = SDL_SCANCODE_X;
int Y = SDL_SCANCODE_Y;
int Z = SDL_SCANCODE_Z;

int Space = SDL_SCANCODE_SPACE;

int Grave        = SDL_SCANCODE_GRAVE;
int LeftBracket  = SDL_SCANCODE_LEFTBRACKET;
int RightBracket = SDL_SCANCODE_RIGHTBRACKET;
int Minus        = SDL_SCANCODE_MINUS;
int Equals       = SDL_SCANCODE_EQUALS;
int Comma        = SDL_SCANCODE_COMMA;
int Period       = SDL_SCANCODE_PERIOD;
int Slash        = SDL_SCANCODE_SLASH;
int Backslash    = SDL_SCANCODE_BACKSLASH;

int Backspace    = SDL_SCANCODE_BACKSPACE;
int Return       = SDL_SCANCODE_RETURN;
int Escape       = SDL_SCANCODE_ESCAPE;
int Tab          = SDL_SCANCODE_TAB;
int Home         = SDL_SCANCODE_HOME;
int End          = SDL_SCANCODE_END;
int PageUp       = SDL_SCANCODE_PAGEUP;
int PageDown     = SDL_SCANCODE_PAGEDOWN;
int Insert       = SDL_SCANCODE_INSERT;
int Delete       = SDL_SCANCODE_DELETE;

int Pause        = SDL_SCANCODE_PAUSE;
int PrintScreen  = SDL_SCANCODE_PRINTSCREEN;

int CapsLock     = SDL_SCANCODE_CAPSLOCK;
int NumLock      = SDL_SCANCODE_NUMLOCKCLEAR;
int ScrollLock   = SDL_SCANCODE_SCROLLLOCK;

int LeftShift    = SDL_SCANCODE_LSHIFT;
int RightShift   = SDL_SCANCODE_RSHIFT;

int LeftCtrl     = SDL_SCANCODE_LCTRL;
int RightCtrl    = SDL_SCANCODE_RCTRL;

int LeftAlt      = SDL_SCANCODE_LALT;
int RightAlt     = SDL_SCANCODE_RALT;

int LeftGui      = SDL_SCANCODE_LGUI;
int RightGui     = SDL_SCANCODE_RGUI;

int Menu         = SDL_SCANCODE_MENU;
int ContextMenu  = SDL_SCANCODE_APPLICATION;

int F1   = SDL_SCANCODE_F1;
int F2   = SDL_SCANCODE_F2;
int F3   = SDL_SCANCODE_F3;
int F4   = SDL_SCANCODE_F4;
int F5   = SDL_SCANCODE_F5;
int F6   = SDL_SCANCODE_F6;
int F7   = SDL_SCANCODE_F7;
int F8   = SDL_SCANCODE_F8;
int F9   = SDL_SCANCODE_F9;
int F10  = SDL_SCANCODE_F10;
int F11  = SDL_SCANCODE_F11;
int F12  = SDL_SCANCODE_F12;
int F13  = SDL_SCANCODE_F13;
int F14  = SDL_SCANCODE_F14;
int F15  = SDL_SCANCODE_F15;
int F16  = SDL_SCANCODE_F16;
int F17  = SDL_SCANCODE_F17;
int F18  = SDL_SCANCODE_F18;
int F19  = SDL_SCANCODE_F19;
int F20  = SDL_SCANCODE_F20;
int F21  = SDL_SCANCODE_F21;
int F22  = SDL_SCANCODE_F22;
int F23  = SDL_SCANCODE_F23;
int F24  = SDL_SCANCODE_F24;

int Up       = SDL_SCANCODE_UP;
int Down     = SDL_SCANCODE_DOWN;
int Left     = SDL_SCANCODE_LEFT;
int Right    = SDL_SCANCODE_RIGHT;

int KP0 = SDL_SCANCODE_KP_0;
int KP1 = SDL_SCANCODE_KP_1;
int KP2 = SDL_SCANCODE_KP_2;
int KP3 = SDL_SCANCODE_KP_3;
int KP4 = SDL_SCANCODE_KP_4;
int KP5 = SDL_SCANCODE_KP_5;
int KP6 = SDL_SCANCODE_KP_6;
int KP7 = SDL_SCANCODE_KP_7;
int KP8 = SDL_SCANCODE_KP_8;
int KP9 = SDL_SCANCODE_KP_9;
int KP_Enter      = SDL_SCANCODE_KP_ENTER;
int KP_Plus       = SDL_SCANCODE_KP_PLUS;
int KP_Minus      = SDL_SCANCODE_KP_MINUS;
int KP_Multiply   = SDL_SCANCODE_KP_MULTIPLY;
int KP_Divide     = SDL_SCANCODE_KP_DIVIDE;
int KP_Period     = SDL_SCANCODE_KP_PERIOD;

}//namespace Key
}//namespace Keyboard

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
