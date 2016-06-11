#include "Widget_DirectoryItem.hpp"
#include "WidgetFlags.hpp"
#include "Keyboard.hpp"
#include "KeyboardModifiers.hpp"
#include "KeyEvent.hpp"
#include "Clipboard.hpp"
#include "ClipboardEvent.hpp"

#include <iostream>
using namespace std;

namespace r64fx{

Widget_DirectoryItem::Widget_DirectoryItem(const std::string &caption, const std::string path, Widget* parent)
: Widget_DataItem(caption, parent)
, m_path(path)
{
    if(m_path.empty())
    {
        m_path = "/";
    }

    if(m_path.back() == '/')
    {
        m_flags |= R64FX_WIDGET_ITEM_IS_DIRECTORY;
        setKind(Widget_DataItem::Kind::Tree);
    }
    else
    {
        setKind(Widget_DataItem::Kind::Plain);
    }
}


Widget_DirectoryItem::~Widget_DirectoryItem()
{
    if(isPopulated())
    {
        depopulate();
    }
}


std::string Widget_DirectoryItem::path() const
{
    return m_path;
}


std::string Widget_DirectoryItem::fullPath() const
{
    auto parent_widget = parent();

    if(!parent_widget)
        return m_path;

    auto parent_item = dynamic_cast<Widget_DirectoryItem*>(parent_widget);
    if(!parent_item)
        return m_path;

    return parent_item->fullPath() + m_path;
}


bool Widget_DirectoryItem::isDirectory() const
{
    return m_flags & R64FX_WIDGET_ITEM_IS_DIRECTORY;
}


bool Widget_DirectoryItem::isPopulated() const
{
    return m_flags & R64FX_WIDGET_DIRECTORY_IS_POPULATED;
}


void Widget_DirectoryItem::collapse()
{
    if(isPopulated())
    {
        bool has_expanded_child = false;
        for(auto child : *this)
        {
            auto data_item = dynamic_cast<Widget_DataItem*>(child);
            if(data_item && data_item->isExpanded())
            {
                has_expanded_child = true;
                break;
            }
        }

        if(!has_expanded_child)
        {
            depopulate();
        }
    }
    Widget_DataItem::collapse();
}


void Widget_DirectoryItem::expand()
{
    if(!isPopulated())
    {
        populate();
    }
    Widget_DataItem::expand();
}


void Widget_DirectoryItem::keyPressEvent(KeyPressEvent* event)
{
    if(Keyboard::CtrlDown())
    {
        if(event->key() == Keyboard::Key::C)
        {
            anounceClipboardData({"text/uri-list", "text/plain"}, ClipboardMode::Clipboard);
        }
        else if(event->key() == Keyboard::Key::V)
        {
            requestClipboardMetadata(ClipboardMode::Clipboard);
        }
    }
}


void Widget_DirectoryItem::clipboardDataRecieveEvent(ClipboardDataRecieveEvent* event)
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


void Widget_DirectoryItem::clipboardDataTransmitEvent(ClipboardDataTransmitEvent* event)
{
    string clipboard_message = "";

    if(event->type() == "text/plain")
    {
        clipboard_message = caption();
    }
    else if(event->type() == "text/uri-list")
    {
        clipboard_message = "file://" + fullPath();
        if(clipboard_message.back() == '/')
        {
            clipboard_message.pop_back();
        }
        clipboard_message.push_back('\n');
    }

    if(!clipboard_message.empty())
        event->transmit((void*)clipboard_message.c_str(), clipboard_message.size());
}


void Widget_DirectoryItem::clipboardMetadataRecieveEvent(ClipboardMetadataRecieveEvent* event)
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


void Widget_DirectoryItem::populate()
{
    if(!isDirectory())
        return;

    Directory dir(fullPath());
    if(!dir.isOpen())
    {
        cerr << "Failed to load directory " << fullPath() << "\n";
        return;
    }

    dir.forEachEntry([](const Directory::Entry* entry, void* arg){
        auto self = (Widget_DirectoryItem*) arg;
        self->loadEntry(entry);
    }, this);

    resizeAndReallign(0);

    m_flags |= R64FX_WIDGET_DIRECTORY_IS_POPULATED;
}


void Widget_DirectoryItem::loadEntry(const Directory::Entry* entry)
{
    if(entry->name()[0] != '.')
    {
        string entry_path = entry->name();
        if(entry->isDirectory() && entry_path.back() != '/')
        {
            entry_path.push_back('/');
        }

        auto dt = new Widget_DirectoryItem(entry->name(), entry_path, this);
        (void)dt;
    }
}


void Widget_DirectoryItem::depopulate()
{
    for(;;)
    {
        auto child = popFirstChild();
        if(!child)
            break;

        delete child;
    }

    m_flags &= ~R64FX_WIDGET_DIRECTORY_IS_POPULATED;
}

}//namespace r64fx