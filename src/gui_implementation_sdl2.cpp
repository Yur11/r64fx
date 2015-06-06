#include "gui_implementation_iface.hpp"

#include "Program.hpp"
#include "MouseEvent.hpp"
#include "KeyEvent.hpp"

#include <SDL2/SDL.h>

#include <iostream>
#include <map>

using namespace std;

namespace r64fx{

namespace{
    unsigned int g_pressed_mouse_buttons = 0;
    map<Window*, SDL_Window*> g_sdl_windows;

    SDL_Window* find_sdl_window(Window* window)
    {
        auto it = g_sdl_windows.find(window);
        if(it == g_sdl_windows.end())
            return nullptr;
        else
            return it->second;
    }
};
    
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
// void Window_SDL2::turnIntoMenu()
// {
//     SDL_SysWMinfo info;
//     SDL_VERSION(&info.version);
//     
//     if(!SDL_GetWindowWMInfo(m_SDL_Window, &info))
//     {
//         cerr << "Failed to get window manager info!\n";
//         return;
//     }
//     
//     if(info.subsystem != SDL_SYSWM_X11)
//     {
//         cerr << "WM is not X11!\n";
//         return;
//     }
// 
//     auto x11_display = info.info.x11.display;
//     auto x11_window = info.info.x11.window;
//     
//     auto atom_net_wm_window_type = XInternAtom(x11_display, "_NET_WM_WINDOW_TYPE", True);
//     auto atom_net_wm_window_type_menu = XInternAtom(x11_display, "_NET_WM_WINDOW_TYPE_MENU", True);
//     XChangeProperty(
//         x11_display, 
//         x11_window, 
//         atom_net_wm_window_type, 
//         XA_ATOM, 
//         32, 
//         PropModeReplace, 
//         (unsigned char*)&atom_net_wm_window_type_menu, 
//         1
//     );
//     XFlush(x11_display);
// }


Window* get_event_window(SDL_Event* event)
{
    return (Window*) SDL_GetWindowData(SDL_GetWindowFromID(event->key.windowID), "window");
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


bool init_window(Window* window, int extra_flags)
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
    g_sdl_windows[window] = sdl_window;
    
    return true;
}


bool init_window_gl3(Window* window)
{
    return init_window(window, SDL_WINDOW_OPENGL);
}


bool init_window_normal(Window* window)
{
    return init_window(window, 0);
}


void cleanup_window(Window* window)
{
    auto sdl_window = find_sdl_window(window);
    if(!sdl_window)
    {
        cerr << "Failed to destroy SDL window!\n";
        return;
    }
    
    SDL_DestroyWindow(sdl_window);
}


void show_window(Window* window)
{
    auto sdl_window = find_sdl_window(window);
    if(!sdl_window)
    {
        cerr << "Failed to show SDL window!\n";
        return;
    }
    
    SDL_ShowWindow(sdl_window);
}


void hide_window(Window* window)
{
    auto sdl_window = find_sdl_window(window);
    if(!sdl_window)
    {
        cerr << "Failed to hide SDL window!\n";
        return;
    }
    
    SDL_HideWindow(sdl_window);
}


void process_window_event(Program* program, Window* window, SDL_WindowEvent* windowevent)
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
            break;
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
            program->closeEvent(window);
            break;
        default:
            break;
    }
}


void process_some_events(Program* program)
{
    static SDL_Event event;
    while(SDL_PollEvent(&event))
    {
        switch(event.type)
        {            
            case SDL_MOUSEBUTTONDOWN:
            {
                Window* window = get_event_window(&event);
                unsigned int mouse_button = get_event_button(event.button);
                g_pressed_mouse_buttons &= mouse_button;
                
                MouseEvent me(event.button.x, event.button.y, mouse_button);
                program->mousePressEvent(window, &me);
                break;
            }
            
            case SDL_MOUSEBUTTONUP:
            {
                Window* window = get_event_window(&event);
                unsigned int mouse_button = get_event_button(event.button);
                g_pressed_mouse_buttons &= ~mouse_button;
                
                MouseEvent me(event.button.x, event.button.y, mouse_button);
                program->mouseReleaseEvent(window, &me);
                break;
            }
            
            case SDL_MOUSEMOTION:
            {
                Window* window = get_event_window(&event);
                MouseEvent me(event.motion.x, event.motion.y, g_pressed_mouse_buttons);
                program->mouseMoveEvent(window, &me);
                break;
            }
            
            case SDL_KEYDOWN:
            {
                Window* window = get_event_window(&event);
                KeyEvent ke(event.key.keysym.scancode);
                program->keyPressEvent(window, &ke);
                break;
            }
            
            case SDL_KEYUP:
            {
                Window* window = get_event_window(&event);
                KeyEvent ke(event.key.keysym.scancode);
                program->keyReleaseEvent(window, &ke);
                break;
            }
            
            case SDL_WINDOWEVENT:
            {
                Window* window = get_event_window(&event);
                process_window_event(program, window, &event.window);
                break;
            }
        }
    }
}


unsigned int pressed_mouse_buttons()
{
    
}


unsigned int keyboard_modifiers()
{
    
}

}//namespace r64fx
