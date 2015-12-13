#include "Widget.hpp"
#include "Window.hpp"
#include "Mouse.hpp"
#include "KeyEvent.hpp"
#include "ReconfContext.hpp"
#include "Painter.hpp"
#include "Image.hpp"
#include "Painter.hpp"

#include <map>

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

    Widget* g_mouse_grabber = nullptr;
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
            width(), height(), "", Window::Type::Normal
        );
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
        Window::deleteInstance(m_parent.window);
        m_parent.window = nullptr;
        m_flags ^= R64FX_WIDGET_IS_WINDOW;
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


bool Widget::isObscuredLeft() const
{
    return m_flags & R64FX_WIDGET_IS_OBSCURED_LEFT;
}


bool Widget::isObscuredTop() const
{
    return m_flags & R64FX_WIDGET_IS_OBSCURED_TOP;
}


bool Widget::isObscuredRight() const
{
    return m_flags & R64FX_WIDGET_IS_OBSCURED_RIGHT;
}


bool Widget::isObscuredBottom() const
{
    return m_flags & R64FX_WIDGET_IS_OBSCURED_BOTTOM;
}


bool Widget::isPartiallyObscured() const
{
    return m_flags & R64FX_WIDGET_IS_OBSCURED;
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


void Widget::reconfigure(ReconfContext* ctx)
{
    reconfigureChildren(ctx);
}


void Widget::mousePressEvent(MousePressEvent* event)
{
    m_flags &= ~R64FX_CHILD_WANTS_UPDATE;
    for(auto child : m_children)
    {
        if((child->isVisible() && child->rect().overlaps(event->position())))
        {
            auto position = event->position();
            event->setPosition(position - child->position());
            child->m_flags &= ~R64FX_WIDGET_UPDATE_FLAGS;
            child->mousePressEvent(event);
            if(child->m_flags & R64FX_WIDGET_UPDATE_FLAGS)
            {
                m_flags |= R64FX_CHILD_WANTS_UPDATE;
            }
            event->setPosition(position);
        }
    }
}


void Widget::mouseReleaseEvent(MouseReleaseEvent* event)
{
    m_flags &= ~R64FX_CHILD_WANTS_UPDATE;
    for(auto child : m_children)
    {
        if((child->isVisible() && child->rect().overlaps(event->position())))
        {
            auto position = event->position();
            event->setPosition(position - child->position());
            child->m_flags &= ~R64FX_WIDGET_UPDATE_FLAGS;
            child->mouseReleaseEvent(event);
            if(child->m_flags & R64FX_WIDGET_UPDATE_FLAGS)
            {
                m_flags |= R64FX_CHILD_WANTS_UPDATE;
            }
            event->setPosition(position);
        }
    }
}


void Widget::mouseMoveEvent(MouseMoveEvent* event)
{
    m_flags &= ~R64FX_CHILD_WANTS_UPDATE;
    for(auto child : m_children)
    {
        if((child->isVisible() && child->rect().overlaps(event->position())))
        {
            auto position = event->position();
            event->setPosition(position - child->position());
            child->m_flags &= ~R64FX_WIDGET_UPDATE_FLAGS;
            child->mouseMoveEvent(event);
            if(child->m_flags & R64FX_WIDGET_UPDATE_FLAGS)
            {
                m_flags |= R64FX_CHILD_WANTS_UPDATE;
            }
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


void Widget::update()
{
    m_flags |= R64FX_WIDGET_WANTS_UPDATE;
}


void Widget::reconfigureChildren(ReconfContext* ctx)
{
    auto painter = ctx->painter();

    bool got_rect = ctx->got_rect;
    if((m_flags & R64FX_WIDGET_WANTS_UPDATE) && !ctx->got_rect)
    {
        ctx->addRect(toRootCoords({0, 0, width(), height()}));
        ctx->got_rect = true;
    }

    if(!m_children.isEmpty())
    {
        /* Recalculate children visibility. */
        if(m_flags & R64FX_WIDGET_WANTS_UPDATE)
        {
            for(auto child : m_children)
            {
                unsigned long flags = 0;

                if(child->rect().right()    < 0              ||
                    child->rect().bottom()  < 0              ||
                    child->rect().left()    > rect().width() ||
                    child->rect().top()     > rect().height() )
                {
                    flags &= ~R64FX_WIDGET_IS_VISIBLE;
                }
                else
                {
                    flags |= R64FX_WIDGET_IS_VISIBLE;

                    if(child->rect().left() < 0)
                    {
                        flags |= R64FX_WIDGET_IS_OBSCURED_LEFT;
                    }
                    else
                    {
                        flags &= ~R64FX_WIDGET_IS_OBSCURED_LEFT;
                    }

                    if(child->rect().top() < 0)
                    {
                        flags |= R64FX_WIDGET_IS_OBSCURED_TOP;
                    }
                    else
                    {
                        flags &= ~R64FX_WIDGET_IS_OBSCURED_TOP;
                    }

                    if(child->rect().right() > rect().width())
                    {
                        flags |= R64FX_WIDGET_IS_OBSCURED_RIGHT;
                    }
                    else
                    {
                        flags &= ~R64FX_WIDGET_IS_OBSCURED_RIGHT;
                    }

                    if(child->rect().bottom() > rect().height())
                    {
                        flags |= R64FX_WIDGET_IS_OBSCURED_BOTTOM;
                    }
                    else
                    {
                        flags &= ~R64FX_WIDGET_IS_OBSCURED_BOTTOM;
                    }
                }//if

                child->m_flags =
                    (child->m_flags & ~R64FX_WIDGET_VISIBILITY_FLAGS) | flags;

                /* Set the update flag on the widget.*/
                if(child->isVisible())
                {
                    child->update();
                }
            }//for every child
        }//if wants update

        for(auto child : m_children)
        {
            if(child->isVisible() && (child->m_flags & R64FX_WIDGET_UPDATE_FLAGS))
            {
                auto offset = painter->offset();
                painter->setOffset(offset + child->position());
                if(child->m_flags & R64FX_WIDGET_WANTS_UPDATE)
                {
                    child->reconfigure(ctx);
                }
                else//Child wants update
                {
                    child->reconfigureChildren(ctx);
                }
                painter->setOffset(offset);
            }//if
        }//for every child
    }//if has children

    ctx->got_rect = got_rect;
    set_bits(m_flags, false, R64FX_WIDGET_UPDATE_FLAGS);
}

}//namespace r64fx
