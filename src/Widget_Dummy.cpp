#include "Widget_Dummy.hpp"
#include "Painter.hpp"
#include "Mouse.hpp"
#include "Image.hpp"

#include <iostream>
using namespace std;

namespace r64fx{
    
Widget_Dummy::Widget_Dummy(Color<unsigned char> color, Widget* parent)
: Widget(parent)
, m_Color(color)
{

}


Widget_Dummy::~Widget_Dummy()
{

}


void Widget_Dummy::reconfigureEvent(ReconfigureEvent* event)
{
    auto painter = event->painter();
    Rect<int> rect = {0, 0, width(), height()};

    if(rect != event->visibleRect())
    {
        painter->fillRect({63, 63, 63}, {{0, 0}, size()});
    }
    else if(on)
    {
        painter->fillRect({127, 127, 127}, {{0, 0}, size()});
    }
    else
    {
        painter->fillRect(m_Color, {{0, 0}, size()});
    }

    Widget::reconfigureEvent(event);

    painter->fillRect({0, 0, 0}, {m_Point, {10, 10}});
}


void Widget_Dummy::mousePressEvent(MousePressEvent* event)
{
    Widget::mousePressEvent(event);

    if(!event->handled)
    {
        if(event->button() == MouseButton::Left())
        {
            on = true;
            event->handled = true;
            m_Point = event->position();
            grabMouse();
            update();
        }
    }
}


void Widget_Dummy::mouseReleaseEvent(MouseReleaseEvent* event)
{
    Widget::mouseReleaseEvent(event);

    if(!event->handled)
    {
        if(event->button() == MouseButton::Left())
        {
            on = false;
            event->handled = true;
            ungrabMouse();
            update();
        }
    }
}


void Widget_Dummy::mouseMoveEvent(MouseMoveEvent* event)
{
    Widget::mouseMoveEvent(event);
/*
    if(!event->handled)
    {
        event->handled = true;
        m_Point = event->position();
        update();
    }*/
}


void Widget_Dummy::keyPressEvent(KeyPressEvent* event)
{
    Widget::keyPressEvent(event);
}


void Widget_Dummy::keyReleaseEvent(KeyReleaseEvent* event)
{
    Widget::keyReleaseEvent(event);
}
    
}//namespace r64fx
