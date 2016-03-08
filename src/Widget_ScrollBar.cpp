#include "Widget_ScrollBar.hpp"
#include "Painter.hpp"

#include <iostream>

using namespace std;


namespace r64fx{

namespace{
    void position_changed_stub(Widget_ScrollBar*, void*) {}
}

Widget_ScrollBar::Widget_ScrollBar(Widget* parent)
: Widget(parent)
, m_position_changed(position_changed_stub)
{

}


void Widget_ScrollBar::setRatio(float ratio)
{
    m_ratio = ratio;
}

float Widget_ScrollBar::ratio() const
{
    return m_ratio;
}


void Widget_ScrollBar::setHandlePosition(float position)
{
    m_handle_position = position;
    if(m_handle_position < 0.0f)
        m_handle_position = 0.0f;
    else if(m_handle_position > 1.0f)
        m_handle_position = 1.0f;
}


float Widget_ScrollBar::handlePosition() const
{
    return m_handle_position;
}


void Widget_ScrollBar::onPositionChanged(void(*callback)(Widget_ScrollBar* scroll_bar, void* data), void* data)
{
    if(callback)
        m_position_changed = callback;
    else
        m_position_changed = position_changed_stub;
    m_position_changed_data = data;
}


Widget_ScrollBar_Vertical::Widget_ScrollBar_Vertical(Widget* parent)
: Widget_ScrollBar(parent)
{

}


void Widget_ScrollBar_Vertical::reconfigureEvent(ReconfigureEvent* event)
{
    static unsigned char fg[4] = {255, 0, 0, 0};
    static unsigned char bg[4] = {127, 180, 255, 0};

    auto p = event->painter();
    p->fillRect({0, 0, width(), height()}, bg); //Remove me!

    int handle_length = height() * m_ratio;
    int handle_offset = (height() - handle_length) * m_handle_position;
    Rect<int> handle_rect(0, handle_offset, width(), handle_length);

    p->fillRect(handle_rect, fg);
    Widget::reconfigureEvent(event);
}


void Widget_ScrollBar_Vertical::mouseMoveEvent(MouseMoveEvent* event)
{
    if(!(event->button() & MouseButton::Left()) || event->dy() == 0)
        return;

    int length = height() - height() * m_ratio;
    float step = float(event->dy()) / float(length);
    setHandlePosition(handlePosition() + step);
    update();
}



Widget_ScrollBar_Horizontal::Widget_ScrollBar_Horizontal(Widget* parent)
: Widget_ScrollBar(parent)
{

}


void Widget_ScrollBar_Horizontal::reconfigureEvent(ReconfigureEvent* event)
{
    static unsigned char color[4] = {255, 0, 0, 0};

    auto p = event->painter();
    Rect<int> handle_rect;

    int handle_length = width() * m_ratio;
    int handle_offset = (width() - handle_length) * m_handle_position;
    handle_rect = {handle_offset, 0, handle_length, height()};

    p->fillRect(handle_rect, color);
    Widget::reconfigureEvent(event);
}


void Widget_ScrollBar_Horizontal::mouseMoveEvent(MouseMoveEvent* event)
{
    if(!(event->button() & MouseButton::Left()))
        return;
}

}//namespace r64fx