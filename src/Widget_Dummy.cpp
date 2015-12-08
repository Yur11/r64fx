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


void Widget_Dummy::reconfigure(Painter* painter)
{
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
    
}//namespace r64fx
