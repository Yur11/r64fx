#include "Widget.hpp"
#include "Widget_View.hpp"
#include "Window.hpp"
#include "Mouse.hpp"
#include "Keyboard.hpp"
#include "KeyboardModifiers.hpp"
#include "KeyEvent.hpp"
#include "Clipboard.hpp"
#include "ClipboardEvent.hpp"
#include "Painter.hpp"
#include "Program.hpp"

#ifdef R64FX_DEBUG
#include <iostream>
#include <assert.h>

using namespace std;
#endif//R64FX_DEBUG

#include "WidgetFlags.hpp"

namespace r64fx{

void set_bits(unsigned long &flags, const bool yes, unsigned long mask)
{
    if(yes)
        flags |= mask;
    else
        flags &= ~mask;
}


Widget::Widget(Widget* parent)
{
    setParent(parent);
    getsFocusOnClick(true);
    grabsMouseOnClick(true);
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


Widget* Widget::root() const
{
    if(isWindow() || !m_parent.widget)
    {
        return (Widget* const)this;
    }
    else
    {
        return m_parent.widget->root();
    }
}


Widget* Widget::leafAt(Point<int> position, Point<int>* offset)
{
    Widget* leaf = this;
    Point<int> leaf_offset = {0, 0};
    bool got_leaf = true;
    while(got_leaf)
    {
        auto widget_view = dynamic_cast<Widget_View*>(leaf);
        if(widget_view)
        {
            leaf_offset += widget_view->offset();
        }

        got_leaf = false;
        for(auto child : m_children)
        {
            if(child->rect().overlaps(position - leaf_offset))
            {
                got_leaf = true;
                leaf_offset += child->position();
                leaf = child;
                break;
            }
        }
    }

    if(offset)
    {
        offset[0] = leaf_offset;
    }
    return leaf;
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
    if(isWindow() && size != Size<int>(m_parent.window->width(), m_parent.window->height()))
    {
        m_parent.window->resize(size.width(), size.height());
    }
    ResizeEvent event(size);
    resizeEvent(&event);
}


Size<int> Widget::size() const
{
    return m_rect.size();
}


void Widget::setWidth(int width)
{
    setSize({width, height()});
}


int Widget::width() const
{
    return m_rect.width();
}


void Widget::setHeight(int height)
{
    setSize({width(), height});
}


int Widget::height() const
{
    return m_rect.height();
}


Rect<int> Widget::rect() const
{
    return m_rect;
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
        auto view = dynamic_cast<const Widget_View*>(m_parent.widget);
        if(view)
        {
            point += view->offset();
        }
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


bool Widget::isVisible() const
{
    return m_flags & R64FX_WIDGET_IS_VISIBLE;
}


// void Widget::setOrientation(Orientation orientation)
// {
//     set_bits(m_flags, orientation == Orientation::Vertical, R64FX_WIDGET_IS_VERTICAL);
// }
//
//
// Orientation Widget::orientation() const
// {
//     return (m_flags & R64FX_WIDGET_IS_VERTICAL) ? Orientation::Vertical : Orientation::Horizontal;
// }
//
//
// void Widget::setPinned(bool yes)
// {
//     set_bits(m_flags, yes, R64FX_WIDGET_IS_PINNED);
// }
//
//
// bool Widget::isPinned() const
// {
//     return m_flags & R64FX_WIDGET_IS_PINNED;
// }

}//namespace r64fx


#include "WidgetImpl_Mouse.cxx"
#include "WidgetImpl_Keyboard.cxx"
#include "WidgetImpl_Clipboard.cxx"
#include "WidgetImpl_Window.cxx"


namespace r64fx{

void Widget::update()
{
    m_flags |= R64FX_WIDGET_WANTS_UPDATE;
    auto widget = parent();
    while(widget && !(widget->m_flags & R64FX_WIDGET_UPDATE_FLAGS))
    {
        widget->m_flags |= R64FX_CHILD_WANTS_UPDATE;
        widget = widget->parent();
    }
}


Painter* Widget::UpdateEvent::painter()
{
    auto d = (WindowWidgetData*) this;
    return d->painter;
}


void Widget::UpdateEvent::setOffset(Point<int> offset)
{
    auto d = (WindowWidgetData*) this;
    d->painter->setOffset(offset);
}


Point<int> Widget::UpdateEvent::offset() const
{
    auto d = (WindowWidgetData*) this;
    return d->painter->offset();
}


Rect<int> Widget::UpdateEvent::visibleRect()
{
    auto d = (WindowWidgetData*) this;
    return d->visible_rect;
}


void process_window_updates(Window* window, void*)
{
    window->makeCurrent();

    auto d        = (WindowWidgetData*) window->data();
    auto painter  = d->painter;
    auto widget   = d->widget;

    if(widget->m_flags & R64FX_WIDGET_UPDATE_FLAGS)
    {
        painter->adjustForWindowSize();

        d->num_rects = 0;
        d->visible_rect = {0, 0, widget->width(), widget->height()};
        d->got_rect = false;

        if(widget->m_flags & R64FX_WIDGET_WANTS_UPDATE)
        {
            widget->updateEvent((Widget::UpdateEvent*)d);
            painter->repaint();
        }
        else
        {
            widget->updateChildren((Widget::UpdateEvent*)d);
            if(d->num_rects > 0)
            {
                for(int i=0; i<d->num_rects; i++)
                {
                    auto rect = d->rects[i];
                    rect = intersection(rect, {0, 0, window->width(), window->height()});
                }

                painter->repaint(
                    d->rects,
                    d->num_rects
                );
            }
        }
    }
}


void Widget::processEvents()
{
    Window::processSomeEvents(&g_events);
    Window::forEach(process_window_updates, nullptr);
}


void Widget::updateEvent(Widget::UpdateEvent* event)
{
    updateChildren(event);
}


void Widget::updateChildren(Widget::UpdateEvent* event)
{
    auto d = (WindowWidgetData*) event;
    auto parent_visible_rect = d->visible_rect;
    bool got_rect = d->got_rect;

    if(m_flags & R64FX_WIDGET_WANTS_UPDATE)
    {
        /* Calculate a rectangle to process on the window surface
           if we don't aleady have one for one of the parents. */
        if(!d->got_rect)
        {
            if(d->num_rects < max_rects)
            {
                d->rects[d->num_rects] = intersection(
                    Rect<int>(d->painter->offset(), size()), d->painter->clipRect()
                );
                d->num_rects++;
                d->got_rect = true;
            }
        }

        /* Calculate children visibility. */
        for(auto child : m_children)
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

    Point<int> view_offset = {0, 0};
    auto widget_view_self = dynamic_cast<Widget_View*>(this);//We may have offsets.
    if(widget_view_self)
    {
        view_offset = widget_view_self->offset();
    }

    /* Recursively process children. */
    for(auto child : m_children)
    {
        if(child->isVisible() && (child->m_flags & R64FX_WIDGET_UPDATE_FLAGS))
        {
            auto offset = d->painter->offset();

            d->painter->setOffset(
                offset + child->position() + view_offset
            );

            Rect<int> clip_rect;
            if(widget_view_self)//?
            {
                clip_rect = d->painter->clipRect();
                d->painter->setClipRect(toRootCoords({
                    0, 0, width(), height()
                }));
            }

            auto visible_rect = intersection(child->rect(), parent_visible_rect);
            d->visible_rect = {0, 0, visible_rect.width(), visible_rect.height()};

            if((child->m_flags & R64FX_WIDGET_WANTS_UPDATE))
            {
                child->updateEvent((UpdateEvent*)d);
            }
            else
            {
                child->updateChildren((UpdateEvent*)d);
            }

            if(widget_view_self)//?
            {
                d->painter->setClipRect(clip_rect);
            }

            d->painter->setOffset(offset);
        }
    }

    d->got_rect = got_rect;
    d->visible_rect = parent_visible_rect;
    set_bits(m_flags, false, R64FX_WIDGET_UPDATE_FLAGS);
}


void Widget::resizeEvent(ResizeEvent* event)
{

}


void Widget::mousePressEvent(MousePressEvent* event)
{

}


void Widget::mouseReleaseEvent(MouseReleaseEvent* event)
{

}


void Widget::mouseMoveEvent(MouseMoveEvent* event)
{

}


void Widget::mouseEnterEvent()
{

}


void Widget::mouseLeaveEvent()
{

}


void Widget::focusInEvent()
{

}


void Widget::focusOutEvent()
{

}


void Widget::keyPressEvent(KeyPressEvent* event)
{

}


void Widget::keyReleaseEvent(KeyReleaseEvent* event)
{

}


void Widget::textInputEvent(TextInputEvent* event)
{

}


void Widget::clipboardDataRecieveEvent(ClipboardDataRecieveEvent* event)
{

}


void Widget::clipboardDataTransmitEvent(ClipboardDataTransmitEvent* event)
{

}


void Widget::clipboardMetadataRecieveEvent(ClipboardMetadataRecieveEvent* event)
{

}


void Widget::dndEnterEvent(DndEnterEvent* event)
{

}


void Widget::dndLeaveEvent(DndLeaveEvent* event)
{

}


void Widget::dndMoveEvent(DndMoveEvent* event)
{

}


void Widget::closeEvent()
{
    Program::quit();
}

}//namespace r64fx
