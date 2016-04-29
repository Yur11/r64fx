#include "Widget_ItemTree.hpp"
#include "Painter.hpp"
#include "Font.hpp"

#include <iostream>
using namespace std;

namespace r64fx{

namespace{

}//namespace


Widget_ItemTree::Widget_ItemTree(Widget* parent)
: Widget_DataItem(parent)
{

}


void Widget_ItemTree::addItem(Widget_DataItem* item)
{
    Widget::add(item);
}


void Widget_ItemTree::updateEvent(UpdateEvent* event)
{
    auto p = event->painter();

    unsigned char bg[4] = {200, 200, 200, 0};

    p->fillRect({{0, 0}, size()}, bg);

    Widget_DataItem::updateEvent(event);
}


}//namespace