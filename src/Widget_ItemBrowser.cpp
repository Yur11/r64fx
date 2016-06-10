#include "Widget_ItemBrowser.hpp"
#include "Widget_ScrollArea.hpp"
#include "Widget_ScrollBar.hpp"
#include "Widget_DataItem.hpp"
#include "Painter.hpp"
#include "WidgetFlags.hpp"
#include "Keyboard.hpp"
#include "KeyboardModifiers.hpp"
#include "KeyEvent.hpp"
#include "Clipboard.hpp"
#include "ClipboardEvent.hpp"

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


    Widget_DataItem* root_list(Widget_ScrollArea* scroll_area)
    {
        Widget_DataItem* item_list = nullptr;
        for(auto child : *scroll_area)
        {
            auto item_list_child = dynamic_cast<Widget_DataItem*>(child);
            if(item_list_child)
            {
                item_list = item_list_child;
                break;
            }
        }

        return item_list;
    }


    Widget_DataItem* root_list(Widget_ItemBrowser* parent)
    {
        Widget_ScrollArea* scroll_area = nullptr;
        find_children(parent, &scroll_area);
        if(!scroll_area)
            return nullptr;

        return root_list(scroll_area);
    }

    void on_selected_item_stub(Widget_ItemBrowser*, void*) {}
}

Widget_ItemBrowser::Widget_ItemBrowser(Widget* parent)
: Widget(parent)
, m_on_item_selected(on_selected_item_stub)
{
    auto wsa = new Widget_ScrollArea(this);
    auto item_list = new Widget_DataItem("", Widget_DataItem::Kind::List, wsa);
    (void) item_list;
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

        int scroll_area_height = scroll_area->height();
        int item_list_height = item_list->height();
        int slack_height = item_list_height - scroll_area_height;

        if(slack_height > 0)
        {
            if((-scroll_area->offset().y()) > slack_height)
            {
                scroll_area->setOffset({0, -slack_height});
            }
        }
        else
        {
            scroll_area->setOffset({0, 0});
        }

        if(vertical_scroll_bar)
        {
            vertical_scroll_bar->setRatio(float(scroll_area->height()) / float(item_list->height()));

            float handle_position = float(-scroll_area->offset().y()) / float(slack_height);
            vertical_scroll_bar->setHandlePosition(handle_position);
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


Widget_DataItem* Widget_ItemBrowser::selectedItem() const
{
    return m_selected_item;
}


void Widget_ItemBrowser::onItemSelected(void (*callback)(Widget_ItemBrowser* browser, void* data), void* data)
{
    if(callback)
    {
        m_on_item_selected = callback;
        m_on_item_selected_data = data;
    }
    else
    {
        m_on_item_selected = on_selected_item_stub;
        m_on_item_selected_data = nullptr;
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
    setFocus();
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
            Widget_DataItem* item_list = root_list(scroll_area);

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


void Widget_ItemBrowser::keyPressEvent(KeyPressEvent* event)
{
    if(Keyboard::CtrlDown())
    {
        if(event->key() == Keyboard::Key::C)
        {
            auto selected_item = selectedItem();
            if(selected_item)
            {
                anounceClipboardData({"text/uri-list", "text/plain"}, ClipboardMode::Clipboard);
            }
        }
        else if(event->key() == Keyboard::Key::V)
        {
            requestClipboardMetadata(ClipboardMode::Clipboard);
        }
    }
}


void Widget_ItemBrowser::clipboardDataRecieveEvent(ClipboardDataRecieveEvent* event)
{
    if(event->mode() != ClipboardMode::Clipboard)
        return;

    if(event->data() == nullptr && event->size() <= 0)
        return;

    if(event->type() == "text/plain")
    {
        string text((const char*)event->data(), event->size());
        cout << "text/plain:\n" << text << "\n";
    }
    else if(event->type() == "text/uri-list")
    {
        string text((const char*)event->data(), event->size());
        cout << "text/uri-list:\n" << text << "\n";
    }
}


void Widget_ItemBrowser::clipboardDataTransmitEvent(ClipboardDataTransmitEvent* event)
{
    string clipboard_message = "";

    auto selected_item = selectedItem();
    if(selected_item)
    {
        if(event->type() == "text/plain")
        {
            clipboard_message = selected_item->caption();
        }
        else if(event->type() == "text/uri-list")
        {
            
        }
    }

    if(!clipboard_message.empty())
        event->transmit((void*)clipboard_message.c_str(), clipboard_message.size());
}


void Widget_ItemBrowser::clipboardMetadataRecieveEvent(ClipboardMetadataRecieveEvent* event)
{
    if(event->mode() != ClipboardMode::Clipboard)
        return;

    static const ClipboardDataType types[2] = {"text/uri-list", "text/plain"};

    for(int i=0; i<2; i++)
    {
        if(event->has(types[i]))
        {
            requestClipboardData(types[i], event->mode());
            break;
        }
    }
}


void Widget_ItemBrowser::setSelectedItem(Widget_DataItem* item)
{
    m_selected_item = item;
    m_on_item_selected(this, m_on_item_selected_data);
}

}//namespace r64fx