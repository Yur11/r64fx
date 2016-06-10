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


void Widget_ItemTree::addItem(Widget_DataItem* item)
{
    Widget::add(item);
}


void Widget_ItemTree::addItem(const std::string &caption)
{
    Widget::add(new Widget_DataItem(caption));
}


void Widget_ItemTree::resizeAndReallign(int min_width)
{
    static int depth = 0;
    depth++;

    Widget_DataItem::resizeAndReallign(min_width);

    if(!isCollapsed())
    {
        int max_child_width  = width();
        int total_height     = height();

        for(auto child : *this)
        {
            auto data_item = dynamic_cast<Widget_DataItem*>(child);
            if(data_item)
            {
                data_item->resizeAndReallign(min_width);
            }

            if(child->width() > max_child_width)
                max_child_width = child->width();

            total_height += child->height();
        }

        int running_y = height();
        for(auto child : *this)
        {
            child->setWidth(max_child_width);
            child->setX(0);
            child->setY(running_y);
            running_y += child->height();
        }

        setHeight(total_height);
    }

    depth--;
}


int Widget_ItemTree::enumerate(int num)
{
    int n = Widget_DataItem::enumerate(num);
    if(isCollapsed())
        return n;

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