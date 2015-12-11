#include "Widget_Dummy.hpp"
#include "Painter.hpp"
#include "Mouse.hpp"

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


void Widget_Dummy::reconfigure(Painter* painter)
{
    static int i = 0;
    cout << "Widget_Dummy::reconfigure " << (i++) << "\n";

    if(isObscuredLeft() || isObscuredRight())
    {
        painter->fillRect({0, 0, 0}, {{0, 0}, size()});
    }
    else if(isObscuredTop() || isObscuredBottom())
    {
        painter->fillRect({127, 127, 127}, {{0, 0}, size()});
    }
    else
    {
        painter->fillRect(m_Color, {{0, 0}, size()});
    }
    Widget::reconfigure(painter);
}


void Widget_Dummy::mousePressEvent(MousePressEvent* event)
{
    m_Color = {
        m_Color.green(),
        m_Color.blue(),
        m_Color.red()
    };

    cout << "press:   " << event->x() << "x" << event->y() << "\n";
    cout << event->button().code() << "\n";
    Widget::mousePressEvent(event);
    update();
}


void Widget_Dummy::mouseReleaseEvent(MouseReleaseEvent* event)
{
    cout << "release: " << event->x() << "x" << event->y() << "\n";
    Widget::mouseReleaseEvent(event);
}


void Widget_Dummy::mouseMoveEvent(MouseMoveEvent* event)
{
    Widget::mouseMoveEvent(event);
}


void Widget_Dummy::keyPressEvent(KeyEvent* event)
{
    Widget::keyPressEvent(event);
}


void Widget_Dummy::keyReleaseEvent(KeyEvent* event)
{
    Widget::keyReleaseEvent(event);
}
    
}//namespace r64fx
