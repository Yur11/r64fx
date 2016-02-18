#include "Widget_Control.hpp"
#include "Mouse.hpp"
#include "Painter.hpp"

namespace r64fx{

Widget_Control::Widget_Control(Widget* parent) : Widget(parent)
{

}


Widget_Control::~Widget_Control()
{

}


void Widget_Control::reconfigureEvent(ReconfigureEvent* event)
{
    auto p = event->painter();
    p->fillRect({127, 127, 127}, {0, 0, width(), height()});
    Widget::reconfigureEvent(event);
}


void Widget_Control::mousePressEvent(MousePressEvent* event)
{
    Widget::mousePressEvent(event);
}


void Widget_Control::mouseMoveEvent(MouseMoveEvent* event)
{

}

}//namespace r64fx