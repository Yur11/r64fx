#include "Widget_Dummy.hpp"
#include "Painter.hpp"
#include "ReconfigureEvent.hpp"

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


void Widget_Dummy::mousePressEvent(MouseEvent* event)
{
    Widget::mousePressEvent(event);
}


void Widget_Dummy::mouseReleaseEvent(MouseEvent* event)
{
    Widget::mouseReleaseEvent(event);
}


void Widget_Dummy::mouseMoveEvent(MouseEvent* event)
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


void Widget_Dummy::reconfigureEvent(ReconfigureEvent* event)
{
    auto p = event->painter();
    if(isPartiallyObscured())
    {
        p->fillRect({0, 0, 0}, {{0, 0}, size()});
    }
    else
    {
        p->fillRect(m_Color, {{0, 0}, size()});
    }
    Widget::reconfigureEvent(event);
}
    
}//namespace r64fx
