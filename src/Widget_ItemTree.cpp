#include "Widget_ItemTree.hpp"
#include "Painter.hpp"
#include "Font.hpp"

#include <iostream>
using namespace std;

namespace r64fx{

namespace{

}//namespace


Widget_ItemTree::Widget_ItemTree(const std::string &text, Widget* parent)
: Widget_DataItem(text, parent)
{

}


Widget_ItemTree::Widget_ItemTree(Widget* parent)
: Widget_DataItem(parent)
{

}


void Widget_ItemTree::addItem(Widget_DataItem* item)
{
    Widget::add(item);
}


void Widget_ItemTree::addItem(const std::string &text)
{
    Widget::add(new Widget_DataItem(text));
}


void Widget_ItemTree::resizeAndReallign()
{
    Widget_DataItem::resizeAndReallign();

    int item_offset      = height();
    int max_child_width  = width();
    int total_height     = height();

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

    int running_y = height();
    for(auto child : *this)
    {
        child->setWidth(max_child_width);
        child->setX(item_offset);
        child->setY(running_y);
        running_y += child->height();
    }

    setSize({max_child_width + item_offset, total_height});
}


void Widget_ItemTree::collapse()
{

}


void Widget_ItemTree::expand()
{

}


void Widget_ItemTree::updateEvent(UpdateEvent* event)
{
    auto p = event->painter();

    unsigned char bg[4] = {200, 200, 200, 0};

    p->fillRect({{0, 0}, size()}, bg);

    Widget_DataItem::updateEvent(event);
}


}//namespace