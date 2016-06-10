#include "Widget_ItemTree.hpp"
#include "WidgetFlags.hpp"
#include "Painter.hpp"
#include "Font.hpp"

#include <iostream>
using namespace std;

namespace r64fx{

Widget_ItemTree::Widget_ItemTree(const std::string &caption, Widget* parent)
: Widget_DataItem(caption, Widget_DataItem::Kind::Tree, parent)
{

}


Widget_ItemTree::Widget_ItemTree(Widget* parent)
: Widget_DataItem("", Widget_DataItem::Kind::Tree, parent)
{

}


void Widget_ItemTree::mousePressEvent(MousePressEvent* event)
{
    if(event->button() & MouseButton::Left())
    {
        if(event->y() <= lineHeight())
        {
            if(event->x() < (lineHeight() * (indent() + 1)))
            {
                if(isCollapsed())
                {
                    expand();
                }
                else
                {
                    collapse();
                }
            }
            else
            {
                Widget_DataItem::mousePressEvent(event);
            }
        }
        else
        {
            Widget::mousePressEvent(event);
        }
    }
    else
    {
        Widget::mousePressEvent(event);
    }
}

}//namespace