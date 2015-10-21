#if defined R64FX_USE_SDL2
#include "WindowSDL2.hpp"
#define WindowImpl WindowSDL2

#elif defined R64FX_USE_X11
#include "WindowX11.hpp"
#define WindowImpl WindowX11

#endif

#include <vector>

using namespace std;


namespace r64fx{

vector<Window*> g_all_windows;

Window::Window(Window::Type type)
: m_type(type)
{

}


Window::Window(const Window&)
{

}


Window::~Window()
{
    auto it = g_all_windows.begin();
    while(it != g_all_windows.end() && *it != this) it++;
    g_all_windows.erase(it);

    if(g_all_windows.empty())
    {
        WindowImpl::cleanup();
    }
}


Window* Window::newWindow(int width, int height, std::string title, Window::Type type)
{
    auto window =  WindowImpl::newWindow(width, height, title, type);

    g_all_windows.push_back(window);
    return window;
}


void Window::show()
{
    auto impl = (WindowImpl*) this;
    impl->show();
}


void Window::hide()
{
    auto impl = (WindowImpl*) this;
    impl->hide();
}


void Window::resize(int width, int height)
{
    auto impl = (WindowImpl*) this;
    impl->resize(width, height);
}


void Window::repaint()
{
    auto impl = (WindowImpl*) this;
    impl->repaint();
}


Image* Window::image() const
{
    auto impl = (WindowImpl*) this;
    return impl->image();
}


void Window::setTitle(std::string title)
{
    auto impl = (WindowImpl*) this;
    impl->setTitle(title);
}


std::string Window::title() const
{
    auto impl = (WindowImpl*) this;
    return impl->title();
}


void Window::processSomeEvents(Window::Events* events)
{
    WindowImpl::processSomeEvents(events);
}

};