#include "Program.hpp"
#include "Widget.hpp"
#include "Window.hpp"
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
    Window::Events events;
}
    
Program::Program(int argc, char* argv[])
{
    if(program_singleton_instance != nullptr)
    {
        cerr << "Refusing to create a second program instance!\n";
        abort();
    }

    program_singleton_instance = this;

    events.mouse_press = [](Window* window, float x, float y, unsigned int buttons)
    {
        MouseEvent me(x, y, buttons);
        program_singleton_instance->mousePressEvent(window, &me);
    };

    events.mouse_release = [](Window* window, float x, float y, unsigned int buttons)
    {
        MouseEvent me(x, y, buttons);
        program_singleton_instance->mouseReleaseEvent(window, &me);
    };

    events.mouse_move = [](Window* window, float x, float y, unsigned int buttons)
    {
        MouseEvent me(x, y, buttons);
        program_singleton_instance->mouseMoveEvent(window, &me);
    };

    events.key_press = [](Window* window, int key)
    {
        KeyEvent ke(key);
        program_singleton_instance->keyPressEvent(window, &ke);
    };

    events.key_release = [](Window* window, int key)
    {
        KeyEvent ke(key);
        program_singleton_instance->keyReleaseEvent(window, &ke);
    };

    events.resize = [](Window* window, int w, int h)
    {
        ResizeEvent re(w, h);
        program_singleton_instance->resizeEvent(window, &re);
    };

    events.close = [](Window* window)
    {
        program_singleton_instance->closeEvent(window);
    };
}


Program::~Program()
{
}


int Program::exec()
{
    setup();
    
    while(m_should_be_running)
    {
        Window::processSomeEvents(&events);
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


void Program::mousePressEvent(Window* window, MouseEvent* event)
{
    if(window)
    {
        window->widget()->mousePressEvent(event);
    }
}


void Program::mouseReleaseEvent(Window* window, MouseEvent* event)
{
    if(window)
    {
        window->widget()->mouseReleaseEvent(event);
    }
}


void Program::mouseMoveEvent(Window* window, MouseEvent* event)
{
    if(window)
    {
        window->widget()->mouseMoveEvent(event);
    }
}


void Program::keyPressEvent(Window* window, KeyEvent* event)
{
    cout << "key press: " << window << "\n";
    if(window)
    {
        window->widget()->keyPressEvent(event);
    }
}


void Program::keyReleaseEvent(Window* window, KeyEvent* event)
{
    cout << "key release: " << window << "\n";
    if(window)
    {
        window->widget()->keyReleaseEvent(event);
    }
}


void Program::resizeEvent(Window* window, ResizeEvent* event)
{
    window->widget()->resizeEvent(event);
}


void Program::closeEvent(Window* window)
{

}


void Program::setup()
{

}


void Program::cleanup()
{

}

}//namespace r64fx