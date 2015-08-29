#include "WindowSDL2.hpp"
#include "Mouse.hpp"
#include <iostream>

using namespace std;

namespace{

bool g_sdl_is_init = false;

}//namespace

namespace r64fx{


WindowSDL2::WindowSDL2(Window::Type type, SDL_Window* sdl_window)
: Window(type)
, m_SDL_Window(sdl_window)
{

}


WindowSDL2::~WindowSDL2()
{
    if(m_SDL_Window)
    {
        SDL_DestroyWindow(m_SDL_Window);
        m_SDL_Window = nullptr;
    }
}


Window* WindowSDL2::newWindow(int width, int height, std::string title, Window::Type type)
{
    if(!g_sdl_is_init)
    {
        int code = SDL_Init(SDL_INIT_VIDEO);
        if(code != 0)
        {
            cerr << "SDL_Init failed with code " << code << "\n";
            cerr << SDL_GetError() << "\n";
            return nullptr;
        }
    }

    int extra_flags;
    if(type == Window::Type::GL3)
    {
        extra_flags = SDL_WINDOW_OPENGL;
    }

    auto sdl_window = SDL_CreateWindow(
        title.c_str(),
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        width, height,
        SDL_WINDOW_HIDDEN | SDL_WINDOW_RESIZABLE | extra_flags
    );

    if(!sdl_window)
    {
        cerr << "SDL_CreateWindow failed!\n";
        cerr << SDL_GetError() << "\n";
        SDL_Quit();
        return nullptr;
    }

    auto window = new WindowSDL2(type, sdl_window);
    SDL_SetWindowData(sdl_window, "window", window);
    return window;
}


void WindowSDL2::cleanup()
{
    SDL_Quit();
}


void WindowSDL2::show()
{
    SDL_ShowWindow(m_SDL_Window);
}


void WindowSDL2::hide()
{
    SDL_HideWindow(m_SDL_Window);
}


void WindowSDL2::resize(int width, int height)
{
    SDL_SetWindowSize(m_SDL_Window, width, height);
}


void WindowSDL2::updateSurface()
{
    SDL_UpdateWindowSurface(m_SDL_Window);
}


void WindowSDL2::setTitle(std::string title)
{
    SDL_SetWindowTitle(m_SDL_Window, title.c_str());
}


std::string WindowSDL2::title() const
{
    return SDL_GetWindowTitle(m_SDL_Window);
}


namespace{
    Window* get_window_by_id(unsigned int window_id)
    {
        if(!window_id)
            return nullptr;
        return (Window*) SDL_GetWindowData(SDL_GetWindowFromID(window_id), "window");
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


    void process_window_event(Window::Events* events, Window* window, SDL_WindowEvent* windowevent)
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
                events->resize(window, windowevent->data1, windowevent->data2);
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
                events->close(window);
                break;
            }
            default:
                break;
        }
    }
};


void WindowSDL2::processSomeEvents(Window::Events* events)
{
    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
        switch(event.type)
        {
            case SDL_MOUSEBUTTONDOWN:
            {
                events->mouse_press(
                    get_window_by_id(event.button.windowID),
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
                    get_window_by_id(event.button.windowID),
                    event.button.x, event.button.y,
                    get_event_button(event.button)
                );
                break;
            }

            case SDL_MOUSEMOTION:
            {
                events->mouse_move(
                    get_window_by_id(event.motion.windowID),
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
                        get_window_by_id(event.key.windowID), event.key.keysym.scancode
                    );
                }
                auto window = SDL_GetKeyboardFocus();
                cout << "focus: " << window << "\n";
                break;
            }

            case SDL_KEYUP:
            {
                events->key_release(
                    get_window_by_id(event.key.windowID), event.key.keysym.scancode
                );
                auto window = SDL_GetKeyboardFocus();
                cout << "focus: " << window << "\n";
                break;
            }

            case SDL_WINDOWEVENT:
            {
                process_window_event(
                    events,
                    get_window_by_id(event.window.windowID),
                    &event.window
                );
                break;
            }
        }
    }
}

}//namespace r64fx