#include "Widget_ItemList.hpp"
#include "Painter.hpp"

#include <iostream>
using namespace std;

namespace r64fx{

Widget_ItemList::Widget_ItemList(Widget* parent)
: Widget_DataItem("", Widget_DataItem::Kind::List, parent)
{

}


void Widget_ItemList::addItem(Widget_DataItem* item)
{
    Widget::add(item);
}


void Widget_ItemList::addItem(const std::string &text)
{
    Widget::add(new Widget_DataItem(text));
}


int Widget_ItemList::enumerate(int num)
{
    int n = num;
    for(auto child : *this)
    {
        auto data_item = dynamic_cast<Widget_DataItem*>(child);
        if(data_item)
        {
            n = data_item->enumerate(n);
        }
    }
    return n;
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