#include "Widget_ItemList.hpp"
#include "Painter.hpp"

#include <iostream>
using namespace std;

namespace r64fx{

Widget_ItemList::Widget_ItemList(Widget* parent)
: Widget_DataItem("", Widget_DataItem::Kind::List, parent)
{

}


void Widget_ItemList::paintEvent(PaintEvent* event)
{
    auto p = event->painter();

    unsigned char bg[4] = {200, 200, 200, 0};

    p->fillRect({{0, 0}, size()}, bg);

    Widget::paintEvent(event);
}


void Widget_ItemList::mousePressEvent(MousePressEvent* event)
{
    Widget::mousePressEvent(event);
}

}//namespace r64fx