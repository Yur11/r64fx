#include "Widget_ItemList.hpp"
#include "Painter.hpp"

#include <iostream>
using namespace std;

namespace r64fx{

Widget_ItemList::Widget_ItemList(Widget* parent)
: Widget_DataItem(parent)
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


void Widget_ItemList::resizeAndReallign()
{
    int max_child_width  = 0;
    int total_height     = 0;

    for(auto child : *this)
    {
        auto data_item = dynamic_cast<Widget_DataItem*>(child);
        if(data_item)
        {
            data_item->resizeAndReallign();
        }

        if(child->width() > max_child_width)
            max_child_width = child->width();

        total_height += child->height();
    }

    int running_y = 0;
    for(auto child : *this)
    {
        child->setWidth(max_child_width);
        child->setX(0);
        child->setY(running_y);
        running_y += child->height();
    }

    setSize({max_child_width, total_height});
}


void Widget_ItemList::updateEvent(UpdateEvent* event)
{
    auto p = event->painter();

    unsigned char bg[4] = {200, 200, 200, 0};

    p->fillRect({{0, 0}, size()}, bg);

    Widget::updateEvent(event);
}

}//namespace r64fx