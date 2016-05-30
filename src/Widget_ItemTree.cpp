#include "Widget_ItemTree.hpp"
#include "WidgetFlags.hpp"
#include "Painter.hpp"
#include "Font.hpp"

#include <iostream>
using namespace std;

namespace r64fx{

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
                data_item->setTreeDepth(depth);
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


void Widget_ItemTree::collapse()
{
    m_flags |= R64FX_WIDGET_TREE_IS_COLLAPSED;
    auto root_item = rootDataItem();
    auto root_item_parent = root_item->parent();
    if(root_item_parent)
    {
        root_item->resizeAndReallign(root_item_parent->width());
        root_item_parent->clip();
        root_item_parent->repaint();
    }
}


void Widget_ItemTree::expand()
{
    m_flags &= ~R64FX_WIDGET_TREE_IS_COLLAPSED;
    auto root_item = rootDataItem();
    auto root_item_parent = root_item->parent();
    if(root_item_parent)
    {
        root_item->resizeAndReallign(root_item_parent->width());
        root_item_parent->clip();
        root_item_parent->repaint();
    }
}


bool Widget_ItemTree::isCollapsed()
{
    return m_flags & R64FX_WIDGET_TREE_IS_COLLAPSED;
}


void Widget_ItemTree::paintEvent(PaintEvent* event)
{
    auto p = event->painter();

    unsigned char bg[4] = {200, 200, 200, 0};

    p->fillRect({{0, 0}, size()}, bg);

    Widget_DataItem::paintEvent(event);
}


void Widget_ItemTree::mousePressEvent(MousePressEvent* event)
{
    if(event->y() > lineHeight())
    {
        Widget::mousePressEvent(event);
    }
    else if(event->button() & MouseButton::Left())
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
}

}//namespace