#ifndef R64FX_WINDOW_X11_HPP
#define R64FX_WINDOW_X11_HPP

#include "Window.hpp"

namespace r64fx{

class WindowX11 : public Window{
    void* m_private = nullptr;

    WindowX11(int width, int height, std::string title, Window::Type type);

public:
    virtual ~WindowX11();

    static Window* newInstance(int width, int height, std::string title, Window::Type type);

    static void cleanup();

    void show();

    void hide();

    void resize(int width, int height);

    int width() const;

    int height() const;

#ifdef R64FX_USE_GL
    void makeCurrent();
#endif//R64FX_USE_GL

    void repaint();

    Image* image() const;

    void setTitle(std::string title);

    std::string title() const;

    static void processSomeEvents(Window::Events* events);
};

}//namespace r64fx

#endif//R64FX_WINDOW_X11_HPP