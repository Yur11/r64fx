#include "Widget_ItemBrowser.hpp"
#include "Widget_ScrollArea.hpp"
#include "Widget_ScrollBar.hpp"
#include "Widget_DataItem.hpp"
#include "Painter.hpp"
#include "WidgetFlags.hpp"

#include <iostream>

using namespace std;

namespace r64fx{

namespace{

void find_children(
    Widget_ItemBrowser*           parent,
    Widget_ScrollArea**           scroll_area,
    Widget_ScrollBar_Vertical**   vertical_scroll_bar    = nullptr,
    Widget_ScrollBar_Horizontal** horizontal_scroll_bar  = nullptr
)
{
    for(auto child : *parent)
    {
        if(scroll_area)
        {
            auto widget = dynamic_cast<Widget_ScrollArea*>(child);
            if(widget)
            {
                *scroll_area = widget;
                continue;
            }
        }

        if(vertical_scroll_bar)
        {
            auto widget = dynamic_cast<Widget_ScrollBar_Vertical*>(child);
            if(widget)
            {
                *vertical_scroll_bar = widget;
                continue;
            }
        }

        if(horizontal_scroll_bar)
        {
            auto widget = dynamic_cast<Widget_ScrollBar_Horizontal*>(child);
            if(widget)
            {
                *horizontal_scroll_bar = widget;
                continue;
            }
        }
    }
}

}

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
    rearrange();
    clip();
    repaint();
}


void Widget_ItemBrowser::rearrange()
{
    Widget_ScrollArea*           scroll_area            = nullptr;
    Widget_ScrollBar_Vertical*   vertical_scroll_bar    = nullptr;
    Widget_ScrollBar_Horizontal* horizontal_scroll_bar  = nullptr;

    find_children(this, &scroll_area, &vertical_scroll_bar, &horizontal_scroll_bar);

    if(!scroll_area)
        return;

    if(m_flags & R64FX_WIDGET_HAS_VERT_SCROLL_BAR)
    {
        if(!vertical_scroll_bar)
        {
            vertical_scroll_bar = new Widget_ScrollBar_Vertical(this);
        }
    }
    else
    {
        if(vertical_scroll_bar)
        {
            vertical_scroll_bar->setParent(nullptr);
            delete vertical_scroll_bar;
        }
    }

    if(m_flags & R64FX_WIDGET_HAS_HORI_SCROLL_BAR)
    {
        if(!horizontal_scroll_bar)
        {
            horizontal_scroll_bar = new Widget_ScrollBar_Horizontal(this);
        }
    }
    else
    {
        if(horizontal_scroll_bar)
        {
            horizontal_scroll_bar->setParent(nullptr);
            delete horizontal_scroll_bar;
        }
    }

    if(vertical_scroll_bar)
    {
        if(horizontal_scroll_bar)
        {
            vertical_scroll_bar->setHeight(height() - horizontal_scroll_bar->height());
        }
        else
        {
            vertical_scroll_bar->setHeight(height());
        }
        vertical_scroll_bar->setX(width() - vertical_scroll_bar->width());
        vertical_scroll_bar->setY(0);

        scroll_area->setWidth(width() - vertical_scroll_bar->width());
    }
    else
    {
        scroll_area->setWidth(width());
    }


    if(horizontal_scroll_bar)
    {
        if(vertical_scroll_bar)
        {
            horizontal_scroll_bar->setWidth(width() - vertical_scroll_bar->width());
        }
        else
        {
            horizontal_scroll_bar->setWidth(width());
        }
        horizontal_scroll_bar->setX(0);
        horizontal_scroll_bar->setY(height() - horizontal_scroll_bar->height());

        scroll_area->setHeight(height() - horizontal_scroll_bar->height());
    }
    else
    {
        scroll_area->setHeight(height());
    }

    scroll_area->setPosition({0, 0});
}

}//namespace r64fx