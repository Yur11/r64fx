#include "Widget.hpp"
#include "Window.hpp"
#include "Mouse.hpp"
#include "KeyEvent.hpp"
#include "ReconfContext.hpp"
#include "Painter.hpp"
#include "Program.hpp"

#ifdef R64FX_DEBUG
#include <iostream>
#include <assert.h>

using namespace std;
#endif//R64FX_DEBUG

#include "WidgetFlags.hpp"

namespace r64fx{

namespace{
    void set_bits(unsigned long &flags, const bool yes, unsigned long mask)
    {
        if(yes)
            flags |= mask;
        else
            flags &= ~mask;
    }

    /* Widget that currently grabs mouse input. */
    Widget* g_mouse_grabber   = nullptr;

    /* Maximum number of individual rectangles
     * that can be repainted after reconf. cycle. */
    constexpr int max_rects = 16;

    /* Collection of data attached to the window.
     * We should be able to cast back and forth
     * between WindowWidgetContext and ReconfContext. */
    struct WindowWidgetContext : Widget::ReconfContext{

        /* Root widget shown in the window that
         * this context is attached to. */
        Widget*  widget = nullptr;

        /* Painter serving the window. */
        Painter* painter = nullptr;

        /* Current visible rect. passed to widget reconf. method. */
        Rect<int> visible_rect;

        /* List of rectangles to be repainted after reconf. cycle. */
        Rect<int> rects[max_rects];

        /* Number of rectangles that must be repainted. */
        int num_rects = 0;

