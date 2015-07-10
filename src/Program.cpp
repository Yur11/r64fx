#include "Program.hpp"
#include "ProgramImplEventIface.hpp"
#include "Window.hpp"

#include "gui_implementation_iface.hpp"

#include "Widget.hpp"
#include "MouseEvent.hpp"
#include "KeyEvent.hpp"
#include "ResizeEvent.hpp"

#include <iostream>
#include <vector>
#include <unistd.h>

using namespace std;

namespace r64fx{
    
namespace{
    Program* program_singleton_instance = nullptr;
    ProgramImplEventIface* impl_iface = nullptr;
    std::vector<Window*> all_windows;
}
    
Program::Program(int argc, char* argv[])
{
    if(program_singleton_instance != nullptr)
    {
        cerr << "Refusing to create a second program instance!\n";
        abort();
    }

    if(!Impl::init())
    {
        abort();
    }
    
    program_singleton_instance = this;
    impl_iface = new ProgramImplEventIface;
}


Program::~Program()
{
    Impl::cleanup();
    delete impl_iface;
}


int Program::exec()
{
    setup();
    
    while(m_should_be_running)
    {
        Impl::process_some_events(impl_iface);
        usleep(100);
    }
    
    cleanup();
    
    return 0;
}


void Program::quit()
{
    m_should_be_running = false;
}


Program* Program::instance()
{
    return program_singleton_instance;
}


void Program::mousePressEvent(Widget* widget, MouseEvent* event)
{
    widget->mousePressEvent(event);
}


void Program::mouseReleaseEvent(Widget* widget, MouseEvent* event)
{
    widget->mouseReleaseEvent(event);
}


void Program::mouseMoveEvent(Widget* widget, MouseEvent* event)
{
    widget->mouseMoveEvent(event);
}


void Program::keyPressEvent(Widget* widget, KeyEvent* event)
{
    widget->keyPressEvent(event);
}


void Program::keyReleaseEvent(Widget* widget, KeyEvent* event)
{
    widget->keyReleaseEvent(event);
}


void Program::resizeEvent(Widget* widget, ResizeEvent* event)
{
    widget->resizeEvent(event);
}


void Program::closeEvent(Widget* widget)
{

}


void Program::setup()
{
    
}


void Program::cleanup()
{
    
}



/* ======================
 * Window implementation
 * ====================== */

Window::Window(Widget* root)
: m_root_widget(root)
{

}


Window::~Window()
{
}


Window* Window::createNew(Widget* root, PainterType pt, WindowType wt, const char* title)
{
    Window* window = new Window(root);

    if(pt == PainterType::GL3)
    {
        if(Impl::init_window_gl3(window))
        {
            cerr << "Failed to create GL3 window!\n";
            delete window;
            return nullptr;
        }
    }
    else if(pt == PainterType::Normal)
    {
        if(Impl::init_window_normal(window))
        {
            cerr << "Failed to create a simple window!\n";
            delete window;
            return nullptr;
        }
    }
    else if(pt == PainterType::BestSupported)
    {
        if(!Impl::init_window_gl3(window) && !Impl::init_window_normal(window))
        {
            cerr << "Failed to create a window!\n";
            delete window;
            return nullptr;
        }
    }

    if(wt == WindowType::Menu)
    {
        Impl::turn_into_menu(window);
    }

    if(title)
    {
        window->setTitle(title);
    }

    all_windows.push_back(window);

    return window;
}

void Window::destroy(Window* window)
{
    Impl::cleanup_window(window);
    auto it = all_windows.begin();
    while(it != all_windows.end())
    {
        if(*it == window)
        {
            all_windows.erase(it);
            delete window;
            return;
        }
        it++;
    }
}


void Window::show()
{
    Impl::show_window(this);
}


void Window::hide()
{
    Impl::hide_window(this);
}


void Window::resize(int w, int h)
{
    Impl::resize_window(this, w, h);
}


void Window::setTitle(const char* title)
{
    Impl::set_window_title(this, title);
}


const char* Window::title()
{
    return Impl::window_title(this);
}


/* ======================================
 * Program Implementation Event Interface
 * ======================================*/

void ProgramImplEventIface::initMousePressEvent(WindowImplIface* window_iface, float x, float y, unsigned int button)
{
    auto window = (Window*) window_iface;
    MouseEvent event(x, y, button);
    program_singleton_instance->mousePressEvent(window->rootWidget(), &event);
}


void ProgramImplEventIface::initMouseReleaseEvent(WindowImplIface* window_iface, float x, float y, unsigned int button)
{
    auto window = (Window*) window_iface;
    MouseEvent event(x, y, button);
    program_singleton_instance->mouseReleaseEvent(window->rootWidget(), &event);
}


void ProgramImplEventIface::initMouseMoveEvent(WindowImplIface* window_iface, float x, float y, unsigned int buttons)
{
    auto window = (Window*) window_iface;
    MouseEvent event(x, y, buttons);
    program_singleton_instance->mouseMoveEvent(window->rootWidget(), &event);
}


void ProgramImplEventIface::initKeyPressEvent(WindowImplIface* window_iface, unsigned int key)
{
    auto window = (Window*) window_iface;
    KeyEvent event(key);
    program_singleton_instance->keyPressEvent(window->rootWidget(), &event);
}


void ProgramImplEventIface::initKeyReleaseEvent(WindowImplIface* window_iface, unsigned int key)
{
    auto window = (Window*) window_iface;
    KeyEvent event(key);
    program_singleton_instance->keyReleaseEvent(window->rootWidget(), &event);
}


void ProgramImplEventIface::initResizeEvent(WindowImplIface* window_iface, int w, int h)
{
    auto window = (Window*) window_iface;
    ResizeEvent event(window->rootWidget()->size(), {w, h});
    program_singleton_instance->resizeEvent(window->rootWidget(), &event);
}


void ProgramImplEventIface::initCloseEvent(WindowImplIface* window_iface)
{
    auto window = (Window*) window_iface;
    program_singleton_instance->closeEvent(window->rootWidget());
}

}//namespace r64fx