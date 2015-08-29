#ifndef R64FX_WINDOW_SDL2_HPP
#define R64FX_WINDOW_SDL2_HPP

#include "Window.hpp"
#include <SDL2/SDL.h>

namespace r64fx{

class WindowSDL2 : public Window{
    SDL_Window* m_SDL_Window = nullptr;

    WindowSDL2(Window::Type type, SDL_Window* sdl_window);

public:
    virtual ~WindowSDL2();

    static Window* newWindow(int width, int height, std::string title, Window::Type type = Window::Type::Normal);

    static void cleanup();

    void show();

    void hide();

    void resize(int width, int height);

    void updateSurface();

    void setTitle(std::string title);

    std::string title() const;

    static void processSomeEvents(Window::Events* events);
};

}//namespace r64fx

#endif//R64FX_WINDOW_SDL2_HPP