        /* Used in reconf. logic. */
        bool got_rect = false;
    };

}//namespace


Widget::Widget(Widget* parent)
{
    setParent(parent);
}


Widget::~Widget()
{
    
}

    
void Widget::setParent(Widget* parent)
{
    if(isWindow())
    {
#ifdef R64FX_DEBUG
        cerr << "WARNING: Widget::setParent()\nTrying to set parent on a window!\n";
#endif//R64FX_DEBUG
        return;
    }

    if(!parent)
    {
        if(m_parent.widget)
        {
            m_parent.widget->m_children.remove(this);
        }
    }
    else
    {
        parent->m_children.append(this);
    }
    m_parent.widget = parent;
}


Widget* Widget::parent() const
{
    return (Widget*)(isWindow() ? nullptr : m_parent.widget);
}

Window* Widget::parentWindow() const
{
    return (Window*)(isWindow() ? m_parent.window : nullptr);
}


void Widget::add(Widget* child)
{
    if(!child)
    {
#ifdef R64FX_DEBUG
        cerr << "WARNING: Widget::add(nullptr)\n";
#endif//R64FX_DEBUG
        return;
    }
    child->setParent(this);
}


Widget* Widget::root()
{
    if(isWindow())
    {
        return this;
    }
    else if(m_parent.widget)
    {
        return m_parent.widget->root();
    }
    else
    {
        return nullptr;
    }
}


void Widget::setPosition(Point<int> pos)
{
    m_rect.setPosition(pos);
}


Point<int> Widget::position() const
{
    return m_rect.position();
}


void Widget::setSize(Size<int> size)
{
    m_rect.setSize(size);
    if(isWindow())
    {
        m_parent.window->resize(size.w, size.h);
    }
}


Size<int> Widget::size() const
{
    return m_rect.size();
}


int Widget::width() const
{
    return m_rect.width();
}


int Widget::height() const
{
    return m_rect.height();
}


Rect<int> Widget::rect() const
{
    return m_rect;
}


void Widget::show()
{
    if(!isWindow())
    {
        auto window = Window::newInstance(
            width(), height(), "", Window::Type::Image
        );
#ifdef R64FX_DEBUG
        if(!window)
        {
            cerr << "Widget: Failed to create window!\n";
            abort();
        }
#endif//R64FX_DEBUG

        auto painter = Painter::newInstance(window);
#ifdef R64FX_DEBUG
        if(!painter)
        {
            cerr << "Widget: Failed to create painter!\n";
            abort();
        }
#endif//R64FX_DEBUG

        auto ctx = new(nothrow) WindowWidgetContext;
#ifdef R64FX_DEBUG
        if(!ctx)
        {
            cerr << "Widget: Failed to create reconf_ctx!\n";
            abort();
        }
#endif//R64FX_DEBUG

        ctx->widget = this;
        ctx->painter = painter;

        window->setData(ctx);

        m_parent.window = window;
        m_flags |= R64FX_WIDGET_IS_WINDOW;
    }
    m_parent.window->show();
    m_parent.window->resize(width(), height());
    update();
}


void Widget::hide()
{
    if(isWindow())
    {
        m_parent.window->hide();
    }
}


void Widget::close()
{
    if(isWindow())
    {
        auto reconf_ctx = (ReconfContext*) m_parent.window->data();

        Painter::deleteInstance(reconf_ctx->painter());
        Window::deleteInstance(m_parent.window);
        delete reconf_ctx;
        m_parent.window = nullptr;
        m_flags &= ~R64FX_WIDGET_IS_WINDOW;
    }
}


void Widget::update()
{
    m_flags |= R64FX_WIDGET_WANTS_UPDATE;
    auto widget = this;
    while(widget && !(widget->m_flags & R64FX_CHILD_WANTS_UPDATE))
    {
        widget->m_flags |= R64FX_CHILD_WANTS_UPDATE;
        widget = widget->parent();
    }
}


bool Widget::isWindow() const
{
    return m_flags & R64FX_WIDGET_IS_WINDOW;
}


bool Widget::isVisible() const
{
    return m_flags & R64FX_WIDGET_IS_VISIBLE;
}


void Widget::grabMouse()
{
    g_mouse_grabber = this;
}


void Widget::ungrabMouse()
{
    g_mouse_grabber = nullptr;
}


Widget* Widget::mouseGrabber()
{
    return g_mouse_grabber;
}


bool Widget::isMouseGrabber() const
{
    return this == g_mouse_grabber;
}


void Widget::setWindowTitle(std::string title)
{
    if(isWindow())
    {
        m_parent.window->setTitle(title);
    }
}


std::string Widget::windowTitle() const
{
    if(isWindow())
    {
        return m_parent.window->title();
    }
    else
    {
        return "";
    }
}


Point<int> Widget::toRootCoords(Point<int> point) const
{
    point += position();
    if(isWindow() || m_parent.widget == nullptr)
    {
        return point;
    }
    else
    {
        return m_parent.widget->toRootCoords(point);
    }
}


Rect<int> Widget::toRootCoords(Rect<int> rect) const
{
    rect += position();
    if(isWindow() || m_parent.widget == nullptr)
    {
        return rect;
    }
    else
    {
        return m_parent.widget->toRootCoords(rect);
    }
}


Painter* Widget::ReconfContext::painter()
{
    auto ctx = (WindowWidgetContext*) this;
    return ctx->painter;
}


Rect<int> Widget::ReconfContext::visibleRect()
{
    auto ctx = (WindowWidgetContext*) this;
    return ctx->visible_rect;
}


void Widget::processWindowResize(Window* window)
{
    auto ctx = (WindowWidgetContext*) window->data();
    ctx->painter->reconfigure();
    ctx->widget->setSize({window->width(), window->height()});
    ctx->widget->update();
}


void Widget::initReconf(Window* window)
{
    auto ctx = (WindowWidgetContext*) window->data();
    auto widget = ctx->widget;
    if(widget)
    {
        if(widget->m_flags & R64FX_WIDGET_UPDATE_FLAGS)
        {
            ctx->num_rects = 0;
            ctx->visible_rect = {0, 0, widget->width(), widget->height()};
            ctx->got_rect = false;

            if(widget->m_flags & R64FX_WIDGET_WANTS_UPDATE)
            {
                widget->reconfigure((ReconfContext*)ctx);
                ctx->painter->repaint();
            }
            else
            {
                widget->reconfigureChildren(ctx);
                if(ctx->num_rects > 0)
                {
                    for(int i=0; i<ctx->num_rects; i++)
                    {
                        auto rect = ctx->rects[i];
                        rect = intersection(rect, {0, 0, window->width(), window->height()});
                    }

                    ctx->painter->repaint(
                        ctx->rects,
                        ctx->num_rects
                    );
                }
            }
        }
    }
}


void Widget::reconfigure(ReconfContext* ctx)
{
    reconfigureChildren(ctx);
}


void Widget::reconfigureChildren(ReconfContext* reconf_ctx)
{
    auto ctx = (WindowWidgetContext*) reconf_ctx;
    auto parent_visible_rect = ctx->visibleRect();
    bool got_rect = ctx->got_rect;

    if(m_flags & R64FX_WIDGET_WANTS_UPDATE)
    {
        if(!ctx->got_rect)
        {
            if(ctx->num_rects < max_rects)
            {
                ctx->rects[ctx->num_rects] = toRootCoords({0, 0, width(), height()});
                ctx->num_rects++;
                ctx->got_rect = true;
            }
        }

        for(auto child : m_children)
        {
            if(m_flags & R64FX_WIDGET_WANTS_UPDATE)
            {
                auto visible_rect = intersection(child->rect(), parent_visible_rect);
                if(visible_rect.width() > 0 && visible_rect.height() > 0)
                {
                    child->m_flags |= R64FX_WIDGET_IS_VISIBLE;
                    child->m_flags |= R64FX_WIDGET_WANTS_UPDATE;
                }
                else
                {
                    child->m_flags &= ~R64FX_WIDGET_IS_VISIBLE;
                }
            }
        }
    }

    for(auto child : m_children)
    {
        if(child->isVisible() && (child->m_flags & R64FX_WIDGET_UPDATE_FLAGS))
        {
            auto offset = ctx->painter->offset();
            ctx->painter->setOffset(offset + child->position());
            auto visible_rect = intersection(child->rect(), parent_visible_rect);
            ctx->visible_rect = {0, 0, visible_rect.width(), visible_rect.height()};
            if(child->m_flags & R64FX_WIDGET_WANTS_UPDATE)
            {
                child->reconfigure((ReconfContext*)ctx);
            }
            else
            {
                child->reconfigureChildren(ctx);
            }
            ctx->painter->setOffset(offset);
        }
    }

    ctx->got_rect = got_rect;
    ctx->visible_rect = parent_visible_rect;
    set_bits(m_flags, false, R64FX_WIDGET_UPDATE_FLAGS);
}


void Widget::initMousePressEvent(Window* window, MousePressEvent* event)
{
    auto ctx = (WindowWidgetContext*) window->data();

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
        ctx->widget->mousePressEvent(event);
    }
}


