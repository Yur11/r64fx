#include "Widget_Control.hpp"
#include "Mouse.hpp"

namespace r64fx{

namespace{
    void on_value_changed_stub(Widget_Control*, void*) {}
}


Widget_Control::Widget_Control(Widget* parent) : Widget(parent)
{
    onValueChanged(nullptr);
}


Widget_Control::~Widget_Control()
{

}


float Widget_Control::minValue() const
{
    return m_min_value;
}


float Widget_Control::maxValue() const
{
    return m_max_value;
}


void Widget_Control::setValueRange(float minval, float maxval)
{

}


float Widget_Control::value() const
{
    return m_value;
}


void Widget_Control::onValueChanged(void (*callback)(Widget_Control*, void*), void* data)
{
    if(callback == nullptr)
    {
        m_on_value_changed = on_value_changed_stub;
    }
    m_on_value_changed_data = data;
}


void Widget_Control::mousePressEvent(MousePressEvent* event)
{
    if(event->button() == MouseButton::Left())
    {
        m_mouse_pos = event->position();
    }
    Widget::mousePressEvent(event);
}


void Widget_Control::mouseMoveEvent(MouseMoveEvent* event)
{
    if(event->button() == MouseButton::Left())
    {
        float dy = m_mouse_pos.y() - event->y();
        if(dy != 0)
        {
            m_mouse_pos = event->position();
        }
    }
}

}//namespace r64fx