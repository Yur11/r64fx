#include "Widget_DirectoryItem.hpp"
#include "WidgetFlags.hpp"

#include <iostream>
using namespace std;

namespace r64fx{

Widget_DirectoryItem::Widget_DirectoryItem(const std::string &caption, const std::string path, bool is_directory, Widget* parent)
: Widget_DataItem(caption, (is_directory ? Widget_DirectoryItem::Kind::Tree : Widget_DataItem::Kind::Plain), parent)
, m_path(path)
{
    if(is_directory)
    {
        m_flags |= R64FX_WIDGET_ITEM_IS_DIRECTORY;
        if(!m_path.empty() && m_path.back() != '/')
        {
            m_path.push_back('/');
        }
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


void Widget_DirectoryItem::populate()
{
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
    collapse();
}


void Widget_DirectoryItem::loadEntry(const Directory::Entry* entry)
{
    if(entry->name()[0] != '.')
    {
        if(entry->isDirectory())
        {
            auto dt = new Widget_DirectoryItem(entry->name(), entry->name(), true, this);
            dt->populate();
        }
        else
        {
            auto di = new Widget_DirectoryItem(entry->name(), entry->name(), false, this);
            (void)di;
        }
    }
}

}//namespace r64fx