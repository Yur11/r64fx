#ifndef R64FX_WINDOW_X11_HPP
#define R64FX_WINDOW_X11_HPP

#include "Window.hpp"
#include <X11/Xlib.h>

namespace r64fx{

class WindowX11 : public Window{
    ::Window m_xwindow;
    XWindowAttributes m_attrs;
    XImage* m_image = nullptr;
    XGCValues m_xgc_values;
    GC m_gc;

    WindowX11(Window::Type type);

public:
    virtual ~WindowX11();

    static Window* newWindow(int width, int height, std::string title, Window::Type type = Window::Type::Normal);

    static void cleanup();

    void show();

    void hide();

    void resize(int width, int height);

    void updateSurface();

    void setTitle(std::string title);

    std::string title() const;

    static void processSomeEvents(Window::Events* events);

    inline ::Window xWindow() const { return m_xwindow; }
};

}//namespace r64fx

#endif//R64FX_WINDOW_X11_HPP