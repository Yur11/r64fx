#include "WindowX11.hpp"
#include <vector>
#include <iostream>

using namespace std;

namespace r64fx{

extern vector<Window*> g_all_windows;

namespace{

Display*   g_display = nullptr;
int        g_screen;
Atom       g_WM_PROTOCOLS;
Atom       g_WM_DELETE_WINDOW;

WindowX11* get_window_from_xwindow(::Window xwindow)
{
    for(unsigned int i=0; i<g_all_windows.size(); i++)
    {
        WindowX11* window = (WindowX11*) g_all_windows[i];
        if(window->xWindow() == xwindow)
        {
            return window;
        }
    }
    return nullptr;
}

}//namespace


WindowX11::WindowX11(Window::Type type)
: Window(type)
{
    m_xwindow = XCreateSimpleWindow(
        g_display,
        RootWindow(g_display, g_screen),
        0, 0, 640, 480, 0,
        BlackPixel(g_display, g_screen), WhitePixel(g_display, g_screen)
    );

    XSetWMProtocols(g_display, m_xwindow, &g_WM_DELETE_WINDOW, 1);
    XSelectInput(g_display, m_xwindow, KeyPressMask);
    XGetWindowAttributes(g_display, m_xwindow, &m_attrs);

//     m_gc = XCreateGC(g_display, m_xwindow, GCGraphicsExposures, &(m_xgc_values));

    XFlush(g_display);
}


WindowX11::~WindowX11()
{

}


Window* WindowX11::newWindow(int width, int height, std::string title, Window::Type type)
{
    if(!g_display)
    {
        g_display = XOpenDisplay(nullptr);
        if(!g_display)
        {
            cerr << "Failed to open display!\n";
            return nullptr;
        }

        g_WM_PROTOCOLS       = XInternAtom(g_display, "WM_PROTOCOLS", true);
        g_WM_DELETE_WINDOW   = XInternAtom(g_display, "WM_DELETE_WINDOW", true);
    }

    return new WindowX11(type);
}


void WindowX11::cleanup()
{
    XCloseDisplay(g_display);
}


void WindowX11::show()
{
    XMapWindow(g_display, m_xwindow);
    XFlush(g_display);
}


void WindowX11::hide()
{
    XUnmapWindow(g_display, m_xwindow);
    XFlush(g_display);
}


void WindowX11::resize(int width, int height)
{
    XResizeWindow(g_display, m_xwindow, width, height);
}


void WindowX11::updateSurface()
{

}


void WindowX11::setTitle(std::string title)
{

}


std::string WindowX11::title() const
{

}


void WindowX11::processSomeEvents(Window::Events* events)
{
    while(XPending(g_display))
    {

        XEvent xevent;
        XNextEvent(g_display, &xevent);

        WindowX11* window = get_window_from_xwindow(xevent.xany.window);
        if(!window)
            continue;

        switch(xevent.type)
        {
            case KeyPress:
            {
                events->key_press(window, XLookupKeysym(&xevent.xkey, 0));
                break;
            }

            case KeyRelease:
            {
                events->key_release(window, XLookupKeysym(&xevent.xkey, 0));
                break;
            }

            case ClientMessage:
            {
                if(xevent.xclient.message_type == g_WM_PROTOCOLS)
                {
                    events->close(window);
                }
                break;
            }

            default:
                break;
        }
    }
}

}//namespace r64fx