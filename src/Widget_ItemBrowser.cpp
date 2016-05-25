#include "Widget_ItemBrowser.hpp"
#include "Widget_DataItem.hpp"
#include "Painter.hpp"

#include <iostream>

using namespace std;

namespace r64fx{

Widget_ItemBrowser::Widget_ItemBrowser(Widget* parent)
: Widget_ScrollArea(parent)
{

}


void Widget_ItemBrowser::setRootItem(Widget_DataItem* item)
{
    if(m_root_item)
    {
        for(auto child : *this)
        {
            if(child == m_root_item)
            {
                child->setParent(nullptr);
            }
        }
    }
    m_root_item = item;
    m_root_item->setParent(this);
}


Widget_DataItem* Widget_ItemBrowser::rootItem()
{
    return m_root_item;
}


void Widget_ItemBrowser::paintEvent(Widget::PaintEvent* event)
{
    cout << childrenBoundingRect() << "\n";

    auto p = event->painter();
    unsigned char red[4] = {255, 0, 0, 0};
    p->fillRect({0, 0, width(), height()}, red);
    Widget_ScrollArea::paintEvent(event);
}


void Widget_ItemBrowser::resizeEvent(ResizeEvent* event)
{
    if(m_root_item)
    {
        if(event->width() > m_root_item->width())
        {
            m_root_item->setWidth(event->width());
            
        }
    }
}

}//namespace r64fx