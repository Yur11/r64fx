#include "Window.hpp"
#include "MouseEvent.hpp"
#include "KeyEvent.hpp"

#include "gui_implementation_iface.hpp"

#include <iostream>

using namespace std;

namespace r64fx{
            
Window::Window(Widget* root)
: m_root_widget(root)
{
    
}


Window* Window::createNew(Widget* root,Type type)
{
    Window* window = new Window(root);
    
    if(type == Window::Type::GL3)
    {
        if(init_window_gl3(window))
        {
            return window;
        }
        else
        {
            cerr << "Failed to create GL3 window!\n";
        }
    }
    else if(type == Window::Type::Normal)
    {
        if(init_window_normal(window))
        {
            return window;
        }
        else
        {
            cerr << "Failed to create a simple window!\n";
        }
    }
    else if(type == Window::Type::BestSupported)
    {
        if(init_window_gl3(window) || init_window_normal(window))
        {
            return window;
        }
        else
        {
            cerr << "Failed to create a simple window!\n";
        }
    }
    
    delete window;
    return nullptr;
}


Window::~Window()
{
}

void Window::show()
{
    show_window(this);
}
    

void Window::hide()
{
    hide_window(this);
}


void Window::mousePressEvent(MouseEvent* event)
{
}

    
void Window::mouseReleaseEvent(MouseEvent* event)
{
}
    
    
void Window::mouseMoveEvent(MouseEvent* event)
{    
}
    
    
void Window::keyPressEvent(KeyEvent* event)
{
}

    
void Window::keyReleaseEvent(KeyEvent* event)
{
}


void Window::closeEvent()
{
    
}
    
}//namespace r64fx
