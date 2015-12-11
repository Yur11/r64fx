#include "Widget_Dummy.hpp"
#include "ReconfContext.hpp"
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


void Widget_Dummy::reconfigure(ReconfContext* ctx)
{
    auto painter = ctx->painter();

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
    Widget::reconfigure(ctx);
}


void Widget_Dummy::mousePressEvent(MousePressEvent* event)
{
    Widget::mousePressEvent(event);

    if(!event->handled)
    {
        auto p = toRootCoords(event->position());
        cout << p.x() << ", " << p.y() << "\n";

        event->handled = true;
        m_Color = {
            255, 0, 0
        };

        trackMouseRelease(true);
        update();
    }
}


void Widget_Dummy::mouseReleaseEvent(MouseReleaseEvent* event)
{
    Widget::mouseReleaseEvent(event);

    if(!event->handled)
    {
        auto p = toRootCoords(event->position());
        cout << p.x() << ", " << p.y() << "\n";

        event->handled = true;
        m_Color = {
            0, 255, 0
        };

        trackMouseRelease(false);
        update();
    }
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
