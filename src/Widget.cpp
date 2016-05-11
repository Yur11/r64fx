#include "Widget.hpp"
#include "Window.hpp"
#include "Mouse.hpp"
#include "Keyboard.hpp"
#include "KeyboardModifiers.hpp"
#include "KeyEvent.hpp"
#include "Clipboard.hpp"
#include "ClipboardEvent.hpp"
#include "Painter.hpp"
#include "Program.hpp"
#include "Timer.hpp"

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


Timer* g_gui_timer = nullptr;


Widget::Widget(Widget* parent)
{
    setParent(parent);
    getsFocusOnClick(true);
    grabsMouseOnClick(true);
    ungrabsMouseOnRelease(true);

    m_flags |= R64FX_WIDGET_IS_VISIBLE;
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
        leaf_offset += leaf->contentOffset();

        got_leaf = false;
        for(auto child : leaf->m_children)
        {
            if(Rect<int>(child->position(), child->size()).overlaps(position - leaf_offset))
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


void Widget::setX(int x)
{
    m_rect.setX(x);
}


int Widget::x() const
{
    return m_rect.x();
}


void Widget::setY(int y)
{
    m_rect.setY(y);
}


int Widget::y() const
{
    return m_rect.y();
}


void Widget::setSize(Size<int> size)
{
    m_rect.setSize(size);
    if(isWindow() && size != m_parent.window->size())
    {
        m_parent.window->setSize(size);
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


Point<int> Widget::toRootCoords(Point<int> point, Widget** root)
{
    auto widget = this;
    for(;;)
    {
        point += widget->position();
        if(widget->parent())
        {
            point += widget->parent()->contentOffset();
            widget = widget->parent();
            continue;
        }
        else
        {
            break;
        }
    }

    if(root)
    {
        root[0] = widget;
    }

    return point;
}


void Widget::recomputeChildrenVisibility(const Rect<int> &clip_rect)
{
    cout << "recomputeChildrenVisibility()\n";

    auto shifted_clip_rect = clip_rect - contentOffset();

    for(auto child : m_children)
    {
        auto visible_rect = intersection(Rect<int>(child->position(), child->size()), shifted_clip_rect);
        if(visible_rect.width() > 0 && visible_rect.height() > 0)
        {
            child->m_flags |= R64FX_WIDGET_IS_VISIBLE;

            if(visible_rect.width() < child->width() || visible_rect.height() < child->height())
            {
                child->m_flags |= R64FX_WIDGET_IS_PARTIALLY_VISIBLE;
            }
            else
            {
                child->m_flags &= ~R64FX_WIDGET_IS_PARTIALLY_VISIBLE;
            }

            child->recomputeChildrenVisibility(Rect<int>(
                visible_rect.position() - child->position(),
                visible_rect.size()
            ));
        }
        else
        {
            child->m_flags &= ~R64FX_WIDGET_IS_VISIBLE;
            child->m_flags &= ~R64FX_WIDGET_IS_PARTIALLY_VISIBLE;
        }
    }
}


void Widget::recomputeChildrenVisibility()
{
    Rect<int> clip_rect(0, 0, width(), height());
    recomputeChildrenVisibility(clip_rect);
}


bool Widget::isVisible() const
{
    return m_flags & R64FX_WIDGET_IS_VISIBLE;
}


Point<int> Widget::contentOffset()
{
    return {0, 0};
}


void Widget::setOrientation(Orientation orientation)
{
    set_bits(m_flags, orientation == Orientation::Vertical, R64FX_WIDGET_IS_VERTICAL);
}


Orientation Widget::orientation() const
{
    return (m_flags & R64FX_WIDGET_IS_VERTICAL) ? Orientation::Vertical : Orientation::Horizontal;
}


void Widget::setPinned(bool yes)
{
    set_bits(m_flags, yes, R64FX_WIDGET_IS_PINNED);
}


bool Widget::isPinned() const
{
    return m_flags & R64FX_WIDGET_IS_PINNED;
}

}//namespace r64fx


#include "WidgetImpl_Paint.cxx"
#include "WidgetImpl_Mouse.cxx"
#include "WidgetImpl_Keyboard.cxx"
#include "WidgetImpl_Clipboard.cxx"
#include "WidgetImpl_Window.cxx"


namespace r64fx{


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
