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

    Widget*          g_mouse_grabber   = nullptr;
    ReconfContext    g_reconf_ctx;
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
        m_parent.window = Window::newInstance(
            width(), height(), "", Window::Type::Image
        );
        m_parent.window->setPainter(Painter::newInstance(m_parent.window));
        m_parent.window->setWidget(this);
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
        Painter::deleteInstance(m_parent.window->painter());
        Window::deleteInstance(m_parent.window);
        m_parent.window = nullptr;
        m_flags ^= R64FX_WIDGET_IS_WINDOW;
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


Window* Widget::rootWindow() const
{
    if(isWindow())
    {
        return m_parent.window;
    }
    else if(m_parent.widget != nullptr)
    {
        return m_parent.widget->parentWindow();
    }
    else
    {
        return nullptr;
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


void Widget::initReconf(Window* window)
{
    auto widget = window->widget();
    if(widget)
    {
        if(widget->m_flags & R64FX_WIDGET_UPDATE_FLAGS)
        {
            g_reconf_ctx.clearRects();
            g_reconf_ctx.setPainter(window->painter());
            g_reconf_ctx.setVisibleRect({0, 0, widget->width(), widget->height()});
            g_reconf_ctx.got_rect = false;

            if(widget->m_flags & R64FX_WIDGET_WANTS_UPDATE)
            {
                widget->reconfigure(&g_reconf_ctx);
                window->painter()->repaint();
            }
            else
            {
                widget->reconfigureChildren(&g_reconf_ctx);
                if(g_reconf_ctx.num_rects > 0)
                {
                    for(int i=0; i<g_reconf_ctx.num_rects; i++)
                    {
                        auto rect = g_reconf_ctx.rects[i];
                        rect = intersection(rect, {0, 0, window->width(), window->height()});
                    }

                    window->painter()->repaint(
                        g_reconf_ctx.rects,
                        g_reconf_ctx.num_rects
                    );
                }
            }
        }
    }
}


void Widget::initMousePressEvent(Window* window, MousePressEvent* event)
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


void Widget::initMouseReleaseEvent(Window* window, MouseReleaseEvent* event)
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


void Widget::initMouseMoveEvent(Window* window, MouseMoveEvent* event)
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


void Widget::initKeyPressEvent(Window* window, KeyEvent* event)
{
    window->widget()->keyPressEvent(event);
}


void Widget::initKeyReleaseEvent(Window* window, KeyEvent* event)
{
    window->widget()->keyReleaseEvent(event);
}


void Widget::reconfigure(ReconfContext* ctx)
{
    reconfigureChildren(ctx);
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


void Widget::keyPressEvent(KeyEvent* event)
{

}


void Widget::keyReleaseEvent(KeyEvent* event)
{

}


void Widget::reconfigureChildren(ReconfContext* ctx)
{
    auto painter = ctx->painter();
    auto parent_visible_rect = ctx->visibleRect();
    bool got_rect = ctx->got_rect;

    if(m_flags & R64FX_WIDGET_WANTS_UPDATE)
    {
        if(!ctx->got_rect)
        {
            ctx->addRect(toRootCoords({0, 0, width(), height()}));
            ctx->got_rect = true;
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
            auto offset = painter->offset();
            painter->setOffset(offset + child->position());
            auto visible_rect = intersection(child->rect(), parent_visible_rect);
            ctx->setVisibleRect({0, 0, visible_rect.width(), visible_rect.height()});
            if(child->m_flags & R64FX_WIDGET_WANTS_UPDATE)
            {
                child->reconfigure(ctx);
            }
            else
            {
                child->reconfigureChildren(ctx);
            }
            painter->setOffset(offset);
        }
    }

    ctx->got_rect = got_rect;
    ctx->m_visible_rect = parent_visible_rect;
    set_bits(m_flags, false, R64FX_WIDGET_UPDATE_FLAGS);
}

}//namespace r64fx
