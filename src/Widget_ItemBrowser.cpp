#include "Widget_ItemBrowser.hpp"
#include "Widget_ScrollArea.hpp"
#include "Widget_ScrollBar.hpp"
#include "Widget_ItemList.hpp"
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


Widget_ItemList* root_list(Widget_ScrollArea* scroll_area)
{
    Widget_ItemList* item_list = nullptr;
    for(auto child : *scroll_area)
    {
        auto item_list_child = dynamic_cast<Widget_ItemList*>(child);
        if(item_list_child)
        {
            item_list = item_list_child;
            break;
        }
    }

    return item_list;
}


Widget_ItemList* root_list(Widget_ItemBrowser* parent)
{
    Widget_ScrollArea* scroll_area = nullptr;
    find_children(parent, &scroll_area);
    if(!scroll_area)
        return nullptr;

    return root_list(scroll_area);
}

}

Widget_ItemBrowser::Widget_ItemBrowser(Widget* parent)
: Widget(parent)
{
    auto wsa = new Widget_ScrollArea(this);
    auto item_list = new Widget_ItemList(wsa);
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


void Widget_ItemBrowser::addItem(Widget_DataItem* item)
{
    auto item_list = root_list(this);
    if(!item_list)
        return;

    item_list->addItem(item);
}


void Widget_ItemBrowser::showVerticalScrollBar(bool yes)
{
    if(yes)
        m_flags |= R64FX_WIDGET_HAS_VERT_SCROLL_BAR;
    else
        m_flags &= ~R64FX_WIDGET_HAS_VERT_SCROLL_BAR;
    rearrange();
}


void Widget_ItemBrowser::showHorizontalScrollBar(bool yes)
{
    if(yes)
        m_flags |= R64FX_WIDGET_HAS_HORI_SCROLL_BAR;
    else
        m_flags &= ~R64FX_WIDGET_HAS_HORI_SCROLL_BAR;
    rearrange();
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

    auto item_list = root_list(scroll_area);
    if(item_list)
    {
        item_list->resizeAndReallign(scroll_area->width());
    }
}


void Widget_ItemBrowser::paintEvent(Widget::PaintEvent* event)
{
    auto p = event->painter();
    unsigned char red[4] = {255, 0, 0, 0};
    p->fillRect({0, 0, width(), height()}, red);

    Widget::paintEvent(event);
}


void Widget_ItemBrowser::resizeEvent(ResizeEvent* event)
{
    rearrange();
    clip();
    repaint();
}


void Widget_ItemBrowser::mousePressEvent(MousePressEvent* event)
{
    Widget::mousePressEvent(event);

    if(event->button() & MouseButton::Left())
    {
        rearrange();
        cout << "click\n";
    }
}

}//namespace r64fx