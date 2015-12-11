#include "Program.hpp"
#include "Widget.hpp"
#include "Window.hpp"
#include "ReconfContext.hpp"
#include "Painter.hpp"
#include "Mouse.hpp"
#include "KeyEvent.hpp"
#include "WidgetFlags.hpp"

#include <iostream>
#include <vector>
#include <unistd.h>

using namespace std;

namespace r64fx{
    
namespace{
    Program* program_singleton_instance = nullptr;
    Window::Events events;
    MouseButton g_pressed_buttons;
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
        g_pressed_buttons |= MouseButton(button);
        MousePressEvent me(x, y, MouseButton(button));
        program_singleton_instance->mousePressEvent(window, &me);
    };

    events.mouse_release = [](Window* window, float x, float y, unsigned int button)
    {
        g_pressed_buttons &= ~MouseButton(button);
        MouseReleaseEvent me(x, y, MouseButton(button));
        program_singleton_instance->mouseReleaseEvent(window, &me);
    };

    events.mouse_move = [](Window* window, float x, float y)
    {
        MouseMoveEvent me(x, y, g_pressed_buttons);
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

    events.reconfigure = [](Window* window, int old_w, int old_h, int new_w, int new_h)
    {
        program_singleton_instance->reconfigure(window);
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


void Program::reconfigure(Window* window)
{
    window->painter()->reconfigure();
    window->widget()->setSize({window->width(), window->height()});
    window->widget()->update();
}


void Program::mousePressEvent(Window* window, MousePressEvent* event)
{
    window->widget()->mousePressEvent(event);
}


void Program::mouseReleaseEvent(Window* window, MouseReleaseEvent* event)
{
    window->widget()->mouseReleaseEvent(event);
}


void Program::mouseMoveEvent(Window* window, MouseMoveEvent* event)
{
    window->widget()->mouseMoveEvent(event);
}


void Program::keyPressEvent(Window* window, KeyEvent* event)
{
    window->widget()->keyPressEvent(event);
}


void Program::keyReleaseEvent(Window* window, KeyEvent* event)
{
    window->widget()->keyReleaseEvent(event);
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
    ReconfContext ctx(window->painter());

    auto widget = window->widget();
    if(widget)
    {
        if(widget->m_flags & R64FX_WIDGET_UPDATE_FLAGS)
        {
            if(widget->m_flags & R64FX_WIDGET_WANTS_UPDATE)
            {
                widget->reconfigure(&ctx);
                window->painter()->repaint();
            }
            else
            {
                widget->reconfigureChildren(&ctx);
                if(!ctx.rects.empty())
                {
                    for(auto &rect : ctx.rects)
                    {
                        rect = intersection(rect, {0, 0, window->width(), window->height()});
                    }

                    window->painter()->repaint(
                        ctx.rects.data(), ctx.rects.size()
                    );
                }
            }
        }
    }
}

}//namespace r64fx