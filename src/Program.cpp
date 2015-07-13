#include "Program.hpp"

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
    Impl::Events events;
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

    events.mouse_press = [](Impl::WindowData_t* wd, float x, float y, unsigned int buttons)
    {
        MouseEvent me(x, y, buttons);
        program_singleton_instance->mousePressEvent((Widget*)wd, &me);
    };

    events.mouse_release = [](Impl::WindowData_t* wd, float x, float y, unsigned int buttons)
    {
        MouseEvent me(x, y, buttons);
        program_singleton_instance->mouseReleaseEvent((Widget*)wd, &me);
    };

    events.mouse_move = [](Impl::WindowData_t* wd, float x, float y, unsigned int buttons)
    {
        MouseEvent me(x, y, buttons);
        program_singleton_instance->mouseMoveEvent((Widget*)wd, &me);
    };

    events.key_press = [](Impl::WindowData_t* wd, int key)
    {
        KeyEvent ke(key);
        program_singleton_instance->keyPressEvent((Widget*)wd, &ke);
    };

    events.key_release = [](Impl::WindowData_t* wd, int key)
    {
        KeyEvent ke(key);
        program_singleton_instance->keyReleaseEvent((Widget*)wd, &ke);
    };

    events.resize = [](Impl::WindowData_t* wd, int w, int h)
    {
        ResizeEvent re(w, h);
        program_singleton_instance->resizeEvent((Widget*)wd, &re);
    };

    events.close = [](Impl::WindowData_t* wd)
    {
        program_singleton_instance->closeEvent((Widget*)wd);
    };
}


Program::~Program()
{
    Impl::cleanup();
}


int Program::exec()
{
    setup();
    
    while(m_should_be_running)
    {
        Impl::process_some_events(&events);
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

}//namespace r64fx