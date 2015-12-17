#include "Program.hpp"
#include "Widget.hpp"
#include "Window.hpp"
#include "Mouse.hpp"
#include "KeyEvent.hpp"
#include "WidgetFlags.hpp"

#include <iostream>
#include <vector>
#include <unistd.h>

using namespace std;

namespace r64fx{
    
namespace{
    Program*         program_singleton_instance = nullptr;
    Window::Events   events;
    MouseButton      pressed_buttons;
}


Program::Program(int argc, char* argv[])
{
    if(program_singleton_instance != nullptr)
    {
        cerr << "Refusing to create a second program instance!\n";
        abort();
    }

    program_singleton_instance = this;

    events.mouse_press = [](Window* window, float x, float y, unsigned int button)
    {
        pressed_buttons |= MouseButton(button);
        MousePressEvent me(x, y, MouseButton(button));
        program_singleton_instance->mousePressEvent(window, &me);
    };

    events.mouse_release = [](Window* window, float x, float y, unsigned int button)
    {
        pressed_buttons &= ~MouseButton(button);
        MouseReleaseEvent me(x, y, MouseButton(button));
        program_singleton_instance->mouseReleaseEvent(window, &me);
    };

    events.mouse_move = [](Window* window, float x, float y)
    {
        MouseMoveEvent me(x, y, pressed_buttons);
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

    events.resize = [](Window* window, int old_w, int old_h, int new_w, int new_h)
    {
        program_singleton_instance->resizeEvent(window);
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
        Window::forEachWindow([](Window* window, void* data){
            auto p = (Program*) data;
            p->performUpdates(window);
        }, this);
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


void Program::resizeEvent(Window* window)
{
    Widget::processWindowResize(window);
}


void Program::mousePressEvent(Window* window, MousePressEvent* event)
{
    Widget::initMousePressEvent(window, event);
}


void Program::mouseReleaseEvent(Window* window, MouseReleaseEvent* event)
{
    Widget::initMouseReleaseEvent(window, event);
}


void Program::mouseMoveEvent(Window* window, MouseMoveEvent* event)
{
    Widget::initMouseMoveEvent(window, event);
}


void Program::keyPressEvent(Window* window, KeyEvent* event)
{
    Widget::initKeyPressEvent(window, event);
}


void Program::keyReleaseEvent(Window* window, KeyEvent* event)
{
    Widget::initKeyReleaseEvent(window, event);
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


void Program::performUpdates(Window* window)
{
    Widget::initReconf(window);
}

}//namespace r64fx