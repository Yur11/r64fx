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
    Program*         program_singleton_instance = nullptr;
    Window::Events   events;
    MouseButton      pressed_buttons;
    ReconfContext*   reconf_ctx;
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

    reconf_ctx = new ReconfContext(16);
}


Program::~Program()
{
    delete[] reconf_ctx;
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
    window->painter()->reconfigure();
    window->widget()->setSize({window->width(), window->height()});
    window->widget()->update();
}


void Program::mousePressEvent(Window* window, MousePressEvent* event)
{
    auto grabber = Widget::mouseGrabber();
    if(grabber)
    {
        event->setPosition(
            event->position() - grabber->toRootCoords(Point<int>(0, 0))
        );
        grabber->mousePressEvent(event);

        if(grabber->m_flags & R64FX_WIDGET_UPDATE_FLAGS)
        {
            auto widget = grabber->parent();
            while(widget)
            {
                widget->m_flags |= R64FX_CHILD_WANTS_UPDATE;
                widget = widget->parent();
            }
        }
    }
    else
    {
        window->widget()->mousePressEvent(event);
    }
}


void Program::mouseReleaseEvent(Window* window, MouseReleaseEvent* event)
{
    auto grabber = Widget::mouseGrabber();
    if(grabber)
    {
        event->setPosition(
            event->position() - grabber->toRootCoords(Point<int>(0, 0))
        );
        grabber->mouseReleaseEvent(event);
    }
    else
    {
        window->widget()->mouseReleaseEvent(event);
    }
}


void Program::mouseMoveEvent(Window* window, MouseMoveEvent* event)
{
    auto grabber = Widget::mouseGrabber();
    if(grabber)
    {
        event->setPosition(
            event->position() - grabber->toRootCoords(Point<int>(0, 0))
        );
        grabber->mouseMoveEvent(event);
    }
    else
    {
        window->widget()->mouseMoveEvent(event);
    }
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
    auto widget = window->widget();
    if(widget)
    {
        if(widget->m_flags & R64FX_WIDGET_UPDATE_FLAGS)
        {
            reconf_ctx->clearRects();
            reconf_ctx->setPainter(window->painter());
            reconf_ctx->setVisibleRect({0, 0, widget->width(), widget->height()});
            reconf_ctx->got_rect = false;

            if(widget->m_flags & R64FX_WIDGET_WANTS_UPDATE)
            {
                widget->reconfigure(reconf_ctx);
                window->painter()->repaint();
            }
            else
            {
                widget->reconfigureChildren(reconf_ctx);
                if(reconf_ctx->num_rects > 0)
                {
                    for(int i=0; i<reconf_ctx->num_rects; i++)
                    {
                        auto rect = reconf_ctx->rects[i];
                        rect = intersection(rect, {0, 0, window->width(), window->height()});
                    }

                    window->painter()->repaint(
                        reconf_ctx->rects,
                        reconf_ctx->num_rects
                    );
                }
            }
        }
    }
}

}//namespace r64fx