#if defined R64FX_USE_SDL2
#include "WindowSDL2.hpp"
#define WindowImpl WindowSDL2

#elif defined R64FX_USE_X11
#include "WindowX11.hpp"
#define WindowImpl WindowX11

#endif

#include "Painter.hpp"

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

}


Window* Window::newInstance(int width, int height, std::string title, Window::Type type)
{
    auto window =  WindowImpl::newInstance(width, height, title, type);
    window->setPainter(Painter::newInstance(window));
    g_all_windows.push_back(window);
    return window;
}


void Window::destroyInstance(Window* window)
{
    auto it = g_all_windows.begin();
    while(it != g_all_windows.end() && *it != window) it++;
    g_all_windows.erase(it);

    Painter::destroyInstance(window->painter());
    delete window;

    if(g_all_windows.empty())
    {
        WindowImpl::cleanup();
    }
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


int Window::width() const
{
    auto impl = (WindowImpl*) this;
    return impl->width();
}


int Window::height() const
{
    auto impl = (WindowImpl*) this;
    return impl->height();
}


#ifdef R64FX_USE_GL
void Window::makeCurrent()
{
    auto impl = (WindowImpl*) this;
    impl->makeCurrent();
}
#endif//R64FX_USE_GL


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


void Window::getComponentIndices(int* r, int* g, int* b, int* a)
{
    auto impl = (WindowImpl*) this;
    impl->getComponentIndices(r, g, b, a);
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