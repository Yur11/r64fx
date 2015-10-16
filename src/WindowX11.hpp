#ifndef R64FX_WINDOW_X11_HPP
#define R64FX_WINDOW_X11_HPP

#include "Window.hpp"
#include <X11/Xlib.h>
#ifdef R64FX_USE_GL
#include <GL/glx.h>
#endif//R64FX_USE_GL

namespace r64fx{

class WindowX11 : public Window{
    ::Window m_xwindow;
    XWindowAttributes m_attrs;
    XImage* m_ximage = nullptr;
    Image* m_image = nullptr;
    XGCValues m_xgc_values;
    GC m_gc;
    std::string m_title = "";

#ifdef R64FX_USE_GL
    GLXContext m_gl_context = 0;
#endif//R64FX_USE_GL

    WindowX11(Window::Type type);

public:
    virtual ~WindowX11();

    static Window* newWindow(int width, int height, std::string title, Window::Type type = Window::Type::Normal);

    static void cleanup();

    void show();

    void hide();

    void resize(int width, int height);

    void makeCurrent();

    void repaint();

    Image* image() const;

    void setTitle(std::string title);

    std::string title() const;

    static void processSomeEvents(Window::Events* events);

    inline ::Window xWindow() const { return m_xwindow; }

    int width() const;

    int height() const;

private:
    void updateAttrs();

    XWindowAttributes* attrs();

    void initNormalWindow();

    void resizeImage();

    void destroyImage();

    void processExposeEvent();

#ifdef R64FX_USE_GL
    void initGLWindow();

#endif//R64FX_USE_GL
};

}//namespace r64fx

#endif//R64FX_WINDOW_X11_HPP