#include "Widget_View.hpp"

namespace r64fx{

Widget_View::Widget_View(Widget* parent) : Widget(parent)
{

}


Widget_View::~Widget_View()
{

}


void Widget_View::mousePressEvent(MousePressEvent* event)
{
    event->setPosition(event->position() - offset());
    Widget::mousePressEvent(event);
}


void Widget_View::mouseReleaseEvent(MouseReleaseEvent* event)
{
    event->setPosition(event->position() - offset());
    Widget::mouseReleaseEvent(event);
}


void Widget_View::mouseMoveEvent(MouseMoveEvent* event)
{
    event->setPosition(event->position() - offset());
    Widget::mouseMoveEvent(event);
}

}//namespace r64fx