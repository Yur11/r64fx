#include "Widget_Dummy.hpp"
#include "Painter.hpp"
#include "Mouse.hpp"
#include "Image.hpp"

#include <iostream>
using namespace std;

namespace r64fx{
    
Widget_Dummy::Widget_Dummy(Widget* parent)
: Widget(parent)
{
    changeColor();
}


Widget_Dummy::~Widget_Dummy()
{

}


void Widget_Dummy::paintEvent(PaintEvent* event)
{
    cout << "paint\n";
    auto p = event->painter();
    p->fillRect({0, 0, width(), height()}, m_color);
    Widget::paintEvent(event);
}


void Widget_Dummy::mousePressEvent(MousePressEvent* event)
{
    cout << "Widget_Dummy click!\n";
    changeColor();
    repaint();
}


void Widget_Dummy::mouseReleaseEvent(MouseReleaseEvent* event)
{

}


void Widget_Dummy::mouseMoveEvent(MouseMoveEvent* event)
{

}


void Widget_Dummy::keyPressEvent(KeyPressEvent* event)
{

}


void Widget_Dummy::keyReleaseEvent(KeyReleaseEvent* event)
{

}


void Widget_Dummy::changeColor()
{
    for(int i=0; i<3; i++)
    {
        m_color[i] = (rand() % 127) + 128;
    }
}
    
}//namespace r64fx