void Widget::mousePressEvent(MousePressEvent* event)
{
    for(auto child : m_children)
    {
        if((child->isVisible() && child->rect().overlaps(event->position())))
        {
            auto position = event->position();
            event->setPosition(position - child->position());
            child->mousePressEvent(event);
            event->setPosition(position);
        }
    }
}


void Widget::initMouseReleaseEvent(Window* window, MouseReleaseEvent* event)
{
    auto ctx = (WindowWidgetContext*) window->data();

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
        ctx->widget->mouseReleaseEvent(event);
    }
}


void Widget::mouseReleaseEvent(MouseReleaseEvent* event)
{
    for(auto child : m_children)
    {
        if((child->isVisible() && child->rect().overlaps(event->position())))
        {
            auto position = event->position();
            event->setPosition(position - child->position());
            child->mouseReleaseEvent(event);
            event->setPosition(position);
        }
    }
}


void Widget::initMouseMoveEvent(Window* window, MouseMoveEvent* event)
{
    auto ctx = (WindowWidgetContext*) window->data();

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
         ctx->widget->mouseMoveEvent(event);
    }
}


void Widget::mouseMoveEvent(MouseMoveEvent* event)
{
    for(auto child : m_children)
    {
        if((child->isVisible() && child->rect().overlaps(event->position())))
        {
            auto position = event->position();
            event->setPosition(position - child->position());
            child->mouseMoveEvent(event);
            event->setPosition(position);
        }
    }
}


void Widget::initKeyPressEvent(Window* window, KeyEvent* event)
{
    auto ctx = (WindowWidgetContext*) window->data();
    ctx->widget->keyPressEvent(event);
}


void Widget::keyPressEvent(KeyEvent* event)
{

}


void Widget::initKeyReleaseEvent(Window* window, KeyEvent* event)
{
    auto ctx = (WindowWidgetContext*) window->data();
    ctx->widget->keyReleaseEvent(event);
}


void Widget::keyReleaseEvent(KeyEvent* event)
{

}

}//namespace r64fx
