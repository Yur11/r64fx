#include "Window_SDL2.hpp"
#include "Mouse.hpp"
#include <SDL2/SDL_syswm.h>

#include <iostream>
#include <vector>

#ifdef DEBUG
#include <assert.h>
#endif//DEBUG

using namespace std;

namespace r64fx{
   
bool tracking_mouse = false;
bool should_quit = false;
unsigned int pressed_mouse_buttons = 0;

    
Window_SDL2::Window_SDL2(int width, int height, const char* title)
{
    setEventCallback(Window_SDL2::processEvents);
        
    m_SDL_Window = SDL_CreateWindow(
        title, 
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
        width, height, 
        SDL_WINDOW_HIDDEN | SDL_WINDOW_RESIZABLE
    );
    
    if(!m_SDL_Window)
    {
        cerr << "Failed to create SDL Window!\n";
        abort();
    }

    SDL_SetWindowData(m_SDL_Window, "window", this);    
}


Window_SDL2::~Window_SDL2()
{
    cout << "Window: " << this << " destroyed!\n";
    
    SDL_DestroyWindow(m_SDL_Window);
}


void Window_SDL2::swapBuffers()
{
}


void Window_SDL2::makeCurrent()
{
}


void Window_SDL2::render()
{
}


Size<int> Window_SDL2::size()
{
    Size<int> s;
    SDL_GetWindowSize(m_SDL_Window, &s.w, &s.h);
    return s;
}


void Window_SDL2::show()
{
    SDL_ShowWindow(m_SDL_Window);
}


void Window_SDL2::hide()
{
    SDL_HideWindow(m_SDL_Window);
}


void Window_SDL2::warpMouse(int x, int y)
{
    SDL_WarpMouseInWindow(m_SDL_Window, x, height() - y);
}


bool Window_SDL2::isShown()
{
    return SDL_GetWindowFlags(m_SDL_Window) & SDL_WINDOW_SHOWN;
}


bool Window_SDL2::isMaximized()
{
    return SDL_GetWindowFlags(m_SDL_Window) & SDL_WINDOW_MAXIMIZED;
}


bool Window_SDL2::isMinimized()
{
    return SDL_GetWindowFlags(m_SDL_Window) & SDL_WINDOW_MINIMIZED;
}


void Window_SDL2::turnIntoMenu()
{
    SDL_SysWMinfo info;
    SDL_VERSION(&info.version);
    
    if(!SDL_GetWindowWMInfo(m_SDL_Window, &info))
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


bool Window_SDL2::init()
{
    return false;
}
    

void Window_SDL2::cleanup()
{
    SDL_Quit();
}
    
    
void Window_SDL2::trackMouse(bool on)
{
    tracking_mouse = on;
}


bool Window_SDL2::shouldQuit()
{
    return should_quit;
}


/** @brief Get an Window_SDL2 that recieved the event or an alternative value if the window was nullptr. */
Window_SDL2* get_event_window_or_alternative(unsigned int window_id, Window_SDL2* alternative = nullptr)
{
    auto window = (Window_SDL2*) SDL_GetWindowData(SDL_GetWindowFromID(window_id), "window");
    return window ? window : alternative;
}

        
void Window_SDL2::processEvents()
{    
    static SDL_Event event;
    static Window_SDL2* window = nullptr;
    while(SDL_PollEvent(&event))
    {
        switch(event.type)
        {            
            /* Keyboard */
            case SDL_KEYDOWN:
            {
                int x = 0; 
                int y = 0;
                
                SDL_GetMouseState(&x, &y);
                
                window = get_event_window_or_alternative(event.key.windowID, window);
#ifdef DEBUG
                assert(window != nullptr);
#endif//DEBUG
                window->makeCurrent();
                
                window->initKeyPressEvent(
                    x, y,
                    event.key.keysym.scancode, 
                    pressed_mouse_buttons, 
                    event.key.keysym.mod
                );
                break;
            }
            
            case SDL_KEYUP:
            {
                int x = 0; 
                int y = 0;
                
                SDL_GetMouseState(&x, &y);
                
                window = get_event_window_or_alternative(event.key.windowID, window);
#ifdef DEBUG
                assert(window != nullptr);
#endif//DEBUG
                window->makeCurrent();
                
                window->initKeyReleaseEvent(
                    x, y,
                    event.key.keysym.scancode, 
                    pressed_mouse_buttons, 
                    event.key.keysym.mod
                );
                break;
            }
            
            case SDL_TEXTEDITING:
            {
                break;
            }
            
            case SDL_TEXTINPUT:
            {
                window = get_event_window_or_alternative(event.text.windowID, window);
#ifdef DEBUG
                assert(window != nullptr);
#endif//DEBUG
                window->makeCurrent();
                
                std::string chr;
                for(int i=0; i<SDL_TEXTINPUTEVENT_TEXT_SIZE && event.text.text[i]; i++)
                {
                    chr.push_back(event.text.text[i]);
                }
                window->initTextInputEvent(chr);
                break;
            }
            
            /* Mouse */
            case SDL_MOUSEMOTION:
            {
                window = get_event_window_or_alternative(event.motion.windowID, window);
#ifdef DEBUG
                assert(window != nullptr);
#endif//DEBUG
                window->makeCurrent();
                
                window->initMouseMoveEvent(event.motion.x, event.motion.y, pressed_mouse_buttons, SDL_GetModState());
                break;
            }
            
            case SDL_MOUSEBUTTONDOWN:
            {
                window = get_event_window_or_alternative(event.button.windowID, window);                
#ifdef DEBUG
                assert(window != nullptr);
#endif//DEBUG
                window->makeCurrent();
                                
                if(event.button.button == SDL_BUTTON_LEFT)
                    pressed_mouse_buttons |= Mouse::Button::Left;
                else if(event.button.button == SDL_BUTTON_MIDDLE)
                    pressed_mouse_buttons |= Mouse::Button::Middle;
                else if(event.button.button == SDL_BUTTON_RIGHT)
                    pressed_mouse_buttons |= Mouse::Button::Right;
                window->initMousePressEvent(event.button.x, event.button.y, pressed_mouse_buttons, SDL_GetModState());
                break;
            }
            
            case SDL_MOUSEBUTTONUP:
            {
                window = get_event_window_or_alternative(event.button.windowID, window);
#ifdef DEBUG
                assert(window != nullptr);
#endif//DEBUG
                window->makeCurrent();
                
                if(event.button.button == SDL_BUTTON_LEFT)
                    pressed_mouse_buttons &= ~Mouse::Button::Left;
                else if(event.button.button == ~SDL_BUTTON_MIDDLE)
                    pressed_mouse_buttons &= ~Mouse::Button::Middle;
                else if(event.button.button == SDL_BUTTON_RIGHT)
                    pressed_mouse_buttons &= ~Mouse::Button::Right;
                window->initMouseReleaseEvent(event.button.x, event.button.y, pressed_mouse_buttons, SDL_GetModState());
                break;
            }
            
            case SDL_MOUSEWHEEL:
            {
                int x = 0; 
                int y = 0;
            
                SDL_GetMouseState(&x, &y);

                window = get_event_window_or_alternative(event.wheel.windowID, window);
#ifdef DEBUG
                assert(window != nullptr);
#endif//DEBUG
                window->makeCurrent();
                
                window->initMouseWheelEvent(x, y, event.wheel.x, event.wheel.y, pressed_mouse_buttons, SDL_GetModState());
                break;
            }
            
            /* Clipboard */
            case SDL_CLIPBOARDUPDATE:
            {
                break;
            }
            
            /* Drag and drop */
            case SDL_DROPFILE:
            {
                cout << "dropfile\n";
                break;
            }
            
            /* Window */
            case SDL_WINDOWEVENT:
            {
                window = get_event_window_or_alternative(event.window.windowID, window);
                
                if(event.window.event == SDL_WINDOWEVENT_FOCUS_GAINED)
                {
                }
                else if(event.window.event == SDL_WINDOWEVENT_FOCUS_LOST)
                {
                }
                else if(event.window.event == SDL_WINDOWEVENT_RESIZED)
                {
                    int w = event.window.data1;
                    int h = event.window.data2;
                    window->initResizeEvent(w, h);
                }
                else if(event.window.event == SDL_WINDOWEVENT_CLOSE)
                {
                    window->hide();
                }
                break;
            }
            
            
            case SDL_SYSWMEVENT:
            {
                break;
            }
        }
    }
}
    
}//namespace r64fx
