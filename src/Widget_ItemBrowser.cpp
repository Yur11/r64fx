#include "Widget_ItemBrowser.hpp"
#include "Widget_ScrollArea.hpp"
#include "Widget_ScrollBar.hpp"
#include "Widget_DataItem.hpp"
#include "Painter.hpp"
#include "WidgetFlags.hpp"

#include <iostream>

using namespace std;

namespace r64fx{

Widget_ItemBrowser::Widget_ItemBrowser(Widget* parent)
: Widget(parent)
{
    auto wsa = new Widget_ScrollArea(this);
    m_flags |= R64FX_WIDGET_HAS_VERT_SCROLL_BAR;
    m_flags |= R64FX_WIDGET_HAS_HORI_SCROLL_BAR;
    rearrange();
}


Widget_ItemBrowser::~Widget_ItemBrowser()
{
    for(auto child : *this)
    {
        child->setParent(nullptr);
        delete child;
    }
}


// void Widget_ItemBrowser::setRootItem(Widget_DataItem* item)
// {
//     if(m_root_item)
//     {
//         for(auto child : *this)
//         {
//             if(child == m_root_item)
//             {
//                 child->setParent(nullptr);
//             }
//         }
//     }
//     m_root_item = item;
//     m_root_item->setParent(this);
// }
//
//
// Widget_DataItem* Widget_ItemBrowser::rootItem()
// {
//     return m_root_item;
// }


Widget_ScrollArea* Widget_ItemBrowser::scrollArea()
{
    return nullptr;
}


void Widget_ItemBrowser::showVerticalScrollBar(bool yes)
{
    if(yes)
        m_flags |= R64FX_WIDGET_HAS_VERT_SCROLL_BAR;
    else
        m_flags &= ~R64FX_WIDGET_HAS_VERT_SCROLL_BAR;
    rearrange();
}


Widget_ScrollBar* Widget_ItemBrowser::verticalScrollBar()
{
    return nullptr;
}


void Widget_ItemBrowser::showHorizontalScrollBar(bool yes)
{
    if(yes)
        m_flags |= R64FX_WIDGET_HAS_HORI_SCROLL_BAR;
    else
        m_flags &= ~R64FX_WIDGET_HAS_HORI_SCROLL_BAR;
    rearrange();
}


Widget_ScrollBar* Widget_ItemBrowser::horizontalScrollBar()
{
    return nullptr;
}


void Widget_ItemBrowser::paintEvent(Widget::PaintEvent* event)
{
    cout << childrenBoundingRect() << "\n";

    auto p = event->painter();
    unsigned char red[4] = {255, 0, 0, 0};
    p->fillRect({0, 0, width(), height()}, red);
    Widget::paintEvent(event);
}


void Widget_ItemBrowser::resizeEvent(ResizeEvent* event)
{
//     if(m_root_item)
//     {
//         if(event->width() > m_root_item->width())
//         {
//             m_root_item->setWidth(event->width());
//
//         }
//     }
}


void Widget_ItemBrowser::rearrange()
{
    Widget_ScrollArea* scroll_area = (Widget_ScrollArea*) *begin();
    if(!scroll_area)
        return;

    scroll_area->setParent(nullptr);

    Widget_ScrollBar* vert_sb = nullptr;
    Widget_ScrollBar* hori_sb = nullptr;

    if(m_flags & R64FX_WIDGET_HAS_VERT_SCROLL_BAR)
    {
        vert_sb = (Widget_ScrollBar*) *begin();
        if(!vert_sb)
        {
            vert_sb = new Widget_ScrollBar_Vertical;
        }
        else
        {
            vert_sb->setParent(nullptr);
        }
    }

    if(m_flags & R64FX_WIDGET_HAS_HORI_SCROLL_BAR)
    {
        hori_sb = (Widget_ScrollBar*) *begin();
        if(!hori_sb)
        {
            hori_sb = new Widget_ScrollBar_Horizontal;
        }
        else
        {
            hori_sb->setParent(nullptr);
        }
    }

    scroll_area->setParent(this);

    if(vert_sb)
    {
        vert_sb->setParent(this);
    }

    if(hori_sb)
    {
        hori_sb->setParent(this);
    }
}

}//namespace r64fx