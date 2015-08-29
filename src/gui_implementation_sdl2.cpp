#include "gui_implementation_iface.hpp"

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

void* get_window_data_by_id(unsigned int window_id)
{
    if(!window_id)
        return nullptr;
    return (void*) SDL_GetWindowData(SDL_GetWindowFromID(window_id), "window");
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


WindowHandle_t init_window(WindowData_t* wd, int extra_flags)
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
        return nullptr;
    }

    SDL_SetWindowData(sdl_window, "window", wd);
    
    return sdl_window;
}


WindowHandle_t init_window_gl3(WindowData_t* wd)
{
    return init_window(wd, SDL_WINDOW_OPENGL);
}


WindowHandle_t init_window_normal(WindowData_t* wd)
{
    return init_window(wd, 0);
}


void free_window(WindowHandle_t wh)
{
    SDL_DestroyWindow((SDL_Window*)wh);
}


void show_window(WindowHandle_t wh)
{
    SDL_ShowWindow((SDL_Window*)wh);
//     SDL_MinimizeWindow((SDL_Window*)wh);
//     SDL_RestoreWindow((SDL_Window*)wh);
}


void hide_window(WindowHandle_t wh)
{
    SDL_HideWindow((SDL_Window*)wh);
}


void resize_window(WindowHandle_t wh, int w, int h)
{
    SDL_SetWindowSize((SDL_Window*)wh, w, h);
}


void update_window_surface(WindowHandle_t wh)
{
    SDL_UpdateWindowSurface((SDL_Window*)wh);
}


void set_window_title(WindowHandle_t wh, const char* title)
{
    SDL_SetWindowTitle((SDL_Window*)wh, title);
}


const char* window_title(WindowHandle_t wh)
{
    return SDL_GetWindowTitle((SDL_Window*)wh);
}


void turn_into_menu(WindowHandle_t wh)
{
    auto sdl_window = (SDL_Window*)wh;

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


void process_window_event(Events* events, Window* wd, SDL_WindowEvent* windowevent)
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
            events->resize(wd, windowevent->data1, windowevent->data2);
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
            events->close(wd);
            break;
        }
        default:
            break;
    }
}


void process_some_events(Events* events)
{
    static SDL_Event event;
    while(SDL_PollEvent(&event))
    {
        switch(event.type)
        {            
            case SDL_MOUSEBUTTONDOWN:
            {
                events->mouse_press(
                    get_window_data_by_id(event.button.windowID),
                    event.button.x, event.button.y,
                    get_event_button(event.button)
                );
                auto window = SDL_GetKeyboardFocus();
                cout << "focus: " << window << "\n";
                break;
            }
            
            case SDL_MOUSEBUTTONUP:
            {
                events->mouse_release(
                    get_window_data_by_id(event.button.windowID),
                    event.button.x, event.button.y,
                    get_event_button(event.button)
                );
                break;
            }
            
            case SDL_MOUSEMOTION:
            {
                events->mouse_move(
                    get_window_data_by_id(event.motion.windowID),
                    event.button.x, event.button.y,
                    get_event_button(event.button)
                );
                break;
            }
            
            case SDL_KEYDOWN:
            {
                if(!event.key.repeat)
                {
                    events->key_press(
                        get_window_data_by_id(event.key.windowID), event.key.keysym.scancode
                    );
                }
                auto window = SDL_GetKeyboardFocus();
                cout << "focus: " << window << "\n";
                break;
            }
            
            case SDL_KEYUP:
            {
                events->key_release(
                    get_window_data_by_id(event.key.windowID), event.key.keysym.scancode
                );
                auto window = SDL_GetKeyboardFocus();
                cout << "focus: " << window << "\n";
                break;
            }
            
            case SDL_WINDOWEVENT:
            {
                process_window_event(
                    events,
                    get_window_data_by_id(event.window.windowID),
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


WindowSurface get_window_surface(WindowHandle_t wh)
{
    return WindowSurface(SDL_GetWindowSurface((SDL_Window*)wh));
}


void* pixels(WindowSurface surface)
{
    auto sdl_surface = (SDL_Surface*) surface.ptr;
    return sdl_surface->pixels;
}


void get_surface_size(int &w, int &h, WindowSurface surface)
{
    auto sdl_surface = (SDL_Surface*) surface.ptr;
    w = sdl_surface->w;
    h = sdl_surface->h;
}


int bytes_per_pixel(WindowSurface surface)
{
    auto sdl_surface = (SDL_Surface*) surface.ptr;
    return sdl_surface->format->BytesPerPixel;
}


void get_channel_indices(WindowSurface surface, int &r, int &g, int &b, int &a)
{
    static const unsigned int masks[5] = {
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
        0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff,
#else
        0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000,
#endif
        0x0
    };

    auto sdl_surface = (SDL_Surface*) surface.ptr;
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
