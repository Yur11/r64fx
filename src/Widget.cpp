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

#include <limits>

#ifdef R64FX_DEBUG
#include <iostream>
#include <assert.h>

using namespace std;
#endif//R64FX_DEBUG

#include "WidgetFlags.hpp"

namespace r64fx{

LinkedList<Widget> g_windowed_widgets;

constexpr int max_rects = 32;

struct WidgetImpl{
    Window* m_window = nullptr;

    /* Root widget shown in the window that
     * this context is attached to. */
    Widget*  m_root_widget = nullptr;

    /* Painter serving the window. */
    Painter* m_painter = nullptr;

    inline Painter* painter() const { return m_painter; }

    /* List of rectangles to be repainted after update cycle. */
    Rect<int> m_rects[max_rects];

    /* Number of rectangles that must be repainted. */
    int m_num_rects = 0;

    /* Used in paint logic. */
    bool m_got_rect = false;

    Point<int> m_parent_global_position = {0, 0};

    Rect<int> m_visible_rect = {0, 0};

    void clip();

    void clipChildren(Widget* parent);

    void repaint();

    void paintChildren(Widget* parent);
};


Timer* g_gui_timer = nullptr;


Widget::Widget(Widget* parent)
{
    setParent(parent);
    m_flags |= R64FX_WIDGET_IS_VISIBLE;
}


Widget::~Widget()
{
    for(auto child : m_children)
    {
        child->setParent(nullptr);
    }
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

    if(m_parent.widget)
    {
        m_parent.widget->m_children.remove(this);
    }

    if(parent)
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

Widget* Widget::popFirstChild()
{
    if(m_children.isEmpty())
        return nullptr;
    auto child = m_children.first();
    child->setParent(nullptr);
    return child;
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
    m_position = pos;
}


Point<int> Widget::position() const
{
    return m_position;
}


void Widget::setX(int x)
{
    m_position.setX(x);
}


int Widget::x() const
{
    return m_position.x();
}


void Widget::setY(int y)
{
    m_position.setY(y);
}


int Widget::y() const
{
    return m_position.y();
}


void Widget::setSize(Size<int> size, bool send_event)
{
    m_size = size;
    if(isWindow() && size != m_parent.window->size())
    {
        m_parent.window->setSize(size);
    }

    if(send_event)
    {
        ResizeEvent event(size);
        resizeEvent(&event);
    }
}


Size<int> Widget::size() const
{
    return m_size;
}


void Widget::setWidth(int width, bool send_event)
{
    setSize({width, height()}, send_event);
}


int Widget::width() const
{
    return m_size.width();
}


void Widget::setHeight(int height, bool send_event)
{
    setSize({width(), height}, send_event);
}


int Widget::height() const
{
    return m_size.height();
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


Rect<int> Widget::childrenBoundingRect() const
{
    int min_left   = std::numeric_limits<int>::max();
    int min_top    = std::numeric_limits<int>::max();
    int max_right  = 0;
    int max_bottom = 0;

    for(auto child : m_children)
    {
        Rect<int> rect(child->position(), child->size());

        if(rect.left() < min_left)
            min_left = rect.left();

        if(rect.top() < min_top)
            min_top = rect.top();

        if(rect.right() > max_right)
            max_right = rect.right();

        if(rect.bottom() > max_bottom)
            max_bottom = rect.bottom();
    }

    return Rect<int>(min_left, min_top, max_right - min_left, max_bottom - min_top);
}


void Widget::setOrientation(Orientation orientation)
{
    if(orientation == Orientation::Vertical)
        m_flags |= R64FX_WIDGET_IS_VERTICAL;
    else
        m_flags &= ~R64FX_WIDGET_IS_VERTICAL;
}


Orientation Widget::orientation() const
{
    return (m_flags & R64FX_WIDGET_IS_VERTICAL) ? Orientation::Vertical : Orientation::Horizontal;
}

}//namespace r64fx


#include "WidgetImpl_Paint.cxx"
#include "WidgetImpl_Mouse.cxx"
#include "WidgetImpl_Keyboard.cxx"
#include "WidgetImpl_Clipboard.cxx"
#include "WidgetImpl_Window.cxx"


namespace r64fx{


void Widget::paintEvent(Widget::PaintEvent* event)
{
    event->impl()->paintChildren(this);
}


void Widget::resizeEvent(ResizeEvent* event)
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


void Widget::dndFinishedEvent(DndFinishedEvent* event)
{

}


void Widget::closeEvent()
{
    Program::quit();
}

}//namespace r64fx
