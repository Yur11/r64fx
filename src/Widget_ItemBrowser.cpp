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


void Widget_ItemBrowser::rearrange()
{
    Widget_ScrollArea*           scroll_area            = nullptr;
    Widget_ScrollBar_Vertical*   vertical_scroll_bar    = nullptr;
    Widget_ScrollBar_Horizontal* horizontal_scroll_bar  = nullptr;

    find_children(this, &scroll_area, &vertical_scroll_bar, &horizontal_scroll_bar);

    if(!scroll_area)
        return;

    scroll_area->setSize(this->size());

    auto item_list = root_list(scroll_area);
    if(item_list)
    {
        Rect<int> cbr = item_list->childrenBoundingRect();
        if(cbr.height() > scroll_area->height())
        {
            if(!vertical_scroll_bar)
            {
                vertical_scroll_bar = new Widget_ScrollBar_Vertical(this);
                vertical_scroll_bar->onPositionChanged([](Widget_ScrollBar* sb, void* arg){
                    auto wib = (Widget_ItemBrowser*) arg;
                    wib->scrollTo(sb->handlePosition());
                }, this);
            }
        }
        else
        {
            if(vertical_scroll_bar)
            {
                vertical_scroll_bar->setParent(nullptr);
                delete vertical_scroll_bar;
                vertical_scroll_bar = nullptr;
            }
        }

        if(vertical_scroll_bar)
        {
            vertical_scroll_bar->setRatio(float(scroll_area->height()) / float(item_list->height()));
        }
    }

    if(vertical_scroll_bar)
    {
        vertical_scroll_bar->setHeight(height());
        vertical_scroll_bar->setX(width() - vertical_scroll_bar->width());
        vertical_scroll_bar->setY(0);

        scroll_area->setWidth(width() - vertical_scroll_bar->width());
    }

    scroll_area->setHeight(height());

    scroll_area->setPosition({0, 0});

    if(item_list)
    {
        item_list->resizeAndReallign(scroll_area->width());
    }
}


void Widget_ItemBrowser::scrollTo(float position)
{
    Widget_ScrollArea*           scroll_area            = nullptr;
    Widget_ScrollBar_Vertical*   vertical_scroll_bar    = nullptr;
    Widget_ScrollBar_Horizontal* horizontal_scroll_bar  = nullptr;

    find_children(this, &scroll_area, &vertical_scroll_bar, &horizontal_scroll_bar);

    if(!scroll_area)
        return;

    auto item_list = root_list(scroll_area);
    if(!item_list)
        return;

    int scroll_area_height = scroll_area->height();
    int item_list_height = item_list->height();
    int slack_height = item_list_height - scroll_area_height;
    int offset = -int(float(slack_height) * position);
    scroll_area->setOffset({0, offset});
    clip();
    repaint();
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
        clip();
        repaint();
    }
    else
    {
        Widget_ScrollArea*          scroll_area          = nullptr;
        Widget_ScrollBar_Vertical*  vertical_scroll_bar  = nullptr;

        find_children(this, &scroll_area, &vertical_scroll_bar);

        if(scroll_area)
        {
            Widget_ItemList* item_list = root_list(scroll_area);

            int scroll_area_height = scroll_area->height();
            int item_list_height = item_list->height();
            int slack_height = item_list_height - scroll_area_height;

            if(event->button() & MouseButton::WheelUp())
            {
                scroll_area->setOffset(scroll_area->offset() + Point<int>(0, 20));
                if(scroll_area->offset().y() > 0)
                {
                    scroll_area->setOffset({0, 0});
                }
            }
            else if(event->button() & MouseButton::WheelDown())
            {
                scroll_area->setOffset(scroll_area->offset() - Point<int>(0, 20));
                if(scroll_area->offset().y() < (-slack_height))
                {
                    scroll_area->setOffset({0, -slack_height});
                }
            }

            if(vertical_scroll_bar)
            {
                float handle_position = float(-scroll_area->offset().y()) / float(slack_height);
                vertical_scroll_bar->setHandlePosition(handle_position);
            }

            clip();
            repaint();
        }
    }
}

}//namespace r64fx