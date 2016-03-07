#include "Widget_ScrollBar.hpp"
#include "Painter.hpp"

namespace r64fx{

namespace{
    void position_changed_stub(Widget_ScrollBar*, void*) {}
}

Widget_ScrollBar::Widget_ScrollBar(Orientation orientation, Widget* parent)
: Widget(parent)
, m_position_changed(position_changed_stub)
{
    setOrientation(orientation);
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


void Widget_ScrollBar::reconfigureEvent(ReconfigureEvent* event)
{
    static unsigned char color[4] = {255, 0, 0, 0};

    auto p = event->painter();
    Rect<int> handle_rect;

    if(orientation() == Orientation::Vertical)
    {
        int handle_length = height() * m_ratio;
        int handle_offset = (height() - handle_length) * m_handle_position;

        handle_rect = {0, handle_offset, width(), handle_length};
    }
    else
    {
        int handle_length = width() * m_ratio;
        int handle_offset = (width() - handle_length) * m_handle_position;

        handle_rect = {handle_offset, 0, handle_length, height()};
    }

    p->fillRect(handle_rect, color);

    Widget::reconfigureEvent(event);
}

}//namespace r64fx